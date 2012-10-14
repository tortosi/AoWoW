/*
* Copyright (C) 2008 AoWoW <http://aowow.sourceforge.net/>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Current version Of WoW: 2.4.2
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include "dbcfile.h"

#define uint32 unsigned long

std::fstream ofile("aowow.sql", std::ios::out);

// Звания игроков
struct char_titles
{
	uint32				ID;			// 0, title ids, for example in Quest::GetCharTitleId()
	std::string		name;		// 2-17
//	std::string		name2;	// 19-34
};

// Доп. Стоимость вещей
struct item_extended_cost
{
	uint32 ID;											// 0 extended-cost entry id
	uint32 reqhonorpoints;					// 1 required honor points
	uint32 reqarenapoints;					// 2 required arena points
	uint32 reqitem[5];							// 3-7 required item id
	uint32 reqitemcount[5];					// 8-12 required count of 1st item
	uint32 reqpersonalarenarating;	// 13 required personal arena rating
};

// Скиллы
struct skill_line_ability
{
	//uint32	id;										// [0], unused
	uint32		skillID;							// [1]
	uint32		spellID;							// [2] INDEX
	uint32		racemask;							// [3]
	uint32		classmask;						// [4]
																	// [5-6], unknown, always 0
	uint32		req_skill_value;			// [7] for trade skill.not for training.
	uint32		forward_spellid;			// [8]
	uint32		learnOnGetSkill;			// [9] can be 1 or 2 for spells learned on get skill
	uint32		max_value;						// [10]
	uint32		min_value;						// [11]
																	// [12-13], unknown, always 0
	uint32		reqtrainpoints;				// [14]
};

// Типы сопротивлений (школы спеллов)
struct resistances
{
	std::string name;	// [1] Имя
};

struct spellcasttimes
{
	int base;
};

struct lock
{
	unsigned int type1;
	unsigned int type2;
	unsigned int type3;
	unsigned int type4;
	unsigned int type5;
//	unsigned int type6;
	unsigned int lockproperties1;
	unsigned int lockproperties2;
	unsigned int lockproperties3;
	unsigned int lockproperties4;
	unsigned int lockproperties5;
//	unsigned int lockproperties6;
	unsigned int requiredskill1;
	unsigned int requiredskill2;
	unsigned int requiredskill3;
	unsigned int requiredskill4;
	unsigned int requiredskill5;
//	unsigned int requiredskill6;
};

struct itemset
{
	std::string name;           // Название набора
	unsigned int item1;       	// Номер вещи
	unsigned int item2;
	unsigned int item3;
	unsigned int item4;
	unsigned int item5;
	unsigned int item6;
	unsigned int item7;
	unsigned int item8;
	unsigned int item9;
	unsigned int item10;
	unsigned int spell1;        // Номер спелла-бонуса
	unsigned int spell2;
	unsigned int spell3;
	unsigned int spell4;
	unsigned int spell5;
	unsigned int spell6;
	unsigned int spell7;
	unsigned int spell8;
	unsigned int bonus1;        // Кол-во вещей для бонуса
	unsigned int bonus2;
	unsigned int bonus3;
	unsigned int bonus4;
	unsigned int bonus5;
	unsigned int bonus6;
	unsigned int bonus7;
	unsigned int bonus8;
	unsigned int skillID;       // Необходимый для вещи скилл
	unsigned int skilllevel;    // Необходимый уровень скилла
};

// Имена типов диспелов
struct spelldispeltype
{
	std::string name;	// [1] Имя
};

// Имена механик спеллов
struct spellmechanic
{
	std::string name; // [1] Имя
};

// Имена иконок вещей
struct icons
{
	std::string iconname;
};

struct spellicons
{
	std::string iconname;
};

// Свойства камней
struct gemproperties
{
	unsigned int itemenchantmetID;			// Ссылка на энчант
};

// Шаблоны фракций
struct factiontemplate
{
	unsigned int factionID;						// [1]			- Ссылка на фракцию в Faction.dbc
																			// [2]			- Unknown
	unsigned int ourMask;							// [3]			- Маска на то, что фракция входит в данную фракцию
	unsigned int friendlyMask;					// [4]			- Маска на то, что фракция дружественна к этой
	unsigned int hostileMask;					// [5]			- Маска на то, что фракция враждебна к этой
	unsigned int enemyFaction1;				// [6]
	unsigned int enemyFaction2;				// [7]
	unsigned int enemyFaction3;				// [8]
	unsigned int enemyFaction4;				// [9]
	unsigned int friendFaction1;				// [10]
	unsigned int friendFaction2;				// [11]
	unsigned int friendFaction3;				// [12]
	unsigned int friendFaction4;				// [13]
	// helpers
	int A;															// [x]			- Отношения с альянсом, 1 - Friendly, -1 - Hostile, 0 - Neutral
	int H;															// [x]			- Отношения с альянсом, 1 - Friendly, -1 - Hostile, 0 - Neutral
	bool IsFriendlyTo(factiontemplate const& entry) const
	{
		if(enemyFaction1  == entry.factionID || enemyFaction2  == entry.factionID || enemyFaction3 == entry.factionID || enemyFaction4 == entry.factionID )
				return false;
		if(friendFaction1 == entry.factionID || friendFaction2 == entry.factionID || friendFaction3 == entry.factionID || friendFaction4 == entry.factionID )
				return true;
		return (friendlyMask & entry.ourMask) != 0;
	}
	bool IsHostileTo(factiontemplate const& entry) const
	{
		if(enemyFaction1  == entry.factionID || enemyFaction2  == entry.factionID || enemyFaction3 == entry.factionID || enemyFaction4 == entry.factionID )
				return true;
		if(friendFaction1 == entry.factionID || friendFaction2 == entry.factionID || friendFaction3 == entry.factionID || friendFaction4 == entry.factionID )
				return false;
		return (hostileMask & entry.ourMask) != 0;
	}
//	bool IsHostileToPlayers() const { return hostileMask & FACTION_MASK_PLAYER; }
	bool IsNeutralToAll() const { return hostileMask == 0 && friendlyMask == 0 && enemyFaction1==0 && enemyFaction2==0 && enemyFaction3==0 && enemyFaction4==0; }
};

// Фракции
struct faction
{
	int reputationListID;								// [1]			- -1 для фракций, с которыми нельзя изменить отношение, т.е. бессмысленных для таблицы фракций
//unsigned int BaseRepMask[4];				// [2-5]		- Unknown
																			// [6-7]		- Unknown
//int BaseRepValue[4];								// [8-11]		- Unknown
																			// [14-17]	- Unknown
	unsigned int team;									// [18]			- Группа (Aliance, Horde, ...)
	std::string name;										// [19]			- Имя фракции
	std::string description;						// [36]			- Предположительно, описание фракции

	// Helper
	unsigned int side;									// 1 - Альянс, 2 - Орда
};

// Улучшения вещей
struct itemenchantmet
{
	std::string text;   // Текст энчанта
};

// Моя таблица карт
struct aowow_zones
{
	unsigned int mapID;				// Номер карты мира или инсты
	unsigned int areatableID;	// Если карта мира, то номер подкарты, если инста, то значение из Map.DBC
	float x_min, y_min;					// Координаты левого нижнего угла карты, для инст = 0
	float x_max, y_max;					// Координаты правого верхнего угла карты, для инст = 0
	std::string name;						// Название карты
};

// Инсты + карты мира
struct map
{
	unsigned int type;					// Тип карты (0=norm; 1=dungeon; 2=raid; 3=pvp_zone; 4=arena;)
	std::string name;						// Имя??
	unsigned int areatableID;		// Ссылка на AreaTable.DBC
};

// Абсолютно все карты, подкарты, и т.п.
struct areatable
{
	unsigned int mapID;					// Ссылка на Map.DBC
	unsigned int areatableID;		// Ссылка, рекурсивная
	std::string name;						// Название карты?
};

// Подкарты мира
struct worldmaparea
{
	unsigned int areatableID;
	unsigned int mapID;
	float x0, y0, x1, y1;
};

struct skill
{
//	uint32			id;							// [0]
	uint32			categoryID;			// [1]
															// [2]
	std::string	name;						// [3-18] - Название скилла
															// [19]
	std::string	description;		// [20-35] - Описание скилла
															// [36]
	uint32			spellIcon;			// [37]
};

struct spellrange
{
	float rangeMin;												// [1]		- минимальное расстояние
	float rangeMax;												// [2]		- максимальное расстояние
	std::string name;											// [4]		- название расстояния (по идее [13], но там почему то пусто)
};

struct spellduration
{
	long int durationBase;
};

struct spellradius
{
	float radiusBase;
};

struct spell
{
	unsigned int resistancesID;					// [1]		- Школа спелла (Огонь, вода и т.п.)
	unsigned int dispeltypeID;						// [4]		- Тип диспела (ссылка на SpellDispelType.dbc)
	unsigned int mechanicID;							// [5]		- Механика спелла (ссылка на SpellMechanic.dbc)
	unsigned int cooldown;								// [22]		- Cooldown спелла
	uint32 ChannelInterruptFlags;					// [26]		- Channeled Spell?
	unsigned int levelspell;							// [32]		- Уровень спелла
	unsigned int tool1;									// [43]		- Необходимый инструмент #1
	unsigned int tool2;									// [44]		- Необходимый инструмент #1
	unsigned int reagent1;								// [45]		- Необходимый реагент #1
	unsigned int reagent2;								// [46]		- Необходимый реагент #2
	unsigned int reagent3;								// [47]		- Необходимый реагент #3
	unsigned int reagent4;								// [48]		- Необходимый реагент #4
	unsigned int reagent5;								// [49]		- Необходимый реагент #5
	unsigned int reagent6;								// [50]		- Необходимый реагент #6
	unsigned int reagent7;								// [51]		- Необходимый реагент #7
	unsigned int reagent8;								// [52]		- Необходимый реагент #8
	unsigned int reagentcount1;					// [53]		- Необходимое количество реагента #1
	unsigned int reagentcount2;					// [54]		- Необходимое количество реагента #1
	unsigned int reagentcount3;					// [55]		- Необходимое количество реагента #1
	unsigned int reagentcount4;					// [56]		- Необходимое количество реагента #1
	unsigned int reagentcount5;					// [57]		- Необходимое количество реагента #1
	unsigned int reagentcount6;					// [58]		- Необходимое количество реагента #1
	unsigned int reagentcount7;					// [59]		- Необходимое количество реагента #1
	unsigned int reagentcount8;					// [60]		- Необходимое количество реагента #1
	unsigned int effect1id;							// [64]		- Тип первого эффекта
	unsigned int effect2id;							// [65]		- Тип второго эффекта
	unsigned int effect3id;							// [66]		- Тип третьего эффекта
	unsigned int effect1radius;					// [91]		- Радиус действия первого эффекта
	unsigned int effect2radius;					// [92]		- Радиус действия первого эффекта
	unsigned int effect3radius;					// [93]		- Радиус действия первого эффекта
	unsigned int effect1Aura;						// [94]		- Тип ауры для первого эффекта
	unsigned int effect2Aura;						// [95]		- Тип ауры для второго эффекта
	unsigned int effect3Aura;						// [96]		- Тип ауры для третьего эффекта
	unsigned int effect1triggerspell;		// [112]	- Вызываемый первым эффектом спелл
	unsigned int effect2triggerspell;		// [113]	- Вызываемый вторым эффектом спелл
	unsigned int effect3triggerspell;		// [114]	- Вызываемый третьим эффектом спелл
	std::string rank;											// [140]	- Ранк спелла
	std::string buff;											// [174]	- Неформотированная подсказка для баффа
	unsigned int manacostpercent;				// [191]	- Затраты маны в %


	unsigned int spellcasttimesID;
	unsigned int durationID;
	unsigned int rangeID;
	long int effect1BasePoints;						// Base Damage 1
	long int effect2BasePoints;						// Base Damage 2
	long int effect3BasePoints;						// Base Damage 3
	long int effect1DieSides;							// Random Damage 1
	long int effect2DieSides;							// Random Damage 2
	long int effect3DieSides;							// Random Damage 3
	unsigned long int effect1Amplitude;		// $o1
	unsigned long int effect2Amplitude;		// $o2
	unsigned long int effect3Amplitude;		// $o3
	unsigned int effect1ChainTarget;			// $x1
	unsigned int effect2ChainTarget;			// $x2
	unsigned int effect3ChainTarget;			// $x3
	long int effect1MiscValue;						// $x1
	long int effect2MiscValue;						// $x2
	long int effect3MiscValue;						// $x3
	unsigned int procChance;
	float dmg_multiplier1;
	float dmg_multiplier2;
	float dmg_multiplier3;
	unsigned int procCharges;
	unsigned int spellTargets;
	float effect_1_proc_value;						// $e1
	float effect_2_proc_value;						// $e2
	float effect_3_proc_value;						// $e3
	unsigned int affected_target_level;
	float effect_1_proc_chance;
	float effect_2_proc_chance;
	float effect_3_proc_chance;
	unsigned int effect1itemtype;					// Получившаяся в результате вещь
	unsigned int effect2itemtype;
	unsigned int effect3itemtype;
	unsigned int spellicon;								// Иконка спелла в книге заклинаний
	unsigned int manacost;									// Затраты маны
	std::string name;
	std::string tooltip;
};

typedef std::map<int, spellcasttimes> spellcasttimes_map;
typedef std::map<int, gemproperties> gemproperties_map;
typedef std::map<int, faction> faction_map;
typedef std::map<int, spell> spellsmap;
typedef std::map<int, spellduration> spelldurationmap;
typedef std::map<int, spellrange> spellrangemap;
typedef std::map<int, spellradius> spellradiusmap;
typedef std::map<int, skill> skill_map;
typedef std::map<int, areatable> areatable_map;
typedef std::map<int, worldmaparea> worldmaparea_map;
typedef std::map<int, map> map_map;
typedef std::map<int, aowow_zones> aowow_zones_map;
typedef std::map<int, itemenchantmet> itemenchantmet_map;
typedef std::map<int, icons> icons_map;
typedef std::map<int, spellicons> spellicons_map;
typedef std::map<int, itemset> itemset_map;
typedef std::map<int, factiontemplate> factiontemplate_map;
typedef std::map<int, lock> lock_map;
typedef std::map<int, resistances> resistances_map;
typedef std::map<int, spelldispeltype> spelldispeltype_map;
typedef std::map<int, spellmechanic> spellmechanic_map;
typedef std::map<int, skill_line_ability> skill_line_ability_map;
typedef std::map<int, item_extended_cost> item_extended_cost_map;
typedef std::map<int, char_titles> char_titles_map;

std::string lowercase(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), (int(*)(int))std::tolower);
	return str;
}

std::string strim(std::string str)
{
	for (std::string::iterator it = str.begin() ; it<str.end(); ++it )
	{
		if (*it == ' ') str.erase(it);
	}
	return str;
}

std::string mysql_replace(const std::string & sBuffer)
{
	std::string sResult = sBuffer;
	std::string::size_type uPos = 0;
	while ((uPos = sResult.find("'", uPos)) != sResult.npos)
	{
		sResult = sResult.replace(uPos, 1, "''");
		uPos += 2;
	}
	return sResult;
}

std::string replace(const std::string & sBuffer, const std::string & sSrc, const std::string & sDst)
{
	std::string sResult = sBuffer;
	std::string::size_type uPos = 0;
	while ((uPos = sResult.find(sSrc, uPos)) != sResult.npos)
	{
		sResult = sResult.replace(uPos, sDst.length(), sDst);
		uPos += sDst.length();
	}
	return sResult;
}

void dump (char_titles_map char_titles_s)
{
	char_titles_map::iterator iter = char_titles_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_char_titles`;" << std::endl <<
"CREATE TABLE `aowow_char_titles` (" << std::endl <<
"  `id` integer unsigned NOT NULL," << std::endl <<
"  `name` varchar(40) NOT NULL," << std::endl <<
"  PRIMARY KEY  (`id`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_char_titles` VALUES" << std::endl;
  while (iter != char_titles_s.end())
	{
		ofile << "(" << (*iter).first << ", '"
		<< mysql_replace((*iter).second.name) << "')";
		if (++iter == char_titles_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump(item_extended_cost_map item_extended_cost_s)
{
	item_extended_cost_map::iterator iter = item_extended_cost_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_item_extended_cost`;" << std::endl <<
"CREATE TABLE `aowow_item_extended_cost` (" << std::endl <<
"  `extendedcostID` integer unsigned NOT NULL," << std::endl <<
"  `reqhonorpoints` integer unsigned NOT NULL," << std::endl <<
"  `reqarenapoints` integer unsigned NOT NULL," << std::endl <<
"  `reqitem1` integer unsigned NOT NULL," << std::endl <<
"  `reqitem2` integer unsigned NOT NULL," << std::endl <<
"  `reqitem3` integer unsigned NOT NULL," << std::endl <<
"  `reqitem4` integer unsigned NOT NULL," << std::endl <<
"  `reqitem5` integer unsigned NOT NULL," << std::endl <<
"  `reqitemcount1` integer unsigned NOT NULL," << std::endl <<
"  `reqitemcount2` integer unsigned NOT NULL," << std::endl <<
"  `reqitemcount3` integer unsigned NOT NULL," << std::endl <<
"  `reqitemcount4` integer unsigned NOT NULL," << std::endl <<
"  `reqitemcount5` integer unsigned NOT NULL," << std::endl <<
"  `reqpersonalarenarating` integer unsigned NOT NULL," << std::endl <<
"  PRIMARY KEY  (`extendedcostID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl <<
"INSERT INTO `aowow_item_extended_cost` VALUES" << std::endl;
	while (iter != item_extended_cost_s.end())
	{
		ofile << "(" <<
			(*iter).first << ", " <<
			(*iter).second.reqhonorpoints << ", " <<
			(*iter).second.reqarenapoints << ", " <<
			(*iter).second.reqitem[0] << ", " <<
			(*iter).second.reqitem[1] << ", " <<
			(*iter).second.reqitem[2] << ", " <<
			(*iter).second.reqitem[3] << ", " <<
			(*iter).second.reqitem[4] << ", " <<
			(*iter).second.reqitemcount[0] << ", " <<
			(*iter).second.reqitemcount[1] << ", " <<
			(*iter).second.reqitemcount[2] << ", " <<
			(*iter).second.reqitemcount[3] << ", " <<
			(*iter).second.reqitemcount[4] << ", " <<
			(*iter).second.reqpersonalarenarating << ")";
		if (++iter == item_extended_cost_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump (skill_line_ability_map skill_line_ability_s)
{
	skill_line_ability_map::iterator iter = skill_line_ability_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_skill_line_ability`;" << std::endl <<
"CREATE TABLE `aowow_skill_line_ability` (" << std::endl <<
"  `skillID` integer unsigned NOT NULL," << std::endl <<
"  `spellID` integer unsigned NOT NULL," << std::endl <<
"  `racemask` integer unsigned NOT NULL," << std::endl <<
"  `classmask` integer unsigned NOT NULL," << std::endl <<
"  `req_skill_value` integer unsigned NOT NULL," << std::endl <<
"  `max_value` integer unsigned NOT NULL," << std::endl <<
"  `min_value` integer unsigned NOT NULL" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl <<
"ALTER TABLE `aowow_skill_line_ability` ADD INDEX `spellID`(`spellID`);" << std::endl << std::endl <<
"INSERT INTO `aowow_skill_line_ability` VALUES" << std::endl;
  while (iter != skill_line_ability_s.end())
	{
		ofile << "(" <<
			(*iter).second.skillID << ", " <<
			(*iter).second.spellID << ", " <<
			(*iter).second.racemask << ", " <<
			(*iter).second.classmask << ", " <<
			(*iter).second.req_skill_value << ", " <<
			(*iter).second.max_value << ", " <<
			(*iter).second.min_value << ")";
		if (++iter == skill_line_ability_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump (spelldispeltype_map spelldispeltype_s)
{
	spelldispeltype_map::iterator iter = spelldispeltype_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_spelldispeltype`;" << std::endl <<
"CREATE TABLE `aowow_spelldispeltype` (" << std::endl <<
"  `id` integer unsigned NOT NULL," << std::endl <<
"  `name` varchar(40) NOT NULL," << std::endl <<
"  PRIMARY KEY  (`id`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_spelldispeltype` VALUES" << std::endl;
  while (iter != spelldispeltype_s.end())
	{
		ofile << "(" << (*iter).first << ", '" << (*iter).second.name << "')";
		if (++iter == spelldispeltype_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump (spellmechanic_map spellmechanic_s)
{
	spellmechanic_map::iterator iter = spellmechanic_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_spellmechanic`;" << std::endl <<
"CREATE TABLE `aowow_spellmechanic` (" << std::endl <<
"  `id` integer unsigned NOT NULL," << std::endl <<
"  `name` varchar(40) NOT NULL," << std::endl <<
"  PRIMARY KEY  (`id`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_spellmechanic` VALUES" << std::endl;
  while (iter != spellmechanic_s.end())
	{
		ofile << "(" << (*iter).first << ", '" << (*iter).second.name << "')";
		if (++iter == spellmechanic_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump (resistances_map resistances_s)
{
	resistances_map::iterator iter = resistances_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_resistances`;" << std::endl <<
"CREATE TABLE `aowow_resistances` (" << std::endl <<
"  `id` integer unsigned NOT NULL," << std::endl <<
"  `name` varchar(40) NOT NULL," << std::endl <<
"  PRIMARY KEY  (`id`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_resistances` VALUES" << std::endl;
  while (iter != resistances_s.end())
	{
		ofile << "(" << (*iter).first << ", '" << (*iter).second.name << "')";
		if (++iter == resistances_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump (spellcasttimes_map spellcasttimes_s)
{
	spellcasttimes_map::iterator iter = spellcasttimes_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_spellcasttimes`;" << std::endl <<
"CREATE TABLE `aowow_spellcasttimes` (" << std::endl <<
"  `id` integer unsigned NOT NULL," << std::endl <<
"  `base` integer NOT NULL," << std::endl <<
"  PRIMARY KEY  (`id`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_spellcasttimes` VALUES" << std::endl;
  while (iter != spellcasttimes_s.end())
	{
		ofile << "(" << (*iter).first << ", '" << (*iter).second.base << "')";
		if (++iter == spellcasttimes_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump (lock_map lock_s)
{
	unsigned int num = 0;
	lock_map::iterator iter = lock_s.begin();
	ofile << 
"DROP TABLE IF EXISTS `aowow_lock`;" << std::endl <<
"CREATE TABLE `aowow_lock` (" << std::endl <<
"  `lockID` integer unsigned NOT NULL," << std::endl <<
"  `type1` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `type2` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `type3` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `type4` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `type5` tinyint(1) unsigned NOT NULL," << std::endl <<
//"  `type6` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `lockproperties1` integer unsigned NOT NULL," << std::endl <<
"  `lockproperties2` integer unsigned NOT NULL," << std::endl <<
"  `lockproperties3` integer unsigned NOT NULL," << std::endl <<
"  `lockproperties4` integer unsigned NOT NULL," << std::endl <<
"  `lockproperties5` integer unsigned NOT NULL," << std::endl <<
//"  `lockproperties6` integer unsigned NOT NULL," << std::endl <<
"  `requiredskill1` integer unsigned NOT NULL," << std::endl <<
"  `requiredskill2` integer unsigned NOT NULL," << std::endl <<
"  `requiredskill3` integer unsigned NOT NULL," << std::endl <<
"  `requiredskill4` integer unsigned NOT NULL," << std::endl <<
"  `requiredskill5` integer unsigned NOT NULL," << std::endl <<
//"  `requiredskill6` integer unsigned NOT NULL," << std::endl <<
"  PRIMARY KEY  (`lockID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Locks';" << std::endl << std::endl <<
"LOCK TABLES `aowow_lock` WRITE;" <<std::endl <<
"/*!40000 ALTER TABLE `aowow_lock` DISABLE KEYS */;" << std::endl <<
"INSERT INTO `aowow_lock` VALUES" << std::endl;
	while (iter != lock_s.end())
	{
		ofile << "(" << (*iter).first << ", " <<
			(*iter).second.type1 << ", " <<
			(*iter).second.type2 << ", " <<
			(*iter).second.type3 << ", " <<
			(*iter).second.type4 << ", " <<
			(*iter).second.type5 << ", " <<
//			(*iter).second.type6 << ", " <<
			(*iter).second.lockproperties1 << ", " <<
			(*iter).second.lockproperties2 << ", " <<
			(*iter).second.lockproperties3 << ", " <<
			(*iter).second.lockproperties4 << ", " <<
			(*iter).second.lockproperties5 << ", " <<
//			(*iter).second.lockproperties6 << ", " <<
			(*iter).second.requiredskill1 << ", " <<
			(*iter).second.requiredskill2 << ", " <<
			(*iter).second.requiredskill3 << ", " <<
			(*iter).second.requiredskill4 << ", " <<
			(*iter).second.requiredskill5 << ") ";
//			(*iter).second.requiredskill6 << ")";
		num++;
/*
		iter++;
		if (iter == lock_s.end())
		{
			ofile << ";" << std::endl;
			break;
		}
		if (num>3000)
		{
			ofile << ";" << std::endl;
			ofile << "INSERT INTO `aowow_lock` VALUES";
			num = 0;
		} else {
			ofile << ",";
		}
*/
		if (++iter == lock_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	ofile << "/*!40000 ALTER TABLE `aowow_lock` ENABLE KEYS */;" << std::endl;
	ofile << "UNLOCK TABLES;" << std::endl;
	return;
}

void dump (itemset_map itemset_s)
{
	itemset_map::iterator iter = itemset_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_itemset`;" << std::endl <<
"CREATE TABLE `aowow_itemset` (" << std::endl <<
"  `itemsetID` smallint(3) unsigned NOT NULL," << std::endl <<
"  `name` varchar(34) NOT NULL," << std::endl <<
"  `item1` integer unsigned NOT NULL," << std::endl <<
"  `item2` integer unsigned NOT NULL," << std::endl <<
"  `item3` integer unsigned NOT NULL," << std::endl <<
"  `item4` integer unsigned NOT NULL," << std::endl <<
"  `item5` integer unsigned NOT NULL," << std::endl <<
"  `item6` integer unsigned NOT NULL," << std::endl <<
"  `item7` integer unsigned NOT NULL," << std::endl <<
"  `item8` integer unsigned NOT NULL," << std::endl <<
"  `item9` integer unsigned NOT NULL," << std::endl <<
"  `item10` integer unsigned NOT NULL," << std::endl <<
"  `spell1` smallint(5) unsigned NOT NULL," << std::endl <<
"  `spell2` smallint(5) unsigned NOT NULL," << std::endl <<
"  `spell3` smallint(5) unsigned NOT NULL," << std::endl <<
"  `spell4` smallint(5) unsigned NOT NULL," << std::endl <<
"  `spell5` smallint(5) unsigned NOT NULL," << std::endl <<
"  `spell6` smallint(5) unsigned NOT NULL," << std::endl <<
"  `spell7` smallint(5) unsigned NOT NULL," << std::endl <<
"  `spell8` smallint(5) unsigned NOT NULL," << std::endl <<
"  `bonus1` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `bonus2` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `bonus3` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `bonus4` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `bonus5` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `bonus6` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `bonus7` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `bonus8` tinyint(1) unsigned NOT NULL," << std::endl <<
"  `skillID` smallint(3) unsigned NOT NULL," << std::endl <<
"  `skilllevel` smallint(3) unsigned NOT NULL," << std::endl <<
"  PRIMARY KEY  (`itemsetID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_itemset` VALUES" << std::endl;
	while (iter != itemset_s.end())
	{
		ofile << "(" << (*iter).first << ", '" <<
		mysql_replace((*iter).second.name) << "', " <<
		(*iter).second.item1 << ", " <<
		(*iter).second.item2 << ", " <<
		(*iter).second.item3 << ", " <<
		(*iter).second.item4 << ", " <<
		(*iter).second.item5 << ", " <<
		(*iter).second.item6 << ", " <<
		(*iter).second.item7 << ", " <<
		(*iter).second.item8 << ", " <<
		(*iter).second.item9 << ", " <<
		(*iter).second.item10 << ", " <<
		(*iter).second.spell1 << ", " <<
		(*iter).second.spell2 << ", " <<
		(*iter).second.spell3 << ", " <<
		(*iter).second.spell4 << ", " <<
		(*iter).second.spell5 << ", " <<
		(*iter).second.spell6 << ", " <<
		(*iter).second.spell7 << ", " <<
		(*iter).second.spell8 << ", " <<
		(*iter).second.bonus1 << ", " <<
		(*iter).second.bonus2 << ", " <<
		(*iter).second.bonus3 << ", " <<
		(*iter).second.bonus4 << ", " <<
		(*iter).second.bonus5 << ", " <<
		(*iter).second.bonus6 << ", " <<
		(*iter).second.bonus7 << ", " <<
		(*iter).second.bonus8 << ", " <<
		(*iter).second.skillID << ", " <<
		(*iter).second.skilllevel << ")";
		if (++iter == itemset_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump (spellicons_map spellicons_s)
{
	spellicons_map::iterator iter = spellicons_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_spellicons`;" << std::endl <<
"CREATE TABLE `aowow_spellicons` (" << std::endl <<
"  `id` int(11) unsigned NOT NULL default '0' COMMENT 'Icon Identifier'," << std::endl <<
"  `iconname` varchar(50) NOT NULL default '' COMMENT 'Icon Name'," << std::endl <<
"  PRIMARY KEY  (`id`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Icon Names';" << std::endl << std::endl <<
"INSERT INTO `aowow_spellicons` VALUES" << std::endl;
  while (iter != spellicons_s.end())
	{
		ofile << "(" << (*iter).first << ", '" << (*iter).second.iconname << "')";
		if (++iter == spellicons_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump (icons_map icons_s)
{
	icons_map::iterator iter = icons_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_icons`;" << std::endl <<
"CREATE TABLE `aowow_icons` (" << std::endl <<
"  `id` int(11) unsigned NOT NULL default '0' COMMENT 'Icon Identifier'," << std::endl <<
"  `iconname` varchar(50) NOT NULL default '' COMMENT 'Icon Name'," << std::endl <<
"  PRIMARY KEY  (`id`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Icon Names';" << std::endl << std::endl <<
"INSERT INTO `aowow_icons` VALUES" << std::endl;
  while (iter != icons_s.end())
	{
		ofile << "(" << (*iter).first << ", '" << (*iter).second.iconname << "')";
		if (++iter == icons_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump (gemproperties_map gemproperties_s)
{
	gemproperties_map::iterator iter = gemproperties_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_gemproperties`;" << std::endl <<
"CREATE TABLE `aowow_gemproperties` (" << std::endl <<
"  `gempropertiesID` smallint(3) unsigned NOT NULL," << std::endl <<
"  `itemenchantmetID` smallint(3) unsigned NOT NULL," << std::endl <<
"  PRIMARY KEY  (`gempropertiesID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_gemproperties` VALUES" << std::endl;
	while (iter != gemproperties_s.end())
	{
		ofile << "(" << (*iter).first << ", '" << (*iter).second.itemenchantmetID << "')";
		if (++iter == gemproperties_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
};

void dump (faction_map faction_s)
{
	faction_map::iterator iter = faction_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_factions`;" << std::endl <<
"CREATE TABLE `aowow_factions` (" << std::endl <<
"  `factionID` integer unsigned NOT NULL," << std::endl <<
"  `reputationListID` integer NOT NULL," << std::endl <<
"  `team` integer unsigned NOT NULL," << std::endl <<
"  `side` tinyint(1) NOT NULL," << std::endl <<
"  `name` varchar(40) NOT NULL," << std::endl <<
"  `description1` text COMMENT 'Description from client'," << std::endl <<
"  `description2` text COMMENT 'Description from wowwiki.com'," << std::endl <<
"  PRIMARY KEY  (`factionID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_factions` VALUES" << std::endl;
  while (iter != faction_s.end())
	{
		ofile << "(" << (*iter).first << ", " <<
		(*iter).second.reputationListID << ", " <<
		(*iter).second.team << ", " <<
		(*iter).second.side << ", '" <<
		mysql_replace((*iter).second.name) << "', '" <<
		mysql_replace((*iter).second.description) << 
		"', '')";
		if (++iter == faction_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump (factiontemplate_map factiontemplate_s)
{
	int A = 0, H = 0;
	factiontemplate_map::iterator iter = factiontemplate_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_factiontemplate`;" << std::endl <<
"CREATE TABLE `aowow_factiontemplate` (" << std::endl <<
"  `factiontemplateID` smallint(4) unsigned NOT NULL," << std::endl <<
"  `factionID` smallint(4) NOT NULL," << std::endl <<
"  `A` smallint(1) NOT NULL COMMENT 'Aliance: -1 - hostile, 1 - friendly, 0 - neutral'," << std::endl <<
"  `H` smallint(1) NOT NULL COMMENT 'Horde: -1 - hostile, 1 - friendly, 0 - neutral'," << std::endl <<
"  PRIMARY KEY  (`factiontemplateID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_factiontemplate` VALUES" << std::endl;
	while (iter != factiontemplate_s.end())
	{
		// Отношения с фракцией Horde (1801) - Орда
		if ((*iter).second.IsFriendlyTo(factiontemplate_s[1801]))
			H = 1;
		else if ((*iter).second.IsHostileTo(factiontemplate_s[1801]))
			H = -1;
		else
			H = 0;
		// Отношения с фракцией Alliance (1802) - Альянс
		if ((*iter).second.IsFriendlyTo(factiontemplate_s[1802]))
			A = 1;
		else if ((*iter).second.IsHostileTo(factiontemplate_s[1802]))
			A = -1;
		else
			A = 0;

		ofile << "(" << (*iter).first << ", " <<
		(*iter).second.factionID << ", " <<
		A << ", " << H << ")";
		if (++iter == factiontemplate_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump (itemenchantmet_map itemenchantmet_s)
{
	itemenchantmet_map::iterator iter = itemenchantmet_s.begin();
	ofile <<
"DROP TABLE IF EXISTS `aowow_itemenchantmet`;" << std::endl <<
"CREATE TABLE `aowow_itemenchantmet` (" << std::endl <<
"  `itemenchantmetID` smallint(3) unsigned NOT NULL," << std::endl <<
"  `text` text NOT NULL," << std::endl <<
"  PRIMARY KEY  (`itemenchantmetID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_itemenchantmet` VALUES" << std::endl;
	while (iter != itemenchantmet_s.end())
	{
		ofile << "(" << (*iter).first << ", '" << (*iter).second.text << "')";
		if (++iter == itemenchantmet_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
};

void dump(aowow_zones_map aowow_zones_s)
{
	aowow_zones_map::iterator iter = aowow_zones_s.begin();
	ofile << 
"DROP TABLE IF EXISTS `aowow_zones`;" << std::endl <<
"CREATE TABLE `aowow_zones` (" << std::endl <<
"  `mapID` smallint(3) unsigned NOT NULL COMMENT 'Map Identifier'," << std::endl <<
"  `areatableID`  smallint(3) unsigned NOT NULL COMMENT 'Zone Id'," << std::endl <<
"  `name` varchar(40) NOT NULL COMMENT 'Map Name'," << std::endl <<
"  `x_min` float NOT NULL DEFAULT 0.0," << std::endl <<
"  `y_min` float NOT NULL DEFAULT 0.0," << std::endl <<
"  `x_max` float NOT NULL DEFAULT 0.0," << std::endl <<
"  `y_max` float NOT NULL DEFAULT 0.0" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Maps';" << std::endl << std::endl <<
"INSERT INTO `aowow_zones` VALUES" << std::endl;

	while (iter != aowow_zones_s.end())
	{
		ofile << "(" << (*iter).second.mapID << ", " <<
		(*iter).second.areatableID << ", '" <<
		mysql_replace((*iter).second.name) << "', " <<
		(*iter).second.x_min << ", " <<
		(*iter).second.y_min << ", " <<
		(*iter).second.x_max << ", " <<
		(*iter).second.y_max << ")";
		if (++iter == aowow_zones_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
};

void dump (skill_map skill_s)
{
	skill_map::iterator iter = skill_s.begin();
	ofile << 
"DROP TABLE IF EXISTS `aowow_skill`;" << std::endl <<
"CREATE TABLE `aowow_skill` (" << std::endl <<
"  `skillID` integer unsigned NOT NULL," << std::endl <<
"  `categoryID` integer unsigned NOT NULL," << std::endl <<
"  `name` text NOT NULL," << std::endl <<
"  PRIMARY KEY  (`skillID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_skill` VALUES" << std::endl;
	while (iter != skill_s.end())
	{
		ofile << "(" << (*iter).first << ", " <<
		(*iter).second.categoryID << ", '" <<
		(*iter).second.name << "')";
		if (++iter == skill_s.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
};

void dump(spellrangemap & spellrange)
{
	spellrangemap::iterator iter = spellrange.begin();
	ofile << 
"DROP TABLE IF EXISTS `aowow_spellrange`;" << std::endl <<
"CREATE TABLE `aowow_spellrange` (" << std::endl <<
"  `rangeID` integer unsigned NOT NULL," << std::endl <<
"  `rangeMin` float NOT NULL," << std::endl <<
"  `rangeMax` float NOT NULL," << std::endl <<
"  `name` varchar(40) NOT NULL," << std::endl <<
"  PRIMARY KEY  (`rangeID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_spellrange` VALUES" << std::endl;
	while (iter != spellrange.end())
	{
		ofile << "(" << (*iter).first << ", '" <<
		(*iter).second.rangeMin << "', '" <<
		(*iter).second.rangeMax << "', '" <<
		(*iter).second.name << "')";
		if (++iter == spellrange.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump(spelldurationmap & spellduration)
{
	spelldurationmap::iterator iter = spellduration.begin();
	ofile << 
"DROP TABLE IF EXISTS `aowow_spellduration`;" << std::endl <<
"CREATE TABLE `aowow_spellduration` (" << std::endl <<
"  `durationID` smallint(3) unsigned NOT NULL," << std::endl <<
"  `durationBase` int(10) signed NOT NULL," << std::endl <<
"  PRIMARY KEY  (`durationID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_spellduration` VALUES" << std::endl;
	while (iter != spellduration.end())
	{
		ofile << "(" << (*iter).first << ", '" << (*iter).second.durationBase << "')";
		if (++iter == spellduration.end())
			ofile << ";";
		else
			ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump(spellradiusmap & spellradius)
{
	spellradiusmap::iterator iter = spellradius.begin();
	ofile << 
"DROP TABLE IF EXISTS `aowow_spellradius`;" << std::endl <<
"CREATE TABLE `aowow_spellradius` (" << std::endl <<
"  `radiusID` smallint(5) unsigned NOT NULL," << std::endl <<
"  `radiusBase` float NOT NULL," << std::endl <<
"  PRIMARY KEY  (`radiusID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;" << std::endl << std::endl <<
"INSERT INTO `aowow_spellradius` VALUES" << std::endl;
	while (iter != spellradius.end())
	{
		ofile << "(" << (*iter).first << ", '" << (*iter).second.radiusBase << "')";
		if (++iter == spellradius.end())
			ofile << ";";
		else
			ofile << ",";
		ofile << std::endl;
	}
	return;
}

void dump(spellsmap & spells)
{
	unsigned int num = 0;
	spellsmap::iterator iter = spells.begin();
	ofile << 
"DROP TABLE IF EXISTS `aowow_spell`;" << std::endl <<
"CREATE TABLE `aowow_spell` (" << std::endl <<
"  `spellID` smallint(5) unsigned NOT NULL," << std::endl <<
"  `resistancesID` integer unsigned NOT NULL," << std::endl <<
"  `dispeltypeID` integer unsigned NOT NULL," << std::endl <<
"  `mechanicID` integer unsigned NOT NULL," << std::endl <<
"  `spellcasttimesID` integer unsigned NOT NULL," << std::endl <<
"  `durationID` smallint(3) unsigned NOT NULL," << std::endl <<
"  `rangeID` tinyint(3) unsigned NOT NULL," << std::endl <<
"  `cooldown` integer unsigned NOT NULL," << std::endl <<
"  `effect1id` integer unsigned NOT NULL," << std::endl <<
"  `effect2id` integer unsigned NOT NULL," << std::endl <<
"  `effect3id` integer unsigned NOT NULL," << std::endl <<
"  `effect1radius` integer unsigned NOT NULL," << std::endl <<
"  `effect2radius` integer unsigned NOT NULL," << std::endl <<
"  `effect3radius` integer unsigned NOT NULL," << std::endl <<
"  `effect1Aura` smallint(5) unsigned NOT NULL," << std::endl <<
"  `effect2Aura` smallint(5) unsigned NOT NULL," << std::endl <<
"  `effect3Aura` smallint(5) unsigned NOT NULL," << std::endl <<
"  `effect1BasePoints` mediumint(7) signed NOT NULL," << std::endl <<
"  `effect2BasePoints` mediumint(7) signed NOT NULL," << std::endl <<
"  `effect3BasePoints` mediumint(7) signed NOT NULL," << std::endl <<
"  `effect1DieSides` mediumint(7) signed NOT NULL," << std::endl <<
"  `effect2DieSides` mediumint(7) signed NOT NULL," << std::endl <<
"  `effect3DieSides` mediumint(7) signed NOT NULL," << std::endl <<
"  `effect1Amplitude` mediumint(7) unsigned NOT NULL," << std::endl <<
"  `effect2Amplitude` mediumint(7) unsigned NOT NULL," << std::endl <<
"  `effect3Amplitude` mediumint(7) unsigned NOT NULL," << std::endl <<
"  `effect1ChainTarget` tinyint(2) unsigned NOT NULL," << std::endl <<
"  `effect2ChainTarget` tinyint(2) unsigned NOT NULL," << std::endl <<
"  `effect3ChainTarget` tinyint(2) unsigned NOT NULL," << std::endl <<
"  `effect1MiscValue` mediumint(7) signed NOT NULL," << std::endl <<
"  `effect2MiscValue` mediumint(7) signed NOT NULL," << std::endl <<
"  `effect3MiscValue` mediumint(7) signed NOT NULL," << std::endl <<
"  `effect1triggerspell` integer unsigned NOT NULL," << std::endl <<
"  `effect2triggerspell` integer unsigned NOT NULL," << std::endl <<
"  `effect3triggerspell` integer unsigned NOT NULL," << std::endl <<
"  `procChance` tinyint(3) unsigned NOT NULL," << std::endl <<
"  `dmg_multiplier1` float NOT NULL," << std::endl <<
"  `dmg_multiplier2` float NOT NULL," << std::endl <<
"  `dmg_multiplier3` float NOT NULL," << std::endl <<
"  `procCharges` tinyint(3) unsigned NOT NULL," << std::endl <<
"  `spellTargets` tinyint(3) unsigned NOT NULL," << std::endl <<
"  `effect_1_proc_value` float NOT NULL," << std::endl <<
"  `effect_2_proc_value` float NOT NULL," << std::endl <<
"  `effect_3_proc_value` float NOT NULL," << std::endl <<
"  `manacostpercent` integer unsigned NOT NULL," << std::endl <<
"  `affected_target_level` integer unsigned NOT NULL," << std::endl <<
"  `effect_1_proc_chance` float NOT NULL," << std::endl <<
"  `effect_2_proc_chance` float NOT NULL," << std::endl <<
"  `effect_3_proc_chance` float NOT NULL," << std::endl <<
"  `spellname` text NOT NULL," << std::endl <<
"  `rank` text NOT NULL," << std::endl <<
"  `tooltip` text NOT NULL," << std::endl <<
"  `buff` text NOT NULL," << std::endl <<
"  `tool1` integer unsigned NOT NULL, " << std::endl <<
"  `tool2` integer unsigned NOT NULL, " << std::endl <<
"  `reagent1` integer unsigned NOT NULL, " << std::endl <<
"  `reagent2` integer unsigned NOT NULL, " << std::endl <<
"  `reagent3` integer unsigned NOT NULL, " << std::endl <<
"  `reagent4` integer unsigned NOT NULL, " << std::endl <<
"  `reagent5` integer unsigned NOT NULL, " << std::endl <<
"  `reagent6` integer unsigned NOT NULL, " << std::endl <<
"  `reagent7` integer unsigned NOT NULL, " << std::endl <<
"  `reagent8` integer unsigned NOT NULL, " << std::endl <<
"  `reagentcount1` integer unsigned NOT NULL, " << std::endl <<
"  `reagentcount2` integer unsigned NOT NULL, " << std::endl <<
"  `reagentcount3` integer unsigned NOT NULL, " << std::endl <<
"  `reagentcount4` integer unsigned NOT NULL, " << std::endl <<
"  `reagentcount5` integer unsigned NOT NULL, " << std::endl <<
"  `reagentcount6` integer unsigned NOT NULL, " << std::endl <<
"  `reagentcount7` integer unsigned NOT NULL, " << std::endl <<
"  `reagentcount8` integer unsigned NOT NULL, " << std::endl <<
"  `effect1itemtype` integer unsigned NOT NULL, " << std::endl <<
"  `effect2itemtype` integer unsigned NOT NULL, " << std::endl <<
"  `effect3itemtype` integer unsigned NOT NULL, " << std::endl <<
"  `spellicon` integer unsigned NOT NULL, " << std::endl <<
"  `levelspell` integer unsigned NOT NULL, " << std::endl <<
"  `manacost` integer unsigned NOT NULL, " << std::endl <<
"  `ChannelInterruptFlags` integer unsigned NOT NULL, " << std::endl <<
"  PRIMARY KEY  (`spellID`)" << std::endl <<
") ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Spells';" << std::endl << std::endl <<
"LOCK TABLES `aowow_spell` WRITE;" <<std::endl <<
"/*!40000 ALTER TABLE `aowow_spell` DISABLE KEYS */;" << std::endl <<
"INSERT INTO `aowow_spell` VALUES" << std::endl;

	while (iter != spells.end())
	{
		ofile << "(" << (*iter).first << ", " <<
			(*iter).second.resistancesID << ", " <<
			(*iter).second.dispeltypeID << ", " <<
			(*iter).second.mechanicID << ", " <<
			(*iter).second.spellcasttimesID << ", " <<
			(*iter).second.durationID << ", " <<
			(*iter).second.rangeID << ", " <<
			(*iter).second.cooldown << ", " <<
			(*iter).second.effect1id << ", " <<
			(*iter).second.effect2id << ", " <<
			(*iter).second.effect3id << ", " <<
			(*iter).second.effect1radius << ", " <<
			(*iter).second.effect2radius << ", " <<
			(*iter).second.effect3radius << ", " <<
			(*iter).second.effect1Aura << ", " <<
			(*iter).second.effect2Aura << ", " <<
			(*iter).second.effect3Aura << ", " <<
			(*iter).second.effect1BasePoints << ", " <<
			(*iter).second.effect2BasePoints << ", " <<
			(*iter).second.effect3BasePoints << ", " <<
			(*iter).second.effect1DieSides << ", " <<
			(*iter).second.effect2DieSides << ", " <<
			(*iter).second.effect3DieSides << ", " <<
			(*iter).second.effect1Amplitude << ", " <<
			(*iter).second.effect2Amplitude << ", " <<
			(*iter).second.effect3Amplitude << ", " <<
			(*iter).second.effect1ChainTarget << ", " <<
			(*iter).second.effect2ChainTarget << ", " <<
			(*iter).second.effect3ChainTarget << ", " <<
			(*iter).second.effect1MiscValue << ", " <<
			(*iter).second.effect2MiscValue << ", " <<
			(*iter).second.effect3MiscValue << ", " <<
			(*iter).second.effect1triggerspell << ", " <<
			(*iter).second.effect2triggerspell << ", " <<
			(*iter).second.effect3triggerspell << ", " <<
			(*iter).second.procChance <<  ", " <<
			(*iter).second.dmg_multiplier1 << ", " <<
			(*iter).second.dmg_multiplier2 << ", " <<
			(*iter).second.dmg_multiplier3 << ", " <<
			(*iter).second.procCharges << ", " <<
			(*iter).second.spellTargets << ", " <<
			(*iter).second.effect_1_proc_value << ", " <<
			(*iter).second.effect_2_proc_value << ", " <<
			(*iter).second.effect_3_proc_value << ", " <<
			(*iter).second.manacostpercent << ", " <<
			(*iter).second.affected_target_level << ", " <<
			(*iter).second.effect_1_proc_chance << ", " <<
			(*iter).second.effect_2_proc_chance << ", " <<
			(*iter).second.effect_3_proc_chance << ", '" <<
			mysql_replace((*iter).second.name) << "', '" <<
			mysql_replace((*iter).second.rank) << "', '" <<
			replace(mysql_replace((*iter).second.tooltip),"\r\n","<br>") << "', '" <<
			replace(mysql_replace((*iter).second.buff),"\r\n","<br>") << "', " <<
			(*iter).second.tool1 << ", " <<
			(*iter).second.tool2 << ", " <<
			(*iter).second.reagent1 << ", " <<
			(*iter).second.reagent2 << ", " <<
			(*iter).second.reagent3 << ", " <<
			(*iter).second.reagent4 << ", " <<
			(*iter).second.reagent5 << ", " <<
			(*iter).second.reagent6 << ", " <<
			(*iter).second.reagent7 << ", " <<
			(*iter).second.reagent8 << ", " <<
			(*iter).second.reagentcount1 << ", " <<
			(*iter).second.reagentcount2 << ", " <<
			(*iter).second.reagentcount3 << ", " <<
			(*iter).second.reagentcount4 << ", " <<
			(*iter).second.reagentcount5 << ", " <<
			(*iter).second.reagentcount6 << ", " <<
			(*iter).second.reagentcount7 << ", " <<
			(*iter).second.reagentcount8 << ", " <<
			(*iter).second.effect1itemtype << ", " <<
			(*iter).second.effect2itemtype << ", " <<
			(*iter).second.effect3itemtype << ", " <<
			(*iter).second.spellicon << ", " <<
			(*iter).second.levelspell << ", " <<
			(*iter).second.manacost << ", " <<
			(*iter).second.ChannelInterruptFlags << ")";
		num++;
		
		iter++;
		if (iter == spells.end())
		{
			ofile << ";" << std::endl;
			break;
		}
		if (num>1000)
		{
			ofile << ";" << std::endl;
			ofile << "INSERT INTO `aowow_spell` VALUES";
			num = 0;
		} else {
			ofile << ",";
		}

		//if (++iter == spells.end()) ofile << ";"; else ofile << ",";
		ofile << std::endl;
	}
	ofile <<
	"/*!40000 ALTER TABLE `aowow_spell` ENABLE KEYS */;" << std::endl <<
	"UNLOCK TABLES;" << std::endl;
	return;
}

int main()
{
	// Ассоциативный массив всех спеллов по уникальному идентификатору - id
	spellcasttimes_map spellcasttimes_s;
	faction_map faction_s;
	itemenchantmet_map itemenchantmet_s;
	spellsmap spells;
	spelldurationmap spelldurations;
	spellrangemap spellranges;
	spellradiusmap spellradiuss;
	skill_map skill_s;
	areatable_map areatable_s;
	worldmaparea_map worldmaparea_s;
	map_map map_s;
	aowow_zones_map aowow_zones_s;
	gemproperties_map gemproperties_s;
	icons_map icons_s;
	spellicons_map spellicons_s;
	itemset_map itemset_s;
	factiontemplate_map factiontemplate_s;
	lock_map lock_s;
	resistances_map resistances_s;
	spelldispeltype_map spelldispeltype_s;
	spellmechanic_map spellmechanic_s;
	skill_line_ability_map skill_line_ability_s;
	item_extended_cost_map item_extended_cost_s;
	char_titles_map char_titles_s;

	DBCFile SpellDBC, SpellDurationDBC, SpellRangeDBC, SpellRadiusDBC, SkillLineDBC;
	DBCFile WorldMapAreaDBC, AreaTableDBC, MapDBC, SpellItemEnchantmentDBC, FactionDBC;
	DBCFile GemPropertiesDBC, ItemDisplayInfoDBC, ItemSetDBC, FactionTemplateDBC;
	DBCFile LockDBC, SpellIconDBC, SpellCastTimesDBC, ResistancesDBC;
	DBCFile SpellDispelTypeDBC, SpellMechanicDBC, SkillLineAbilityDBC;
	DBCFile ItemExtendedCostDBC, CharTitlesDBC;
	unsigned int i, id;
	std::string name;

	CharTitlesDBC.Load("./dbc/CharTitles.dbc","nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxi");
	if (CharTitlesDBC.IsLoaded())
	{
		std::cout << "Info: CharTitles.dbc succesfully loaded" << std::endl;
		for (i=0;i<CharTitlesDBC.GetNumRows();i++)
		{
			id = CharTitlesDBC.getRecord(i).getUInt(0);
			char_titles_s[id].name = CharTitlesDBC.getRecord(i).getString(2);
//			char_titles_s[id].name2 = CharTitlesDBC.getRecord(i).getString(19);
		}
	} else {
		std::cout << "Error: CharTitles.dbc was not Loaded";
	}
	dump(char_titles_s);

	ItemExtendedCostDBC.Load("./dbc/ItemExtendedCost.dbc","niiiiiiiiiiiii");
	if (ItemExtendedCostDBC.IsLoaded())
	{
		std::cout << "Info: ItemExtendedCost.dbc succesfully loaded" << std::endl;
		for (i=0;i<ItemExtendedCostDBC.GetNumRows();i++)
		{
			id = ItemExtendedCostDBC.getRecord(i).getUInt(0);
			item_extended_cost_s[id].reqhonorpoints = ItemExtendedCostDBC.getRecord(i).getUInt(1);
			item_extended_cost_s[id].reqarenapoints = ItemExtendedCostDBC.getRecord(i).getUInt(2);
			item_extended_cost_s[id].reqitem[0] = ItemExtendedCostDBC.getRecord(i).getUInt(3);
			item_extended_cost_s[id].reqitem[1] = ItemExtendedCostDBC.getRecord(i).getUInt(4);
			item_extended_cost_s[id].reqitem[2] = ItemExtendedCostDBC.getRecord(i).getUInt(5);
			item_extended_cost_s[id].reqitem[3] = ItemExtendedCostDBC.getRecord(i).getUInt(6);
			item_extended_cost_s[id].reqitem[4] = ItemExtendedCostDBC.getRecord(i).getUInt(7);
			item_extended_cost_s[id].reqitemcount[0] = ItemExtendedCostDBC.getRecord(i).getUInt(8);
			item_extended_cost_s[id].reqitemcount[1] = ItemExtendedCostDBC.getRecord(i).getUInt(9);
			item_extended_cost_s[id].reqitemcount[2] = ItemExtendedCostDBC.getRecord(i).getUInt(10);
			item_extended_cost_s[id].reqitemcount[3] = ItemExtendedCostDBC.getRecord(i).getUInt(11);
			item_extended_cost_s[id].reqitemcount[4] = ItemExtendedCostDBC.getRecord(i).getUInt(12);
			item_extended_cost_s[id].reqpersonalarenarating = ItemExtendedCostDBC.getRecord(i).getUInt(13);
		}
	} else {
		std::cout << "Error: ItemExtendedCost.dbc was not Loaded";
	}
	dump(item_extended_cost_s);

	SkillLineAbilityDBC.Load("./dbc/SkillLineAbility.dbc","xiniixxiiiiixxi");
	if (SkillLineAbilityDBC.IsLoaded())
	{
		std::cout << "Info: SkillLineAbility.dbc succesfully loaded" << std::endl;
		for (i=0;i<SkillLineAbilityDBC.GetNumRows();i++)
		{
			id = SkillLineAbilityDBC.getRecord(i).getUInt(0);
			skill_line_ability_s[id].skillID = SkillLineAbilityDBC.getRecord(i).getUInt(1);
			skill_line_ability_s[id].spellID = SkillLineAbilityDBC.getRecord(i).getUInt(2);
			skill_line_ability_s[id].racemask = SkillLineAbilityDBC.getRecord(i).getUInt(3);
			skill_line_ability_s[id].classmask = SkillLineAbilityDBC.getRecord(i).getUInt(4);
			skill_line_ability_s[id].req_skill_value = SkillLineAbilityDBC.getRecord(i).getUInt(7);
			skill_line_ability_s[id].forward_spellid = SkillLineAbilityDBC.getRecord(i).getUInt(8);
			skill_line_ability_s[id].learnOnGetSkill = SkillLineAbilityDBC.getRecord(i).getUInt(9);
			skill_line_ability_s[id].max_value = SkillLineAbilityDBC.getRecord(i).getUInt(10);
			skill_line_ability_s[id].min_value = SkillLineAbilityDBC.getRecord(i).getUInt(11);
			skill_line_ability_s[id].reqtrainpoints = SkillLineAbilityDBC.getRecord(i).getUInt(14);
		}
	} else {
		std::cout << "Error: SkillLineAbility.dbc was not Loaded";
	}
	dump(skill_line_ability_s);

	SpellDispelTypeDBC.Load("./dbc/SpellDispelType.dbc","ns");
	if (SpellDispelTypeDBC.IsLoaded())
	{
		std::cout << "Info: SpellDispelType.dbc succesfully loaded" << std::endl;
		for (i=0;i<SpellDispelTypeDBC.GetNumRows();i++)
		{
			id = SpellDispelTypeDBC.getRecord(i).getUInt(0);
			spelldispeltype_s[id].name = SpellDispelTypeDBC.getRecord(i).getString(1);
		}
	} else {
		std::cout << "Error: SpellDispelType.dbc was not Loaded";
	}

	SpellMechanicDBC.Load("./dbc/SpellMechanic.dbc","ns");
	if (SpellMechanicDBC.IsLoaded())
	{
		std::cout << "Info: SpellMechanic.dbc succesfully loaded" << std::endl;
		for (i=0;i<SpellMechanicDBC.GetNumRows();i++)
		{
			id = SpellMechanicDBC.getRecord(i).getUInt(0);
			spellmechanic_s[id].name = SpellMechanicDBC.getRecord(i).getString(1);
		}
	} else {
		std::cout << "Error: SpellMechanic.dbc was not Loaded";
	}

	ResistancesDBC.Load("./dbc/Resistances.dbc","nixx");
	if (ResistancesDBC.IsLoaded())
	{
		std::cout << "Info: Resistances.dbc succesfully loaded" << std::endl;
		for (i=0;i<ResistancesDBC.GetNumRows();i++)
		{
			id = ResistancesDBC.getRecord(i).getUInt(0);
			resistances_s[id].name = ResistancesDBC.getRecord(i).getString(3);
		}
	} else {
		std::cout << "Error: Resistances.dbc was not Loaded";
	}

	SpellCastTimesDBC.Load("./dbc/SpellCastTimes.dbc","nixx");
	if (SpellCastTimesDBC.IsLoaded())
	{
		std::cout << "Info: SpellCastTimes.dbc succesfully loaded" << std::endl;
		for (i=0;i<SpellCastTimesDBC.GetNumRows();i++)
		{
			id = SpellCastTimesDBC.getRecord(i).getUInt(0);
			spellcasttimes_s[id].base = SpellCastTimesDBC.getRecord(i).getUInt(1);
		}
	} else {
		std::cout << "Error: SpellCastTimes.dbc was not Loaded";
	}

	LockDBC.Load("./dbc/Lock.dbc","niiiiixxxiiiiixxxiixxxxxxxxxxxxxx");
	if (LockDBC.IsLoaded())
	{
		std::cout << "Info: Lock.dbc succesfully loaded" << std::endl;
		for (i=0;i<LockDBC.GetNumRows();i++)
		{
			id = LockDBC.getRecord(i).getUInt(0);
			lock_s[id].type1 = LockDBC.getRecord(i).getUInt(1);
			lock_s[id].type2 = LockDBC.getRecord(i).getUInt(2);
			lock_s[id].type3 = LockDBC.getRecord(i).getUInt(3);
			lock_s[id].type4 = LockDBC.getRecord(i).getUInt(4);
			lock_s[id].type5 = LockDBC.getRecord(i).getUInt(5);
			lock_s[id].lockproperties1 = LockDBC.getRecord(i).getUInt(9);
			lock_s[id].lockproperties2 = LockDBC.getRecord(i).getUInt(10);
			lock_s[id].lockproperties3 = LockDBC.getRecord(i).getUInt(11);
			lock_s[id].lockproperties4 = LockDBC.getRecord(i).getUInt(12);
			lock_s[id].lockproperties5 = LockDBC.getRecord(i).getUInt(13);
			lock_s[id].requiredskill1 = LockDBC.getRecord(i).getUInt(17);
			lock_s[id].requiredskill2 = LockDBC.getRecord(i).getUInt(18);
			lock_s[id].requiredskill3 = LockDBC.getRecord(i).getUInt(19);
			lock_s[id].requiredskill4 = LockDBC.getRecord(i).getUInt(20);
			lock_s[id].requiredskill5 = LockDBC.getRecord(i).getUInt(21);
		}
	} else {
		std::cout << "Error: Lock.dbc was not Loaded";
	}


	ItemSetDBC.Load("./dbc/ItemSet.dbc","dssssssssssssssssxxxxxxxxxxxxxxxxxxiiiiiiiiiiiiiiiiii");
	if (ItemSetDBC.IsLoaded())
	{
		std::cout << "ItemSet.dbc succesfully loaded" << std::endl;
		for (i=0;i<ItemSetDBC.GetNumRows();i++)
		{
			id = ItemSetDBC.getRecord(i).getUInt(0);
			itemset_s[id].name = ItemSetDBC.getRecord(i).getString(1);
			itemset_s[id].item1 = ItemSetDBC.getRecord(i).getUInt(18);
			itemset_s[id].item2 = ItemSetDBC.getRecord(i).getUInt(19);
			itemset_s[id].item3 = ItemSetDBC.getRecord(i).getUInt(20);
			itemset_s[id].item4 = ItemSetDBC.getRecord(i).getUInt(21);
			itemset_s[id].item5 = ItemSetDBC.getRecord(i).getUInt(22);
			itemset_s[id].item6 = ItemSetDBC.getRecord(i).getUInt(23);
			itemset_s[id].item7 = ItemSetDBC.getRecord(i).getUInt(24);
			itemset_s[id].item8 = ItemSetDBC.getRecord(i).getUInt(25);
			itemset_s[id].item9 = ItemSetDBC.getRecord(i).getUInt(26);
			itemset_s[id].item10 = ItemSetDBC.getRecord(i).getUInt(27);
			itemset_s[id].spell1 = ItemSetDBC.getRecord(i).getUInt(35);
			itemset_s[id].spell2 = ItemSetDBC.getRecord(i).getUInt(36);
			itemset_s[id].spell3 = ItemSetDBC.getRecord(i).getUInt(37);
			itemset_s[id].spell4 = ItemSetDBC.getRecord(i).getUInt(38);
			itemset_s[id].spell5 = ItemSetDBC.getRecord(i).getUInt(39);
			itemset_s[id].spell6 = ItemSetDBC.getRecord(i).getUInt(40);
			itemset_s[id].spell7 = ItemSetDBC.getRecord(i).getUInt(41);
			itemset_s[id].spell8 = ItemSetDBC.getRecord(i).getUInt(42);
			itemset_s[id].bonus1 = ItemSetDBC.getRecord(i).getUInt(43);
			itemset_s[id].bonus2 = ItemSetDBC.getRecord(i).getUInt(44);
			itemset_s[id].bonus3 = ItemSetDBC.getRecord(i).getUInt(45);
			itemset_s[id].bonus4 = ItemSetDBC.getRecord(i).getUInt(46);
			itemset_s[id].bonus5 = ItemSetDBC.getRecord(i).getUInt(47);
			itemset_s[id].bonus6 = ItemSetDBC.getRecord(i).getUInt(48);
			itemset_s[id].bonus7 = ItemSetDBC.getRecord(i).getUInt(49);
			itemset_s[id].bonus8 = ItemSetDBC.getRecord(i).getUInt(50);
			itemset_s[id].skillID = ItemSetDBC.getRecord(i).getUInt(51);
			itemset_s[id].skilllevel = ItemSetDBC.getRecord(i).getUInt(52);
		}
	} else {
		std::cout << "Error, while loading ItemSet.dbc";
	}

	SpellIconDBC.Load("./dbc/SpellIcon.dbc","ns");
	if (SpellIconDBC.IsLoaded())
	{
		std::cout << "SpellIcon.dbc succesfully loaded" << std::endl;
		for (i=0;i<SpellIconDBC.GetNumRows();i++)
		{
			id = SpellIconDBC.getRecord(i).getUInt(0);
			name = SpellIconDBC.getRecord(i).getString(1);
			if (name.length() > 0)
			{
				name = lowercase(strim(name.substr(16, name.length()-16)));
				spellicons_s[id].iconname = name;
			}
		}
	} else {
		std::cout << "Error, while loading SpellIcon.dbc";
	}

	ItemDisplayInfoDBC.Load("./dbc/ItemDisplayInfo.dbc","nixxi");
	if (ItemDisplayInfoDBC.IsLoaded())
	{
		std::cout << "Info: ItemDisplayInfo.dbc succesfully loaded" << std::endl;
		for (i=0;i<ItemDisplayInfoDBC.GetNumRows();i++)
		{
			id = ItemDisplayInfoDBC.getRecord(i).getUInt(0);
			name = ItemDisplayInfoDBC.getRecord(i).getString(5);
			if (name.length() > 0)
				icons_s[id].iconname = lowercase(strim(name));
		}
	} else {
		std::cout << "Error: while loading ItemDisplayInfo.dbc";
	}

	GemPropertiesDBC.Load("./dbc/GemProperties.dbc","nixxi");
	if (GemPropertiesDBC.IsLoaded())
	{
		std::cout << "GemProperties.dbc succesfully loaded" << std::endl;
		for (i=0;i<GemPropertiesDBC.GetNumRows();i++)
		{
			id = GemPropertiesDBC.getRecord(i).getUInt(0);
			gemproperties_s[id].itemenchantmetID = GemPropertiesDBC.getRecord(i).getUInt(1);
		}
	} else {
		std::cout << "Error, while loading GemProperties.dbc";
	}

	FactionTemplateDBC.Load("./dbc/FactionTemplate.dbc", "nixiiiiiiiiiii");
	if (FactionTemplateDBC.IsLoaded())
	{
		std::cout << "Info: FactionTemplate.dbc was sucessfully Loaded" << std::endl;
		for (i=0;i<FactionTemplateDBC.GetNumRows();i++)
		{
			id = FactionTemplateDBC.getRecord(i).getUInt(0);
			factiontemplate_s[id].factionID = FactionTemplateDBC.getRecord(i).getUInt(1);
			factiontemplate_s[id].ourMask = FactionTemplateDBC.getRecord(i).getUInt(3);
			factiontemplate_s[id].friendlyMask = FactionTemplateDBC.getRecord(i).getUInt(4);
			factiontemplate_s[id].hostileMask = FactionTemplateDBC.getRecord(i).getUInt(5);
			factiontemplate_s[id].enemyFaction1 = FactionTemplateDBC.getRecord(i).getUInt(6);
			factiontemplate_s[id].enemyFaction2 = FactionTemplateDBC.getRecord(i).getUInt(7);
			factiontemplate_s[id].enemyFaction3 = FactionTemplateDBC.getRecord(i).getUInt(8);
			factiontemplate_s[id].enemyFaction4 = FactionTemplateDBC.getRecord(i).getUInt(9);
			factiontemplate_s[id].friendFaction1 = FactionTemplateDBC.getRecord(i).getUInt(10);
			factiontemplate_s[id].friendFaction2 = FactionTemplateDBC.getRecord(i).getUInt(11);
			factiontemplate_s[id].friendFaction3 = FactionTemplateDBC.getRecord(i).getUInt(12);
			factiontemplate_s[id].friendFaction4 = FactionTemplateDBC.getRecord(i).getUInt(13);
		}
		for(factiontemplate_map::iterator iter = factiontemplate_s.begin(); iter!=factiontemplate_s.end(); iter++)
		{
			// Отношения с фракцией Horde (1801) - Орда
			if ((*iter).second.IsFriendlyTo(factiontemplate_s[1801]))
				(*iter).second.H = 1;
			else if ((*iter).second.IsHostileTo(factiontemplate_s[1801]))
				(*iter).second.H = -1;
			else
				(*iter).second.H = 0;
			// Отношения с фракцией Alliance (1802) - Альянс
			if ((*iter).second.IsFriendlyTo(factiontemplate_s[1802]))
				(*iter).second.A = 1;
			else if ((*iter).second.IsHostileTo(factiontemplate_s[1802]))
				(*iter).second.A = -1;
			else
				(*iter).second.A = 0;
		}
	} else {
		std::cout << "Error: FactionTemplate.dbc was not Loaded" << std::endl;
	}

	FactionDBC.Load("./dbc/Faction.dbc","niiiiixxxxiiiixxxxissssssssssssssssxxxxxxxxxxxxxxxxxx");
	if (FactionDBC.IsLoaded())
	{
		std::cout << "Info: Faction.dbc was sucessfully Loaded" << std::endl;
		for (i=0;i<FactionDBC.GetNumRows();i++)
		{
			id = FactionDBC.getRecord(i).getUInt(0);
			faction_s[id].reputationListID = FactionDBC.getRecord(i).getUInt(1);
			faction_s[id].team = FactionDBC.getRecord(i).getUInt(18);
			faction_s[id].name = FactionDBC.getRecord(i).getString(19);
			faction_s[id].description = FactionDBC.getRecord(i).getString(36);
			faction_s[id].side = 0;
			for (factiontemplate_map::iterator iter = factiontemplate_s.begin(); iter!=factiontemplate_s.end(); iter++)
				if ((*iter).second.factionID == id)
				{
					// Альянс
					if (((*iter).second.A == 1) and ((*iter).second.H == -1))
						faction_s[id].side = 1;
					// Орда
					if (((*iter).second.A == -1) and ((*iter).second.H == 1))
						faction_s[id].side = 2;
					break;
				}
		}
	} else {
		std::cout << "Error: Faction.dbc was not Loaded" << std::endl;
	}

	SpellItemEnchantmentDBC.Load("./dbc/SpellItemEnchantment.dbc","niiiiiixxxiiissssssssssssssssxiiii");
	if (SpellItemEnchantmentDBC.IsLoaded())
	{
		std::cout << "SpellItemEnchantment.dbc succesfully loaded" << std::endl;
		for (i=0;i<SpellItemEnchantmentDBC.GetNumRows();i++)
		{
			id = SpellItemEnchantmentDBC.getRecord(i).getUInt(0);
			itemenchantmet_s[id].text = SpellItemEnchantmentDBC.getRecord(i).getString(13);
		}
	} else {
		std::cout << "Error, while loading SpellItemEnchantment.dbc";
	}

	// Подгружаем таблицу спеллов
	SpellDurationDBC.Load("./dbc/SpellDuration.dbc","niii");
	if (SpellDurationDBC.IsLoaded())
	{
		std::cout << "SpellDuration.dbc succesfully loaded" << std::endl;
		for (i=0;i<SpellDurationDBC.GetNumRows();i++)
		{
			// Duration Base
			id = SpellDurationDBC.getRecord(i).getUInt(0);
			spelldurations[id].durationBase = SpellDurationDBC.getRecord(i).getUInt(1);
		}
	} else {
		std::cout << "Error, while loading SpellDuration.dbc";
	}

	SpellRangeDBC.Load("./dbc/SpellRange.dbc","nffxsssssssssssssssss");
	if (SpellRangeDBC.IsLoaded())
	{
		std::cout << "SpellRange.dbc succesfully loaded" << std::endl;
		for (i=0;i<SpellRangeDBC.GetNumRows();i++)
		{
			id = SpellRangeDBC.getRecord(i).getUInt(0);
			// Расстояния
			spellranges[id].rangeMin = SpellRangeDBC.getRecord(i).getFloat(1);
			spellranges[id].rangeMax = SpellRangeDBC.getRecord(i).getFloat(2);
			// Название
			spellranges[id].name = SpellRangeDBC.getRecord(i).getString(4);
		}
	} else {
		std::cout << "Error, while loading SpellRange.dbc";
	}

	SpellRadiusDBC.Load("./dbc/SpellRadius.dbc","nfxf");
	if (SpellRadiusDBC.IsLoaded())
	{
		std::cout << "SpellRadius.dbc succesfully loaded" << std::endl;
		for (i=0;i<SpellRangeDBC.GetNumRows();i++)
		{
			id = SpellRadiusDBC.getRecord(i).getUInt(0);
			spellradiuss[id].radiusBase = SpellRadiusDBC.getRecord(i).getFloat(1);
		}
	} else {
		std::cout << "Error, while loading SpellRadius.dbc";
	}

	// Подгружаем таблицу спеллов
	SpellDBC.Load("./dbc/Spell.dbc","niixiiiiiiixixiiiiixxiiiiiiiiiiiiiiiiiiifiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiffffffiiixxxiiiiiiiiiiiiiiifffiiiiiiiiiiiifffixiiissssssssssssssssxssssssssssssssssxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxiiiiiiiiixxfffxxxiii");
	if (SpellDBC.IsLoaded())
	{
		std::cout << "Spell.dbc succesfully loaded" << std::endl;
		for (i=0;i<SpellDBC.GetNumRows();i++)
		{
			// Номер спелла (id)
			id = SpellDBC.getRecord(i).getUInt(0);
			// Школа спелла
			spells[id].resistancesID = SpellDBC.getRecord(i).getUInt(1);
			// Тип диспела
			spells[id].dispeltypeID = SpellDBC.getRecord(i).getUInt(3);
			// Механика спелла
			spells[id].mechanicID =  SpellDBC.getRecord(i).getUInt(4);
			// Ссылка на SpellCastTimes
			spells[id].spellcasttimesID = SpellDBC.getRecord(i).getUInt(22);
			// Кулдаун спелла
			spells[id].cooldown = SpellDBC.getRecord(i).getUInt(23);
			// Channeled flag
			spells[id].ChannelInterruptFlags = SpellDBC.getRecord(i).getUInt(27);
			// ???
			spells[id].procChance = SpellDBC.getRecord(i).getUInt(29);
			// ???
			spells[id].procCharges = SpellDBC.getRecord(i).getUInt(30);
			// Уровень спелла
			spells[id].levelspell = SpellDBC.getRecord(i).getUInt(33);
			// DurationID
			spells[id].durationID = SpellDBC.getRecord(i).getUInt(34);
			// Затраты маны*10
			spells[id].manacost = SpellDBC.getRecord(i).getUInt(36);
			// RangeID
			spells[id].rangeID = SpellDBC.getRecord(i).getUInt(40);
			// Необходимые инструменты:
			spells[id].tool1 = SpellDBC.getRecord(i).getUInt(44);
			spells[id].tool2 = SpellDBC.getRecord(i).getUInt(45);
			// Список реагентов:
			spells[id].reagent1 = SpellDBC.getRecord(i).getUInt(46);
			spells[id].reagent2 = SpellDBC.getRecord(i).getUInt(47);
			spells[id].reagent3 = SpellDBC.getRecord(i).getUInt(48);
			spells[id].reagent4 = SpellDBC.getRecord(i).getUInt(49);
			spells[id].reagent5 = SpellDBC.getRecord(i).getUInt(50);
			spells[id].reagent6 = SpellDBC.getRecord(i).getUInt(51);
			spells[id].reagent7 = SpellDBC.getRecord(i).getUInt(52);
			spells[id].reagent8 = SpellDBC.getRecord(i).getUInt(53);
			// Необходимое количество реагентов
			spells[id].reagentcount1 = SpellDBC.getRecord(i).getUInt(54);
			spells[id].reagentcount2 = SpellDBC.getRecord(i).getUInt(55);
			spells[id].reagentcount3 = SpellDBC.getRecord(i).getUInt(56);
			spells[id].reagentcount4 = SpellDBC.getRecord(i).getUInt(57);
			spells[id].reagentcount5 = SpellDBC.getRecord(i).getUInt(58);
			spells[id].reagentcount6 = SpellDBC.getRecord(i).getUInt(59);
			spells[id].reagentcount7 = SpellDBC.getRecord(i).getUInt(60);
			spells[id].reagentcount8 = SpellDBC.getRecord(i).getUInt(61);
			// Тип эффекта
			spells[id].effect1id = SpellDBC.getRecord(i).getUInt(65);
			spells[id].effect2id = SpellDBC.getRecord(i).getUInt(66);
			spells[id].effect3id = SpellDBC.getRecord(i).getUInt(67);
			// Наносимый Спеллом случайный урон 1, 2, 3
			spells[id].effect1DieSides = SpellDBC.getRecord(i).getUInt(68);
			spells[id].effect2DieSides = SpellDBC.getRecord(i).getUInt(69);
			spells[id].effect3DieSides = SpellDBC.getRecord(i).getUInt(70);
			// Наносимый Спеллом базовый урон 1, 2, 3
			spells[id].effect1BasePoints = SpellDBC.getRecord(i).getUInt(80);
			spells[id].effect2BasePoints = SpellDBC.getRecord(i).getUInt(81);
			spells[id].effect3BasePoints = SpellDBC.getRecord(i).getUInt(82);
			// Радиус эффектов
			spells[id].effect1radius = SpellDBC.getRecord(i).getUInt(92);
			spells[id].effect2radius = SpellDBC.getRecord(i).getUInt(93);
			spells[id].effect3radius = SpellDBC.getRecord(i).getUInt(94);
			// Номера аур эффектов
			spells[id].effect1Aura = SpellDBC.getRecord(i).getUInt(95);
			spells[id].effect2Aura = SpellDBC.getRecord(i).getUInt(96);
			spells[id].effect3Aura = SpellDBC.getRecord(i).getUInt(97);
			// $o1, $o2, $o3
			spells[id].effect1Amplitude = SpellDBC.getRecord(i).getUInt(98);
			spells[id].effect2Amplitude = SpellDBC.getRecord(i).getUInt(99);
			spells[id].effect3Amplitude = SpellDBC.getRecord(i).getUInt(100);
			// ???
			spells[id].effect_1_proc_value = SpellDBC.getRecord(i).getFloat(101);
			spells[id].effect_2_proc_value = SpellDBC.getRecord(i).getFloat(102);
			spells[id].effect_3_proc_value = SpellDBC.getRecord(i).getFloat(103);
			// $x1, $x2, $x3
			spells[id].effect1ChainTarget = SpellDBC.getRecord(i).getUInt(104);
			spells[id].effect2ChainTarget = SpellDBC.getRecord(i).getUInt(105);
			spells[id].effect3ChainTarget = SpellDBC.getRecord(i).getUInt(106);
			// Получается вещь
			spells[id].effect1itemtype = SpellDBC.getRecord(i).getUInt(107);
			spells[id].effect2itemtype = SpellDBC.getRecord(i).getUInt(108);
			spells[id].effect3itemtype = SpellDBC.getRecord(i).getUInt(109);
			// $x1, $x2, $x3
			spells[id].effect1MiscValue = SpellDBC.getRecord(i).getUInt(110);
			spells[id].effect2MiscValue = SpellDBC.getRecord(i).getUInt(111);
			spells[id].effect3MiscValue = SpellDBC.getRecord(i).getUInt(112);
			// Вызываемый спеллом спелл :)
			spells[id].effect1triggerspell = SpellDBC.getRecord(i).getUInt(113);
			spells[id].effect2triggerspell = SpellDBC.getRecord(i).getUInt(114);
			spells[id].effect3triggerspell = SpellDBC.getRecord(i).getUInt(115);
			// ???
			spells[id].effect_1_proc_chance = SpellDBC.getRecord(i).getFloat(116);
			spells[id].effect_2_proc_chance = SpellDBC.getRecord(i).getFloat(117);
			spells[id].effect_3_proc_chance = SpellDBC.getRecord(i).getFloat(118);
			// Иконка спелла
			spells[id].spellicon = SpellDBC.getRecord(i).getUInt(121);
			// Имя спелла
			spells[id].name = SpellDBC.getRecord(i).getString(124);
			// Ранг спелла
			spells[id].rank = SpellDBC.getRecord(i).getString(141);
			// Неформатировання подсказка
			spells[id].tooltip = SpellDBC.getRecord(i).getString(158);
			// Неформатированная подсказка для баффа
			spells[id].buff = SpellDBC.getRecord(i).getString(175);
			// Затраты маны в %
			spells[id].manacostpercent = SpellDBC.getRecord(i).getUInt(192);
			// ?Уровень поражаемой цели?
			spells[id].affected_target_level = SpellDBC.getRecord(i).getUInt(195);
			// Количество поражаемых целей
			spells[id].spellTargets = SpellDBC.getRecord(i).getUInt(199);
			// ???
			spells[id].dmg_multiplier1 = SpellDBC.getRecord(i).getFloat(203);
			spells[id].dmg_multiplier2 = SpellDBC.getRecord(i).getFloat(204);
			spells[id].dmg_multiplier3 = SpellDBC.getRecord(i).getFloat(205);
		}
	} else {
		std::cout << "Error, while loading Spell.dbc";
	}

	SkillLineDBC.Load("./dbc/SkillLine.dbc","nixssssssssssssssssxxxxxxxxxxxxxxxxxxi");
	if (SkillLineDBC.IsLoaded())
	{
		std::cout << "SkillLine.dbc succesfully loaded" << std::endl;
		for (i=0;i<SkillLineDBC.GetNumRows();i++)
		{
			id = SkillLineDBC.getRecord(i).getUInt(0);
			skill_s[id].categoryID = SkillLineDBC.getRecord(i).getUInt(1);
			skill_s[id].name = SkillLineDBC.getRecord(i).getString(3);
			skill_s[id].description = SkillLineDBC.getRecord(i).getString(3);
		}
		dump(skill_s);
	} else {
		std::cout << "Error, while loading SkillLine.dbc";
	}

	unsigned int aid = 0;

	MapDBC.Load("./dbc/Map.dbc","nxixssssssssssssssssxxxxxxxixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxixxxxx");
	if (MapDBC.IsLoaded())
	{
		std::cout << "Info: Map.dbc was sucessfully Loaded" << std::endl;
		for (i=0;i<MapDBC.GetNumRows();i++)
		{
			id = MapDBC.getRecord(i).getUInt(0);
			map_s[id].type = MapDBC.getRecord(i).getUInt(2);
			map_s[id].name = MapDBC.getRecord(i).getString(4);
			map_s[id].areatableID = MapDBC.getRecord(i).getUInt(27);
			if (map_s[id].type>0)
			{
				aowow_zones_s[aid].mapID = id;
				aowow_zones_s[aid].areatableID = map_s[id].areatableID;
				aowow_zones_s[aid].name = map_s[id].name;
				aowow_zones_s[aid].x_min = 0; aowow_zones_s[aid].y_min = 0;
				aowow_zones_s[aid].x_max = 0; aowow_zones_s[aid].y_max = 0;
				aid++;
				//std::cout << map_s[id].name << ':' << map_s[id].areatableID << ':' << id << std::endl;
			}
		}
	} else {
		std::cout << "Error: Map.dbc was not Loaded" << std::endl;
	}

	AreaTableDBC.Load("./dbc/AreaTable.dbc","iiinixxxxxissssssssssssssssxixxxxxx");
	if (AreaTableDBC.IsLoaded())
	{
		std::cout << "Info: AreaTable.dbc was sucessfully Loaded" << std::endl;
		for (i=0;i<AreaTableDBC.GetNumRows();i++)
		{
			id = AreaTableDBC.getRecord(i).getUInt(0);
			areatable_s[id].name = AreaTableDBC.getRecord(i).getString(11);
			//std::cout << areatable_s[id].name << id << std::endl;
			//std::cout << id << ": " << areatable_s[id].name << std::endl;
		}
	} else {
		std::cout << "Error: AreaTable.dbc was not Loaded" << std::endl;
	}

	WorldMapAreaDBC.Load("./dbc/WorldMapArea.dbc","xinxffffi");
	if (WorldMapAreaDBC.IsLoaded())
	{
		std::cout << "Info: WorldMapArea.dbc was sucessfully Loaded" << std::endl;
		for (i=0;i<WorldMapAreaDBC.GetNumRows();i++)
		{
			id = WorldMapAreaDBC.getRecord(i).getUInt(0);
			worldmaparea_s[id].mapID = WorldMapAreaDBC.getRecord(i).getUInt(1);
			worldmaparea_s[id].areatableID = WorldMapAreaDBC.getRecord(i).getUInt(2);
			worldmaparea_s[id].y0 = WorldMapAreaDBC.getRecord(i).getFloat(4);
			worldmaparea_s[id].y1 = WorldMapAreaDBC.getRecord(i).getFloat(5);
			worldmaparea_s[id].x0 = WorldMapAreaDBC.getRecord(i).getFloat(6);
			worldmaparea_s[id].x1 = WorldMapAreaDBC.getRecord(i).getFloat(7);
			if (areatable_s[worldmaparea_s[id].areatableID].name.length() > 0)
			{
				aowow_zones_s[aid].mapID = worldmaparea_s[id].mapID;
				aowow_zones_s[aid].areatableID = worldmaparea_s[id].areatableID;
				aowow_zones_s[aid].name = areatable_s[worldmaparea_s[id].areatableID].name;
				if (worldmaparea_s[id].x0>worldmaparea_s[id].x1)
				{
					aowow_zones_s[aid].x_max = worldmaparea_s[id].x0;
					aowow_zones_s[aid].x_min = worldmaparea_s[id].x1;
				} else {
					aowow_zones_s[aid].x_max = worldmaparea_s[id].x1;
					aowow_zones_s[aid].x_min = worldmaparea_s[id].x0;
				}

				if (worldmaparea_s[id].y0>worldmaparea_s[id].y1)
				{
					aowow_zones_s[aid].y_max = worldmaparea_s[id].y0;
					aowow_zones_s[aid].y_min = worldmaparea_s[id].y1;
				} else {
					aowow_zones_s[aid].y_max = worldmaparea_s[id].y1;
					aowow_zones_s[aid].y_min = worldmaparea_s[id].y0;
				}
				aid++;
			}
			//std::cout << id << ": " << worldmaparea_s[id].mapID << ':' << worldmaparea_s[id].areatableID << std::endl;
		}
	} else {
		std::cout << "Error: WorldMapArea.dbc was not Loaded" << std::endl;
	}

	dump(spelldispeltype_s);
	dump(spellmechanic_s);
	dump(resistances_s);
	dump(spellcasttimes_s);
	dump(spellicons_s);
	dump(lock_s);
	dump(icons_s);
	dump(faction_s);
	dump(factiontemplate_s);
	dump(itemset_s);
	dump(aowow_zones_s);
	dump(spells);
	dump(spelldurations);
	dump(spellranges);
	dump(spellradiuss);
	dump(itemenchantmet_s);
	dump(gemproperties_s);
	ofile.close();

	return 0;
}
