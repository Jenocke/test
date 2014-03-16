/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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
#include "cBrothel.h"
#include "cArena.h"
#include "cArenaTry.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "sFacilityList.h"
#include "cGetStringScreenManager.h"
#include "InterfaceProcesses.h"
#include "cScriptManager.h"
#include <iostream>
#include <locale>
#include <sstream>
#include "cGangs.h"

extern	bool			g_InitWin;
extern	int			g_CurrBrothel;
extern	cGold			g_Gold;
extern	cBrothelManager		g_Brothels;
extern	cArenaManager		g_Arena;
extern	cWindowManager		g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;
extern bool g_TryOuts;
extern bool g_Cheats;
extern	bool	eventrunning;
extern string g_ReturnText;
extern cGangManager g_Gangs;
extern bool g_AllTogle;
extern int g_CurrentScreen;
extern int g_Building;

bool cArenaTry::ids_set = false;

void cArenaTry::set_ids()
{
	ids_set = true;
	back_id =		get_id("BackButton");
	walk_id =		get_id("WalkButton");
	curbrothel_id =	get_id("CurrentBrothel");
	arena_id =		get_id("Arena");
}

void cArenaTry::init()
{
	g_CurrentScreen = SCREEN_TRYOUTS;
/*
 *	buttons enable/disable
 */
	DisableButton(walk_id, g_TryOuts);

}

void cArenaTry::process()
{
/*
 *	we need to make sure the ID variables are set
 */
	if(!ids_set) {
		set_ids();
	}

	init();
/* 
 *	no events means we can go home
 */
	if(g_InterfaceEvents.GetNumEvents() == 0) {
		return;
	}

/*
 *	otherwise, compare event IDs 
 *
 *	if it's the back button, pop the window off the stack
 *	and we're done
 */
	if(g_InterfaceEvents.CheckButton(back_id)) {
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	else if(g_InterfaceEvents.CheckButton(walk_id)) {
		do_walk();
		if(!g_Cheats) g_TryOuts = true;
		g_InitWin = true;
	}
}

string cArenaTry::walk_no_luck()
{
	if(m_first_walk) {
		m_first_walk = false;
		return	"Your father once called this 'talent spotting' - "
			"and looking these girls over you see no talent for "
			"anything."
		;
	}
	switch(g_Dice % 8) {
	case 0:
	case 1:
	case 2: return
		"The city is quiet and no one shows up.";
	case 3: return
		"Married. Married. Bodyguard. Already works for you. Married. "
		"Hideous. Not a woman. Married. Escorted. Married... "
		"Might as well go home, there's nothing happening out here."
		;
	case 4: return
		"It's not a bad life, if you can get paid for hanging around "
		"on street corners and eyeing up the pretty girls. Not a "
		"single decent prospect in the bunch of them, mind. "
		"Every silver lining has a cloud..."
		;
	case 5: return
		"You've walked and walked and walked, and the prettiest "
		"thing you've seen all day turned out not to be female. "
		"It's time to go home..."
		;
	case 6: return
		"When the weather is bad, the hunting is good. Get them cold "
		"and wet enough and girls too proud to spread their legs "
		"suddenly can't get their knickers off fast enough, if the job "
		"only comes with room and board. The down side is that you "
		"spend far too much time walking in the rain when everyone "
		"sane is warm inside. Time to head home for a mug of cocoa "
		"and a nice hot trollop."
		;
	case 7: return
		"There's a bit of skirt over there with a lovely "
		"figure, and had a face that was pretty, ninety "
		"years ago. Over yonder, a sweet young thing frolicking "
		"through the marketplace. She's being ever so daring, "
		"spending her daddy's gold, and hasn't yet realised "
		"that there's a dozen of her daddy's goons keeping "
		"a discreet eye on her.  It's like that everywhere "
		"today. Maybe tomorrow will be better."
		;
	}
/*
 *	I don't think this should happen, hence the overly dramatic prose
 */
	return	"The sky is filled with thunder, and portals are opening all "
		"over Crossgate. You've seen five rains of frogs so far and "
		"three madmen speaking in tongues. In the marketplace a "
		"mechanical contraption materialised which walked like a man "
		"and declaimed alien poetry for half an hour before bursting "
		"into flames and collapsing. And the only thing all day that "
		"looked female and and attractive turned out to be some "
		"otherwordly species of carnivorous plant, and had to be "
		"destroyed by the town guard. The only good thing about this "
		"day is that it's over. It's time to go home."
	;
}

void cArenaTry::do_walk()
{
	if(g_TryOuts) {
		g_MessageQue.AddToQue("You can only do this once per week.", 2);
		return;
	}
/*
 *	let's get a girl for the player to meet
 */
	sGirl *girl = g_Girls.GetRandomGirl();
/*
 *	if there's no girl, no meeting
 */
	if(girl == 0) {
		g_MessageQue.AddToQue(walk_no_luck(), 0);
		return;
	}
/*
 *	most of the time, you're not going to find anyone
 *	unless you're cheating, of course.
 */
	cConfig cfg;
	int meet_chance = cfg.initial.girl_meet();
	if(!g_Dice.percent(meet_chance) && !g_Cheats) {
		g_MessageQue.AddToQue(walk_no_luck(), 1);
		return;
	}
/*
 *	I'd like to move this to the handler script
 *
 *	once scripts are stable
 */
	string message = "You hold open try outs to all girls willing to step into, ";
	message += "the arena and fight for there life.";
	g_MessageQue.AddToQue(message, 2);
	int v[2] = {3, -1};
	cTrigger* trig = 0;
	g_Building = BUILDING_ARENA;
	DirPath dp;
	string filename;
	cScriptManager sm;
/*
 *	is there a girl specific talk script?
 */
	if(!(trig = girl->m_Triggers.CheckForScript(TRIGGER_MEET, false, v))) {
		// no, so trigger the default one
		dp = DirPath()
			<< "Resources" << "Scripts" << "MeetArenaTry.script"
		;
	}
	else {
		// trigger the girl-specific one
		dp = DirPath()
			<< "Resources" << "Characters" << girl->m_Name
			<< trig->m_Script
		;
	}
	eventrunning = true;
	sm.Load(dp, girl);
	return;
}


