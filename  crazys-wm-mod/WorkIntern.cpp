/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "cJobManager.h"
#include "cBrothel.h"
#include "cClinic.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cClinicManager g_Clinic;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Job Clinic - Staff
bool cJobManager::WorkIntern(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKINTERN;

	stringstream ss; string girlName = girl->m_Realname;
	if (g_Girls.HasTrait(girl, "AIDS"))
	{
		ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so " << girlName << " was sent to the waiting room.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		return false;
	}
	ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " trains in the Medical field.\n\n";

	cConfig cfg;
	int enjoy = 0, wages = 0, skill = 0, sgMed = 0, sgInt = 0, sgCha = 0, train = 0;
	double roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();


	/* */if (roll_a <= 5)	skill = 7;
	else if (roll_a <= 15)	skill = 6;
	else if (roll_a <= 30)	skill = 5;
	else if (roll_a <= 60)	skill = 4;
	else /*             */	skill = 3;
	/* */if (g_Girls.HasTrait(girl, "Quick Learner"))	{ skill += 2; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; }

	int tmed = g_Girls.GetSkill(girl, SKILL_MEDICINE);		// train = 1
	int tint = g_Girls.GetStat(girl, STAT_INTELLIGENCE);	// train = 2
	int tcha = g_Girls.GetStat(girl, STAT_CHARISMA);		// train = 3
	do{		// `J` New method of selecting what job to do
		/* */if (roll_b < 40 && tmed < 100)		train = 1;	// medicine
		else if (roll_b < 70 && tint < 100)		train = 2;	// intelligence
		else if (roll_b < 100 && tcha < 100)	train = 3;	// charisma
		roll_b -= 30;
	} while (train == 0 && roll_b > 0);

	if (train == 0 && girl->medicine() > 70 && girl->intelligence() > 70)
	{
		ss << girlName << " has completed her Internship and has been promoted to ";
		if (girl->is_slave())	{ ss << "Nurse.";	girl->m_DayJob = girl->m_NightJob = JOB_NURSE; }
		else /*            */	{ ss << "Doctor.";	girl->m_DayJob = girl->m_NightJob = JOB_DOCTOR; }
		return false;
	}
	if (train == 1)
	{
		ss << gettext("She learns how to work with medicine better.\n");
		sgMed = skill;
	}
	else sgMed= g_Dice % 3;
	if (train == 2)
	{
		ss << gettext("She got smarter today.\n");
		sgInt = skill;
	}
	else sgInt = g_Dice % 2;
	if (train == 3)
	{
		ss << gettext("She got more charismatic today.\n");
		sgCha = skill;
	}
	else sgCha = g_Dice % 2;

	if (sgMed > 0)
	{
		ss << gettext("She managed to gain ") << sgMed << gettext(" Medicine skill.\n\n");
		g_Girls.UpdateSkill(girl, SKILL_MEDICINE, sgMed);
	}
	if (sgInt > 0)
	{
		ss << gettext("She managed to gain ") << sgInt << gettext(" Intelligence.\n\n");
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, sgInt);
	}
	if (sgCha > 0)
	{
		ss << gettext("She managed to gain ") << sgCha << gettext(" Charisma.\n\n");
		g_Girls.UpdateStat(girl, STAT_CHARISMA, sgCha);
	}



	//enjoyed the work or not
	/* */if (roll_c <= 10)	{ enjoy -= g_Dice % 3 + 1;	ss << "Some of the patrons abused her during the shift."; }
	else if (roll_c >= 90)	{ enjoy += g_Dice % 3 + 1;	ss << "She had a pleasant time working."; }
	else /*             */	{ enjoy += g_Dice % 2;		ss << "Otherwise, the shift passed uneventfully."; }


	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket())) { wages = 0; }
	else { wages = 25 + (skill * 5); } // `J` Pay her more if she learns more

	girl->m_Pay = wages;

	// Improve stats
	int xp = 5 + skill, libido = int(1 + skill / 2);

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);


	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy, true);
	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, actiontype, "Dealing with patients and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", Day0Night1);
	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, actiontype, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);

	return false;
}

double cJobManager::JP_Intern(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{
		jobperformance +=
			(100 - girl->intelligence()) +
			(100 - girl->medicine()) +
			(100 - girl->charisma());
	}
	else// for the actual check
	{
	}
	return jobperformance;
}