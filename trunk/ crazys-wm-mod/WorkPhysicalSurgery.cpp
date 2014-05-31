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

bool cJobManager::WorkPhysicalSurgery(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";

	// not for patient
	g_Girls.UnequipCombat(girl);

	bool hasDoctor = false;
	if (g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_DOCTOR, true) > 0 || g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_DOCTOR, false) > 0)
		hasDoctor = true;

	if (!hasDoctor)
	{
		message = girl->m_Realname + gettext(" does nothing. You don't have any Doctor (require 1) ");
		(DayNight == 0) ? message += gettext("day") : message += gettext("night"); message += gettext(" Shift.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
		return true;
	}

	if (DayNight == 0)	// the Doctor works on her durring the day
	{
		girl->m_WorkingDay++;
	}
	else	// and if there are nurses on duty, they take care of her at night
	{
		if (g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, 1) > 0)
		{
			girl->m_WorkingDay++;
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, 10);
			g_Girls.UpdateStat(girl, STAT_MANA, 10);
		}
	}

	int numnurse = g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, DayNight == 0);

	stringstream ss;
	if (girl->m_WorkingDay >= 5)
	{
		ss << "The surgery is a success.";
		if (numnurse > 1)
		{
			ss << "The Nurses kept her healthy and happy during her recovery.";
			g_Girls.UpdateStat(girl, STAT_SPIRIT, 5);
			g_Girls.UpdateStat(girl, STAT_MANA, 10);
			g_Girls.UpdateStat(girl, STAT_BEAUTY, 15);
			g_Girls.UpdateStat(girl, STAT_CHARISMA, 15);
		}
		else if (numnurse > 0)
		{
			ss << "The Nurse helped her during her recovery.";
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5);
			g_Girls.UpdateStat(girl, STAT_HEALTH, -10);
			g_Girls.UpdateStat(girl, STAT_MANA, -10);
			g_Girls.UpdateStat(girl, STAT_BEAUTY, 12);
			g_Girls.UpdateStat(girl, STAT_CHARISMA, 12);
		}
		else
		{
			ss << "She is sad and has lost some health during the operation.";
			g_Girls.UpdateStat(girl, STAT_SPIRIT, -5);
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, -15);
			g_Girls.UpdateStat(girl, STAT_HEALTH, -20);
			g_Girls.UpdateStat(girl, STAT_MANA, -20);
			g_Girls.UpdateStat(girl, STAT_BEAUTY, 8);
			g_Girls.UpdateStat(girl, STAT_CHARISMA, 8);
		}
		if (!g_Girls.HasTrait(girl, "Sexy Air"))
		{
			girl->add_trait("Sexy Air", false);
			ss << "She gains Sexy Air trait.";
		}
		else if (!g_Girls.HasTrait(girl, "Cute"))
		{
			girl->add_trait("Cute", false);
			ss << "She gains Cute trait.";
		}

		if (g_Girls.HasTrait(girl, "Fragile")){ g_Girls.UpdateStat(girl, STAT_HEALTH, -5); }
		else if (g_Girls.HasTrait(girl, "Tough")){ g_Girls.UpdateStat(girl, STAT_HEALTH, 5); }
		if (g_Girls.HasTrait(girl, "Pessimist")){ g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5); }
		else if (g_Girls.HasTrait(girl, "Optimist")){ g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5); }

		girl->m_WorkingDay = 0;
		girl->m_DayJob = JOB_CLINICREST;
		girl->m_NightJob = JOB_CLINICREST;
	}
	else
	{
		ss << "The operation is in progess (" << (5 - girl->m_WorkingDay) << " day remaining).";
		ss << "Having a Nurse on duty will speed up her recovery.";
	}

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, DayNight);



	// Improve girl
	int libido = 1;
	if (g_Girls.HasTrait(girl, "Lesbian"))		libido += numnurse;
	if (g_Girls.HasTrait(girl, "Masochist"))	libido += 1;
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))	libido += 2;
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
