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
#ifndef __CINVENTORY_H
#define __CINVENTORY_H

// includes
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "Constants.h"

using namespace std;

struct sGirl;

struct sEffect
{
	// MOD docclox
/*
 *	let's have an enum for possible values of m_Affects
 */
	enum What {
		Skill		= 0,
		Stat		= 1,
		Nothing		= 2,
		GirlStatus	= 3,
		Trait		= 4
	};
	What m_Affects;
/*
 *	define an ostream operator so it will pretty print
 *	(more useful for debugging than game play
 *	but why take it out?)
 */
	friend ostream& operator << (ostream& os, sEffect::What &w);
/*
 *	and a function to go the other way
 *	we need this to turn the strings in the xml file
 *	into numbers
 */
	void set_what(string s) {
		if		(s == "Skill")		m_Affects = Skill;
		else if	(s == "Stat")		m_Affects = Stat;
		else if	(s == "Nothing")	m_Affects = Nothing;
		else if	(s == "GirlStatus")	m_Affects = GirlStatus;
		else if	(s == "Trait")		m_Affects = Trait;
		else {
			m_Affects = Nothing;
			cerr	<< "Error: Bad 'what' string for item effect: '"
				<< s
				<< "'"
				<< endl;
		}
	}
/*
 *	can't make an enum for this since it can represent
 *	different quantites.
 *
 *	The OO approach would be to write some variant classes, I expect
 *	but really? Life's too short...
 */
	unsigned char m_EffectID;	// what stat, skill or status effect it affects
/*
 *	but I still need strings for the skills, states, traits and so forth
 *
 *	these should be (were until the merge) in sGirl. Will be again
 *	as soon as I sort the main mess out...
 */
	const char *girl_status_name(unsigned int id);
	const char *skill_name(unsigned int id);		// WD:	Use definition in sGirl::
	const char *stat_name(unsigned int id);			// WD:	Use definition in sGirl::

#if 0 // WD: use definition and code in sGirl:: to remove dup code
	const char *skill_name(unsigned int id) {
		const char *skills[] = {
			"Anal",
			"Magic",
			"BDSM"),
			"NormalSex"),
			"Beastiality"),
			"Group",
			"Lesbian",
			"Service",
			"Strip"),
			"Combat")
		};
		const size_t n_skills = (sizeof(skills) / sizeof(skills[0]));
		if(id < n_skills) {
			return skills[id];
		}
		cerr << "Error: skill id " << id << " too large (max is " << n_skills << ")" << endl;
		return "";
	}
	const char *stat_name(unsigned int id) {
		const char *stats[] = {
			"Charisma"),	"Happiness"),	"Libido",	"Constitution"),
			"Intelligence"),	"Confidence"),	"Mana"),		"Agility"),
			"Fame"),		"Level"),	"AskPrice"),	"House"),
			"Exp"),		"Age"),		"Obedience"),	"Spirit"),
			"Beauty"),	"Tiredness"),	"Health"),	"PCFear"),
			"PCLove"),	"PCHate")
		};
		const size_t n_stats = (sizeof(stats) / sizeof(stats[0]));
		if(id < n_stats) {
			return stats[id];
		}
		cerr << "Error: stat id " << id << " too large (max is " << n_stats << ")" << endl;
		return 0;
	}
#endif

/*
 *	and we need to go the other way,
 *	setting m_EffectID from the strings in the XML file
 *
 *	WD:	Change to use definition and code in sGirl::
 *		remove duplicated code
 */
	bool set_skill(string s);
	bool set_girl_status(string s);
	bool set_stat(string s);

