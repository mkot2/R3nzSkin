#pragma once

#include <cstdint>
#include <map>
#include <utility>
#include <vector>
#include <compare>

#include "Utils/obfuscate.h"
#include "utils/fnv_hash.hpp"
 
class SkinDatabase {
public:
	class skin_info {
	public:
		const char* model_name;
		std::string skin_name;
		std::int32_t skin_id;
	};

	class jungle_mob_skin_info {
	public:
		const char* name;
		std::vector<std::uint64_t> name_hashes;
		std::vector<const char*> skins;
	};

	class specialSkin {
	public:
		std::uint64_t champHash;
		std::int32_t skinIdStart;
		std::int32_t skinIdEnd;
		std::vector<const char*> gears;
	};

	void load() noexcept;

	std::map<std::uint64_t, std::vector<skin_info>> champions_skins;
	std::vector<std::pair<std::uint32_t, const char*>> wards_skins;

	std::vector<const char*> minions_skins {
		"Minion"_o, "Summer Minion"_o,
		"Project Minion"_o, "Snowdown Minion"_o,
		"Draven Minion"_o, "Star Guardian Minion"_o,
		"Arcade Minion"_o, "Snowdown 2 Minion"_o,
		"Odyssey Minion"_o, "Mouse Minion"_o, "Arcane Minion"_o
	};

	std::vector<const char*> turret_skins {
		"Default Order Turret"_o, "Default Chaos Turret"_o,
		"Snow Order Turret"_o, "Snow Chaos Turret"_o,
		"Twisted Treeline Order Turret"_o, "Twisted Treeline Chaos Turret"_o,
		"URF Order Turret"_o, "URF Chaos Turret"_o,
		"Arcade Turret"_o,
		"Temple of Lily and Lotus Turret"_o,
		"Arcane Order Turret"_o, "Arcane Chaos Turret"_o,
		"Butcher's Bridge Order Turret"_o, "Butcher's Bridge Chaos Turret"_o,
		"Howling Abyss Order Turret"_o, "Howling Abyss Chaos Turret"_o
	};

	std::vector<jungle_mob_skin_info> jungle_mobs_skins {
		{
			"Baron"_o,
			{ FNV("SRU_Baron") },
			{ "Baron"_o, "Snowdown Baron"_o, "Championship Baron"_o, "Lunar Revel Baron"_o, "MSI Baron"_o, "Odyssey Baron"_o, "Championship Birthday Baron"_o, "Ruined King Baron"_o }
		},
		{
			"Blue"_o,
			{ FNV("SRU_Blue") },
			{ "Blue"_o, "Dark Blue"_o, "Pool Party Blue"_o, "Ruined King Blue"_o }
		},
		{
			"Red"_o,
			{ FNV("SRU_Red") },
			{ "Red"_o, "Pool Party Red"_o, "Ruined King Red"_o }
		},
		{
			"Scuttle"_o,
			{ FNV("Sru_Crab") },
			{ "Scuttle"_o, "Halloween Light Scuttle"_o, "Halloween Dark Scuttle"_o, "Ruined King Scuttle"_o }
		},
		{
			"Krug"_o,
			{ FNV("SRU_Krug"), FNV("SRU_KrugMini"), FNV("SRU_KrugMiniMini") },
			{ "Krug"_o, "Dark Krug"_o }
		},
		{
			"Razorbeak"_o,
			{ FNV("SRU_Razorbeak"), FNV("SRU_RazorbeakMini") },
			{ "Razorbeak"_o, "Chicken Razorbeak"_o }
		}
	};

	std::vector<specialSkin> specialSkins {
		{ FNV("Katarina"), 29, 36, { "Dagger 1"_o, "Dagger 2"_o, "Dagger 3"_o, "Dagger 4"_o, "Dagger 5"_o, "Dagger 6"_o }},
		{ FNV("Renekton"), 26, 32, { "Head off"_o, "Head on"_o, "Fins"_o, "Ultimate"_o } },
		{ FNV("MissFortune"), 16, 16, { "Scarlet fair"_o, "Zero hour"_o, "Royal arms"_o, "Starswarm"_o } },
		{ FNV("Ezreal"), 5, 5, { "Level 1"_o, "Level 2"_o, "Level 3"_o } }
	};
};
