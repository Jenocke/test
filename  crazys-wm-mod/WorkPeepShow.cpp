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

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkPeepShow(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
        string message = ""; string girlName = girl->m_Realname;
        if (Preprocessing(ACTION_WORKSTRIP, girl, brothel, DayNight, summary, message))
                return true;

        // put that shit away, you'll scare off the customers!
        g_Girls.UnequipCombat(girl);

        int roll = g_Dice % 100;
        int jobperformance = (g_Girls.GetStat(girl, STAT_CHARISMA) / 2 +
                g_Girls.GetStat(girl, STAT_BEAUTY) / 2 +
                g_Girls.GetSkill(girl, SKILL_STRIP) / 2 +
                g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 2);

        int wages = g_Girls.GetStat(girl, STAT_ASKPRICE) + g_Dice%50;
        int tips = max(g_Dice%50,0);
        double mod = 0;
        int imagetype = IMGTYPE_STRIP;
		int sex = false, work = 0;
        message += "She let the customers watch her get naked.\n\n";

        //good traits
        if (g_Girls.HasTrait(girl, "Charismatic"))              jobperformance += 15;
        if (g_Girls.HasTrait(girl, "Sexy Air"))                 jobperformance += 10;
        if (g_Girls.HasTrait(girl, "Cool Person"))              jobperformance += 10;  //people love to be around her
        if (g_Girls.HasTrait(girl, "Cute"))                     jobperformance += 5;
        if (g_Girls.HasTrait(girl, "Charming"))                 jobperformance += 10;  //people like charming people
        if (g_Girls.HasTrait(girl, "Great Figure"))             jobperformance += 5;
        if (g_Girls.HasTrait(girl, "Great Arse"))               jobperformance += 5;
        if (g_Girls.HasTrait(girl, "Quick Learner"))            jobperformance += 5;
        if (g_Girls.HasTrait(girl, "Psychic"))                  jobperformance += 10;  //knows what people want to hear
        if (g_Girls.HasTrait(girl, "Fearless"))                 jobperformance += 10;

        //bad traits
        if (g_Girls.HasTrait(girl, "Dependant"))                jobperformance -= 50;  //needs others to do the job
        if (g_Girls.HasTrait(girl, "Clumsy"))                   jobperformance -= 20;  //spills food and breaks things often
        if (g_Girls.HasTrait(girl, "Aggressive"))               jobperformance -= 20;  //gets mad easy and may attack people
        if (g_Girls.HasTrait(girl, "Nervous"))                  jobperformance -= 30;  //don't like to be around people
        if (g_Girls.HasTrait(girl, "Meek"))                     jobperformance -= 20;
		if (g_Girls.HasTrait(girl, "Shy"))                      jobperformance -= 20;
        if (g_Girls.HasTrait(girl, "Slow Learner"))             jobperformance -= 10;
        if (g_Girls.HasTrait(girl, "Horrific Scars"))           jobperformance -= 20;
        if (g_Girls.HasTrait(girl, "Small Scars"))              jobperformance -= 5;

        if (jobperformance >= 245)
        {
                mod = 3.0;                              message += "She must be the perfect teaser she never goes to fast but never to slow, she knows just when to do things. The customers can't get enough of her.\n\n";
                /* */if (roll <= 20) {  message += "Knowing every trick in the book and not hesitating to use them, she makes a lot of money today.\n"; }
                else if (roll <= 40) {  message += "She is just a gold mine.\n"; }
                else if (roll <= 60) {  message += "She certainly knows how to sell the show. After hours of teasing she finally gets naked.\n"; }
                else if (roll <= 80) {  message += girlName + " was requested by name by a single customer. He paid double of her daily earnings for the privilege of getting an exclusive performance.\n"; }
                else /*           */ {  message += girlName + " is performing today. Like always the cleaning crew have a lot of work after her shows.\n"; }
        }
        else if (jobperformance >= 185)
        {
                mod = 2.0;                              message += "She is unbelievable at this and is always finding new ways of pleaseing the customers.\n\n";
                /* */if (roll <= 20) {  message += "She is really good at this, knowing how to tease customers into staying and paying for hours.\n"; }
                else if (roll <= 40) {  message += girlName + " swinging her boobs and ass in a very sexual way. As a result causing heavy breathing from the booths.\n"; }
                else if (roll <= 60) {  message += "Her body movement is flawless and very sensual, earning a nice sum today.\n"; }
                else if (roll <= 80) {  message += girlName + " gave a very good performance, making her clients very satisfied with the time they spent here.\n"; }
                else /*           */ {  message += girlName + " was requested by name by a group of customers. She gave them a special performance for a really high price.\n"; }
        }
        else if (jobperformance >= 145)
        {
                mod = 1.5;                              message += "She's good at this job and knows a few tricks to drive the customers wild.\n\n";
                /* */if (roll <= 20) {  message += "She has a good body and sense of rhythm, but she strips too fast, so she make less money from today performance.\n"; }
                else if (roll <= 40) {  message += "She makes the mistake of getting naked too fast, ending the show prematurely.\n"; }
                else if (roll <= 60) {  message += "She certainly has the skills for this job, but with more work she will get even better.\n"; }
                else if (roll <= 80) {  message += girlName + " comes on stage with some 'toys' that she uses to prolong her performance.\n"; }
                else /*           */ {  message += "Having confidence in her skills, " + girlName + " gives a very good performance.\n"; }
        }
        else if (jobperformance >= 100)
        {
                mod = 1.0;                              message += "She made a few mistakes but overall she is okay at this.\n\n";
                /* */if (roll <= 20) {  message += girlName + " can strip and people come here to see her do that but there is nothing extraordinary in her performance.\n"; }
                else if (roll <= 40) {  message += "Pretty boring show, But at least she gets naked all the way thru.\n"; }
                else if (roll <= 60) {  message += "Having stage fright, her movement seems mechanical, leaving paying customers unsatisfied.\n"; }
                else if (roll <= 80) {  message += "Not too confident in herself, " + girlName + "'s tense performance rubs off on the customers.\n"; }
                else /*           */ {  message += "She did good today. With improving her skills, " + girlName + " could become one of the best.\n"; }
        }
        else if (jobperformance >= 70)
        {
                mod = 0.9;                              message += "She was nervous and made a few mistakes. She isn't that good at this.\n\n";
                /* */if (roll <= 20) {  message += "When it was time for " + girlName + " to take off her panties, she collapsed on the floor covering her breasts and crying.\n"; }
                else if (roll <= 40) {  message += "Not being able to unbuckle her fancy bra, she struggled for minutes. Some of the customers left demanding their money back.\n"; }
                else if (roll <= 60) {  message += "She gets naked but in a boring and not exciting way.\n"; }
                else if (roll <= 80) {  message += "Trying a more advanced pose, she hurts herself, falling painfully on the floor.\n"; }
				else /*           */ {	message += girlName + " definitely tries to give a good performance, but having only good will isn't enough.\n";
				}
		}
        else
        {
                mod = 0.8;                              message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
                /* */if (roll <= 20) {  message += "She is awful at this job. Her stage movement and striptease are lifeless.\n"; }
                else if (roll <= 40) {  message += "Her stripping ability could be compared to getting undressed before a shower.\n"; }
                else if (roll <= 60) {  message += "Her show was so bad, that you could swear you heard snoring from one booth.\n"; }
                else if (roll <= 80) {  message += "When trying to take off her t-shirt, " + girlName + "'s head got stuck for minutes.\n"; }
                else /*           */ {  message += "Trying to give her best, " + girlName + " gave a poor performance.\n"; }
        }


		//try and add randomness here
        if (g_Girls.GetStat(girl, STAT_LIBIDO) > 80)
        {
			if (g_Girls.HasTrait(girl, "Lesbian") && g_Girls.HasTrait(girl, "Nymphomaniac"))
			{
				message += "\nShe was horney and she loves sex so she brought in another girl and had sex while the customers watched.";
				imagetype = IMGTYPE_LESBIAN; g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 1);
			}
			else if (g_Girls.HasTrait(girl, "Bisexual") && g_Girls.HasTrait(girl, "Nymphomaniac"))
			{
				if (roll <= 50)
				{
					message += "\nShe was horney and she loves sex so she brought in another girl and had sex while the customers watched.";
					imagetype = IMGTYPE_LESBIAN; g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 1);
				}
				else
				{
					message += "\nShe was horney and she loves sex so she brought in one of the customers and had sex with him while the others watched.";
					sex = true;
				}
			}
			else if (g_Girls.HasTrait(girl, "Straight") && g_Girls.HasTrait(girl, "Nymphomaniac"))
			{
				message += "\nShe was horney and she loves sex so she brought in one of the customers and had sex with him while the others watched.";
				sex = true;
			}
			else
			{
                message += "\nShe was horney and ended up masturbating for the customers, making them very happy.";
				imagetype = IMGTYPE_MAST;
			}
				sCustomer cust;
                GetMiscCustomer(brothel, cust);
                brothel->m_Happiness += 100;
                g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -20);
                mod += 0.2;
                g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1, true);
        }
        else
        {
                brothel->m_Happiness += (g_Dice % 70) + 30;
                brothel->m_MiscCustomers++;
        }

	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{ message += "Stunned by her beauty a customer left her a great tip.\n\n"; mod += 0.1;; }

	if (g_Dice % 100 <= 5)  //glory hole event
	{
		message += "A man manged to cut a hole out from his booth and made himself a glory hole, " + girlName + " saw his cock sticking out and .";
		{
			if (g_Girls.HasTrait(girl, "Meek") || g_Girls.HasTrait(girl, "Shy"))
				{ message += "Meekly ran away from it.\n"; work -= 5; }
			else if (g_Girls.HasTrait(girl, "Lesbian"))
				{ message += "she doesn't understand the appeal of them, which turned her off.\n"; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -10); }
			else if (g_Girls.HasTrait(girl, "Nymphomaniac") &&  !g_Girls.HasTrait(girl, "Lesbian") &&  !g_Girls.HasTrait(girl, "Virgin") && g_Girls.GetStat(girl, STAT_LIBIDO) >= 80) //sex
				{
					message += "decided she needed to use it for her on entertainment.\n";
					imagetype = IMGTYPE_SEX; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -20);
				}
			else if (g_Girls.HasTrait(girl, "Nymphomaniac") &&  !g_Girls.HasTrait(girl, "Lesbian") && g_Girls.GetStat(girl, STAT_LIBIDO) >= 60) //oral
				{ message += "decided she need to taste it.\n"; imagetype = IMGTYPE_ORAL; }
			else if (g_Girls.HasTrait(girl, "Nymphomaniac") &&  !g_Girls.HasTrait(girl, "Lesbian") && g_Girls.GetStat(girl, STAT_LIBIDO) >= 40) //foot
				{ message += "decided she would give him a foot job for been so brave.\n"; imagetype = IMGTYPE_FOOT; }
			else  //hand job
				{ message += "decided she would give him a hand job for been so brave.\n"; imagetype = IMGTYPE_HAND; }
		}
	}

	if (wages < 0)
		wages = 0;

	//enjoyed the work or not
	if (roll <= 5)
	{ message += "\nSome of the patrons abused her during the shift."; work -= 1; }
	else if (roll <= 25) 
	{ message += "\nShe had a pleasant time working."; work += 3; }
	else
	{ message += "\nOtherwise, the shift passed uneventfully."; work += 1; }

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKSTRIP, work , true);


	if (sex)
	{
		u_int n = 0;
		sCustomer cust;
		while (true)
		{
		GetMiscCustomer(brothel, cust);
		if (is_sex_type_allowed(cust.m_SexPref, brothel))
			break;
		}
		g_Girls.GirlFucks(girl, DayNight, &cust, false,message,n);
		brothel->m_Happiness += 100;
		int imageType = IMGTYPE_SEX;
		if(n == SKILL_ANAL)
			imageType = IMGTYPE_ANAL;
		else if(n == SKILL_BDSM)
			imageType = IMGTYPE_BDSM;
		else if(n == SKILL_NORMALSEX)
			imageType = IMGTYPE_SEX;
		else if(n == SKILL_BEASTIALITY)
			imageType = IMGTYPE_BEAST;
		else if(n == SKILL_GROUP)
			imageType = IMGTYPE_GROUP;
		else if(n == SKILL_LESBIAN)
			imageType = IMGTYPE_LESBIAN;
		else if(n == SKILL_ORALSEX)
			imageType = IMGTYPE_ORAL;
		else if(n == SKILL_TITTYSEX)
			imageType = IMGTYPE_TITTY;
		else if(n == SKILL_HANDJOB)
			imageType = IMGTYPE_HAND;
		else if(n == SKILL_STRIP)
			imageType = IMGTYPE_STRIP;
		// work out the pay between the house and the girl
		girl->m_Tips = int(tips * mod);
        girl->m_Pay = int(wages * mod);
		girl->m_Events.AddMessage(message, imageType, DayNight);
	}
	else
		{
		 // work out the pay between the house and the girl
        girl->m_Tips = int(tips * mod);
        girl->m_Pay = int(wages * mod);
        girl->m_Events.AddMessage(message, imagetype, DayNight);
		}


    // Improve stats
	int xp = 15, libido = 1, skill = 3;

    if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

    g_Girls.UpdateStat(girl, STAT_FAME, 1);
    g_Girls.UpdateStat(girl, STAT_EXP, xp);
    g_Girls.UpdateSkill(girl, SKILL_STRIP, g_Dice%skill + 1);
    g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill + 1);
    g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);


    //lose
    g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_WORKSTRIP, girlName + " has had so many people see her naked she is no longer nervous about anything.", DayNight != 0);

    return false;
}