#if 0 // WD: use definition and code in sGirl:: to remove dup code
	bool set_skill(string s)
	{

		if(s == "Anal")			m_EffectID = 0;
		else if(s == "Magic")		m_EffectID = 1;
		else if(s == "BDSM")		m_EffectID = 2;
		else if(s == "NormalSex")	m_EffectID = 3;
		else if(s == "Beastiality")	m_EffectID = 4;
		else if(s == "Group")		m_EffectID = 5;
		else if(s == "Lesbian")		m_EffectID = 6;
		else if(s == "Service")		m_EffectID = 7;
		else if(s == "Strip")		m_EffectID = 8;
		else if(s == "Combat")		m_EffectID = 9;
		else {
			cerr << "unknown skill: " << s << endl;
			return false;
		}
		return true;
	}
	bool set_girl_status(string s)
	{
		if(s == "Poisoned")		m_EffectID = 1;
		else if(s == "Badly Poisoned")	m_EffectID = 2;
		else if(s == "Pregnant")	m_EffectID = 3;
		else if(s == "Pregnant By Player") 	m_EffectID = 4;
		else if(s == "Slave")		m_EffectID = 5;
		else if(s == "Has Daughter")	m_EffectID = 6;
		else if(s == "Has Son")		m_EffectID = 7;
		else if(s == "Inseminated")	m_EffectID = 8;
		else if(s == "Controlled")	m_EffectID = 9;
		else if(s == "Catacombs")	m_EffectID = 10;
		else {
			cerr << "unknown girl status: " << s << endl;
			return false;
		}
		return true;
	}
	bool set_stat(string s)
	{
		//cerr << "looking up " << s << endl;
		if(s == "Age")			m_EffectID = STAT_AGE;
		else if(s == "Agility") 	m_EffectID = STAT_AGILITY;
		else if(s == "AskPrice")	m_EffectID = STAT_ASKPRICE;
		else if(s == "Beauty")		m_EffectID = STAT_BEAUTY;
		else if(s == "Charisma")	m_EffectID = STAT_CHARISMA;
		else if(s == "Confidence") 	m_EffectID = STAT_CONFIDENCE;
		else if(s == "Constitution") 	m_EffectID = STAT_CONSTITUTION;
		else if(s == "Exp") 		m_EffectID = STAT_EXP;
		else if(s == "Fame") 		m_EffectID = STAT_FAME;
		else if(s == "Happiness") 	m_EffectID = STAT_HAPPINESS;
		else if(s == "Health") 		m_EffectID = STAT_HEALTH;
		else if(s == "House") 		m_EffectID = STAT_HOUSE;
		else if(s == "Intelligence") 	m_EffectID = STAT_INTELLIGENCE;
		else if(s == "Level") 		m_EffectID = STAT_LEVEL;
		else if(s == "Libido") 		m_EffectID = STAT_LIBIDO;
		else if(s == "Mana") 		m_EffectID = STAT_MANA;
		else if(s == "Obedience") 	m_EffectID = STAT_OBEDIENCE;
		else if(s == "PCFear") 		m_EffectID = STAT_PCFEAR;
		else if(s == "PCHate") 		m_EffectID = STAT_PCHATE;
		else if(s == "PCLove") 		m_EffectID = STAT_PCLOVE;
		else if(s == "Spirit") 		m_EffectID = STAT_SPIRIT;
		else if(s == "Tiredness") 	m_EffectID = STAT_TIREDNESS;
		else {
			cerr	<< "Error: unknown stat string '"
				<< s
				<< "'"
				<< endl
			;
			m_EffectID = STAT_CHARISMA;
			return false;
		}
		return true;
	}
#endif
	
/*
 *	magnitude of the effect. 
 *	-10 will subtract 10 from the target stat while equiped
 *	and add 10 when unequiped.
 *
 *	With status effects and traits 1 means add,
 *	0 means take away and 2 means disable
 */
	int m_Amount;
/*
 *	name of the trait it adds
 */
	string m_Trait;
/*
 *	and a pretty printer for the class as a whole
 *	just a debug thing, really
 */
	friend ostream& operator << (ostream& os, sEffect &eff) {
		os << "Effect: " << eff.m_Affects << " ";
		if(eff.m_Affects == Stat) {
			os << eff.stat_name(eff.m_EffectID) ;
		}
		if(eff.m_Affects == Skill) {
			os << eff.skill_name(eff.m_EffectID) ;
		}
		if(eff.m_Affects == Trait) {
			os << "'" << eff.m_Trait << "'";
		}
		if(eff.m_Affects == GirlStatus) {
			os << eff.girl_status_name( eff.m_EffectID );
		}
		os << (eff.m_Amount > 0 ? " +" : " ") << eff.m_Amount;
		return os << endl;
	}
	// end mod
};

