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
#include "cMovieStudio.h"
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
extern cMovieStudioManager g_Studios;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkCameraMage(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	char buffer[1000];
	int jobperformance = 0;
	string girlName = girl->m_Realname;

	g_Girls.UnequipCombat(girl);

	// `J` added this to allow the Director to assign someone to this job without making it permanent
	if (girl->m_DayJob == JOB_FILMFREETIME)	// the director sets the old job to dayjob when changing night job
	{
		message = girlName;
		message += gettext(" worked as a cameramage.\n\n");
	}
	else
	{
		message = "The Director assigned " + girlName + "to run the camera for the week ";
	}

	int roll = g_Dice%100;
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		if (girl->m_DayJob == JOB_FILMFREETIME)
		{
			message = girl->m_Realname + gettext(" refused to work as a cameramage today.");
			girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		}
		else
		{
			if (g_Studios.is_Actress_Job(girl->m_DayJob))	message += "but she wanted to be in front of the camera rather than behind it.\n";
			else if (girl->m_DayJob == JOB_PROMOTER)		message += "but she preferred to sell the movies instead of make them.\n";
			else if (girl->m_DayJob == JOB_FLUFFER)			message += "but she was having a bad hair day and needed some cum in it to make it cooperate.\n";
			else if (girl->m_DayJob == JOB_STAGEHAND)		message += "but she wanted to move around more than the camera would allow her.\n";
			else if (girl->m_DayJob == JOB_CRYSTALPURIFIER) message += "but she preferred to edit the scenes rather than makeing new ones.\n";
			girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_BACKTOWORK);
		}
		return true;
	}
	else if(roll <= 15) 
	{
		message += gettext(" She did not like working in the studio today.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, -1, true);
		jobperformance += -5;
	}
	else if(roll >=90)
	{
		message += gettext(" She had a great time working today.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +3, true);
		jobperformance += 5;
	}
	else
	{
		message += gettext(" Otherwise, the shift passed uneventfully.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +1, true);
	}

/*
 *	work out the pay between the house and the girl
 *
 *	the original calc took the average of beauty and charisma and halved it
 */
	girl->m_Pay += 20;

	int roll_max = girl->spirit() + girl->intelligence();
	roll_max /= 4;
	girl->m_Pay += 10 + g_Dice%roll_max;
	
	jobperformance += (girl->spirit() - 50) / 10;
	jobperformance += (girl->intelligence() - 50) / 10;
	jobperformance += g_Girls.GetSkill(girl, SKILL_SERVICE) / 10;
	jobperformance /= 3;
	jobperformance += g_Girls.GetStat(girl, STAT_LEVEL);
	jobperformance += g_Girls.GetStat(girl, STAT_FAME)/20;
	jobperformance += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP
	
	g_Studios.m_CameraQuality += jobperformance;

	if (jobperformance > 0)
	{
		message += girlName + gettext(" helped improve the scene ");
		_itoa(jobperformance, buffer, 10);
		message += buffer;
		message += "% with her camera skills. \n";
	}
	else if (jobperformance < 0)
	{
		message += girlName + gettext(" did a bad job today, she reduced the scene quality ");
		_itoa(jobperformance, buffer, 10);
		message += buffer;
		message += "% with her poor performance. \n";
	}
	else
		message += girlName + gettext(" did not really effect the scene quality.\n");

	// Improve stats
	int xp = 5, skill = 3;
	if (jobperformance > 5)	skill += 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
		xp += 3;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
		xp -= 3;
	}

	if (g_Dice % 2 == 1)
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill+1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);

	return false;
}
