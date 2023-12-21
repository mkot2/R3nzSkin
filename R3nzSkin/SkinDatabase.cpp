#include <algorithm>
#include <cstdint>
#include <map>
#include <ranges>
#include <string>
#include <utility>

#include "CheatManager.hpp"
#include "SkinDatabase.hpp"
#include "utils/fnv_hash.hpp"

#include "utils/obfuscate.h"

void SkinDatabase::load() noexcept
{
	for (const auto& champion : cheatManager.memory->championManager->champions) {
		std::vector<std::int32_t> skins_ids;
		
		for (auto i{ 0 }; i < champion->skins.size; ++i)
			skins_ids.push_back(champion->skins.list[i].skin_id);
		
		std::ranges::sort(skins_ids);

		std::map<std::string, std::int32_t> temp_skin_list;
		for (const auto& i : skins_ids) {
			const auto skin_display_name{ std::string("game_character_skin_displayname_"_o) + champion->champion_name.str + "_"_o + std::to_string(i) };
			auto skin_display_name_translated{ i > 0 ? std::string(cheatManager.memory->translateString(skin_display_name.c_str())) : std::string(champion->champion_name.str) };

			if (skin_display_name_translated == skin_display_name)
				continue;

			if (const auto it{ temp_skin_list.find(skin_display_name_translated) }; it == temp_skin_list.end()) {
				temp_skin_list[skin_display_name_translated] = 1;
			} else {
				skin_display_name_translated.append(" Chroma "_o + std::to_string(it->second));
				it->second = it->second + 1;
			}

			const auto champ_name{ fnv::hash_runtime(champion->champion_name.str) };
			this->champions_skins[champ_name].push_back({ champion->champion_name.str, skin_display_name_translated, i });

			if (i == 7 && champ_name == FNV("Lux")) {
				this->champions_skins[champ_name].push_back({ "LuxAir"_o, "Elementalist Air Lux"_o, i });
				this->champions_skins[champ_name].push_back({ "LuxDark"_o, "Elementalist Dark Lux"_o, i });
				this->champions_skins[champ_name].push_back({ "LuxFire"_o, "Elementalist Fire Lux"_o, i });
				this->champions_skins[champ_name].push_back({ "LuxIce"_o, "Elementalist Ice Lux"_o, i });
				this->champions_skins[champ_name].push_back({ "LuxMagma"_o, "Elementalist Magma Lux"_o, i });
				this->champions_skins[champ_name].push_back({ "LuxMystic"_o, "Elementalist Mystic Lux"_o, i });
				this->champions_skins[champ_name].push_back({ "LuxNature"_o, "Elementalist Nature Lux"_o, i });
				this->champions_skins[champ_name].push_back({ "LuxStorm"_o, "Elementalist Storm Lux"_o, i });
				this->champions_skins[champ_name].push_back({ "LuxWater"_o, "Elementalist Water Lux"_o, i });
			} else if (i == 6 && champ_name == FNV("Sona")) {
				this->champions_skins[champ_name].push_back({ "SonaDJGenre02"_o, "DJ Sona 2"_o, i });
				this->champions_skins[champ_name].push_back({ "SonaDJGenre03"_o, "DJ Sona 3"_o, i });
			}
		}
	}

	for (auto ward_skin_id{ 1u };; ++ward_skin_id) {
		const auto ward_display_name{ "game_character_skin_displayname_SightWard_"_o + std::to_string(ward_skin_id) };
		const auto ward_display_name_translated{ cheatManager.memory->translateString(ward_display_name.c_str()) };
		
		if (ward_display_name == ward_display_name_translated)
			break;

		this->wards_skins.emplace_back(ward_skin_id, ward_display_name_translated);
	}

	/* sort wards */
	std::ranges::sort(this->wards_skins, [](const auto& a, const auto& b) {
		return std::strong_ordering::less == std::strcmp(a.second, b.second) <=> 0;
	});
}