typedef struct sInventoryItem
{
	string m_Name;
	string m_Desc;
/*
 *	item type: let's make an enum
 */
	enum Type {
		Ring	= 1,	// worn on fingers (max 8)
		Dress	= 2,	// Worn on body, (max 1)
		Shoes	= 3,	// worn on feet, (max 1)
		Food	= 4,	// Eaten, single use
		Necklace= 5,	// worn on neck, (max 1)
		Weapon	= 6,	// equiped on body, (max 2)
		Makeup	= 7,	// worn on face, single use
		Armor	= 8,	// worn on body over dresses (max 1)
		Misc	= 9,	// random stuff. may cause a constant effect without having to be equiped
		Armband	= 10,	// (max 2), worn around arms
		SmWeapon= 11,	// (max 1), hidden on body
		UnderWear= 12  // (max 1) worn under dress
	};
	Type m_Type;
/*
 *	and another for special values
 */
	enum Special {
		None		= 0,
		AffectsAll	= 1,
		Temporary	= 2
	};
	Special m_Special;
/*
 *	if 1 then this item doesn't run out if stocked in shop inventory
 */
	bool m_Infinite;
/*
 *	the number of effects this item has
 */
	vector<sEffect> m_Effects;
/*
 *	how much the item is worth?
 */
	long m_Cost;
/*	0 is good, while badness > is evil.
 *	Girls may fight back if badness > 0,
 *	Girls won't normally buy items > 20 on their own
 *      default formula is -5% chance to buy on their own per Badness point (5 Badness = 75% chance)
 *	Girls with low obedience may take off something that is bad for them
 */
	unsigned char m_Badness;
	unsigned char m_GirlBuyChance;  // chance that a girl on break will buy this item if she's looking at it in the shop
/*
 *	0 means common,
 *	1 means 50% chance of appearing in shops,
 *	2 means 25% chance,
 *	3 means 5% chance and
 *	4 means only 15% chance of being found in catacombs,
 *	5 means ONLY given in scripts and
 *	6 means same as 5 but also may be given as a reward for objective
    7 means only 5% chance in catacombs (catacombs05)
	8 means only 1% chance in catacombs (catacombs01)
 */
 	enum Rarity {
		Common		= 0,
		Shop50		= 1,
		Shop25		= 2,
		Shop05		= 3,
		Catacomb15	= 4,
		ScriptOnly	= 5,
		ScriptOrReward	= 6,
		Catacomb05 = 7,   // MYR: Added 05 and 01 for the really, really valuable things like invulnerability
		Catacomb01 = 8
	};
	Rarity m_Rarity;

	void set_rarity(string s)
	{
		if(s == "Common") {
			m_Rarity = Common;
		}
		else if(s == "Shop50") {
			m_Rarity = Shop50;
		}
		else if(s == "Shop25") {
			m_Rarity = Shop25;
		}
		else if(s == "Shop05") {
			m_Rarity = Shop05;
		}
		else if(s == "Catacomb15") {
			m_Rarity = Catacomb15;
		}
		else if(s == "Catacomb05") {
			m_Rarity = Catacomb05;
		}
		else if(s == "Catacomb01") {
			m_Rarity = Catacomb01;
		}
		else if(s == "ScriptOnly") {
			m_Rarity = ScriptOnly;
		}
		else if(s == "ScriptOrReward") {
			m_Rarity = ScriptOrReward;
		}
		else {
			cerr	<< "Error in set_rarity: unexpected value '"
				<< s
				<< "'"
				<< endl;
			m_Rarity = Shop05;	// what to do?
		}
	}

	void set_special(string s)
	{
		if(s == "None") {
			m_Special = None;
		}
		else if(s == "AffectsAll") {
			m_Special = AffectsAll;
		}
		else if(s == "Temporary") {
			m_Special = Temporary;
		}
		else {
			cerr	<< "unexpected special string: '"
				<< s
				<< "'"
				<< endl
			;
			m_Special = None;
		}
	}

	void set_type(string s)
	{
		if(s == "Ring") {
			m_Type = Ring;
		}
		else if(s == "Dress") {
			m_Type = Dress;
		}
		else if(s == "Under Wear") {
			m_Type = UnderWear;
		}
		else if(s == "Shoes") {
			m_Type = Shoes;
		}
		else if(s == "Food") {
			m_Type = Food;
		}
		else if(s == "Necklace") {
			m_Type = Necklace;
		}
		else if(s == "Weapon") {
			m_Type = Weapon;
		}
		else if(s == "Small Weapon") {
			m_Type = SmWeapon;
		}
		else if(s == "Makeup") {
			m_Type = Makeup;
		}
		else if(s == "Armor") {
			m_Type = Armor;
		}
		else if(s == "Misc") {
			m_Type = Misc;
		}
		else if(s == "Armband") {
			m_Type = Armband;
		}
		else {
			cerr << "Error: unexpected item type: " << s << endl;
			m_Type = Misc;
		}
	}

	friend ostream& operator << (ostream& os, sInventoryItem::Special &spec) {
		switch(spec) {
		case None:
			return os << "None";
		case AffectsAll:
			return os << "AffectsAll";
		case Temporary:
			return os << "Temporary";
		default:
			cerr << "error: unexpected special value: " << int(spec) << endl;
			return os << "Error(" << int(spec) << ")";
		}
	}

	friend ostream& operator << (ostream& os, sInventoryItem::Rarity &r) {
		switch(r) {
		case Common:
			return os << "Common";
		case Shop50:
			return os << "Shops, 50%";
		case Shop25:
			return os << "Shops, 25%";
		case Shop05:
			return os << "Shops, 05%";
		case Catacomb15:
			return os << "Catacombs, 15%";
		case Catacomb05:
			return os << "Catacombs, 05%";
		case Catacomb01:
			return os << "Catacombs, 01%";
		case ScriptOnly:
			return os << "Scripted Only";
		case ScriptOrReward:
			return os << "Scripts or Reward";
		default:
			cerr << "error: unexpected rarity value: " << int(r) << endl;
			return os << "Error(" << int(r) << ")";
		}
	}

	friend ostream& operator << (ostream& os, sInventoryItem::Type &typ) {
		switch(typ) {
		case Ring:
			return os << "Ring";
		case Dress:
			return os << "Dress";
		case UnderWear:
			return os << "Under Wear";
		case Shoes:
			return os << "Shoes";
		case Food:
			return os << "Food";
		case Necklace:
			return os << "Necklace";
		case Weapon:
			return os << "Weapon";
		case SmWeapon:
			return os << "Small Weapon";
		case Makeup:
			return os << "Makeup";
		case Armor:
			return os << "Armor";
		case Misc:
			return os << "Misc";
		case Armband:
			return os << "Armband";
		default:
			cerr << "Unexpected type value: " << int(typ) << endl;
			return os << "Error";
		}
		cerr << "How the hell did I get here?" << endl;
		return os;
	}

	friend ostream& operator << (ostream& os, sInventoryItem &it) {
		os << "Item: " << it.m_Name << endl;
		os << "Desc: " << it.m_Desc << endl;
		os << "Type: " << it.m_Type << endl;
		os << "Badness: " << int(it.m_Badness) << endl;
		os << "Special: " << it.m_Special << endl;
		os << "Cost: " << it.m_Cost << endl;
		os << "Rarity: " << it.m_Rarity << endl;
		os << "Infinite: " << (it.m_Infinite ? "True" : "False") << endl;
		for(unsigned int i = 0; i < it.m_Effects.size(); i++) {
			sEffect &eff = it.m_Effects[i];

			os << eff;
		}
		return os;
	}
} sInventoryItem;

int itemcompless(const sInventoryItem * ,const sInventoryItem *);
int part(int first,int last,sInventoryItem * curr[]);
void qswap(int,int,sInventoryItem * curr[]);
void qu_sort(int,int,sInventoryItem * curr[]);

class cInventory
{
public:
	cInventory() {
		for(int i = 0; i < NUM_SHOPITEMS; i++) {
			m_ShopItems[i] = 0;
		}
		m_NumShopItems = 0;
	}
	~cInventory();

	void Free();

	void LoadItems(string filename);
	bool LoadItemsXML(string filename);
	void UpdateShop();	// re-randomizes the shops inventory
	sInventoryItem* GetItem(string name);
	sInventoryItem* GetShopItem(int num);
	int GetRandomShopItem();
	sInventoryItem* GetRandomItem();
	int CheckShopItem(string name);	// checks if a item is in shop inventory, returns -1 if not and the id if it is
	sInventoryItem* BuyShopItem(int num);	// removes and returns the item from the shop
	bool GirlBuyItem(sGirl* girl, int ShopItem, int MaxItems, bool AutoEquip);  // girl buys selected item if possible; returns true if bought

	void Equip(sGirl* girl, int num, bool force);
	void Equip(sGirl* girl, sInventoryItem* item, bool force);
	void Unequip(sGirl* girl, int num);

	void AddItem(sInventoryItem* item);
	void CalculateCost(sInventoryItem* newItem);

	int HappinessFromItem(sInventoryItem* item);  // determines how happy a girl will be to receive an item (or how unhappy to lose it)

	void GivePlayerAllItems();

	bool IsItemEquipable(sInventoryItem* item)
	{
		if(item->m_Type != INVMISC)
			return true;
		return false;
	}
	void sort();

	bool	equip_limited_item_ok(sGirl*, int, bool, int);
	bool	equip_pair_ok(sGirl*, int, bool);
	bool	equip_ring_ok(sGirl*, int, bool);
	bool	equip_singleton_ok(sGirl*, int, bool);
	bool	ok_2_equip(sGirl*, int, bool);
	void	remove_trait(sGirl*, int, int);


private:
	vector<sInventoryItem *> items;  // Master list of items?
	int m_NumShopItems;	// number of items in the shop
	sInventoryItem* m_ShopItems[NUM_SHOPITEMS];	// pointers to all items, the shop can only hold 30 random items
};

#endif