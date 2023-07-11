#include <algorithm>
#include <cstdint>
#include <map>
#include <ranges>
#include <string>
#include <utility>

#include "CheatManager.hpp"
#include "SkinDatabase.hpp"
#include "utils/fnv_hash.hpp"

#include "utils/xorstr.hpp"

void SkinDatabase::load() noexcept
{
	for (const auto& champion : cheatManager.memory->championManager->champions) {
		std::vector<std::int32_t> skins_ids;
		
		for (auto i{ 0u }; i < champion->skins.size; ++i)
			skins_ids.push_back(champion->skins.list[i].skin_id);
		
		std::ranges::sort(skins_ids);

		std::map<std::string, std::int32_t> temp_skin_list;
		for (const auto& i : skins_ids) {
			const auto skin_display_name{ std::string(xorstr_("game_character_skin_displayname_")) + champion->champion_name.str + xorstr_("_") + std::to_string(i) };
			auto skin_display_name_translated{ i > 0 ? std::string(cheatManager.memory->translateString(skin_display_name.c_str())) : std::string(champion->champion_name.str) };

			if (skin_display_name_translated == skin_display_name)
				continue;

			if (const auto it{ temp_skin_list.find(skin_display_name_translated) }; it == temp_skin_list.end()) {
				temp_skin_list[skin_display_name_translated] = 1;
			} else {
				skin_display_name_translated.append(xorstr_(" Chroma ") + std::to_string(it->second));
				it->second = it->second + 1;
			}

			const auto champ_name{ fnv::hash_runtime(champion->champion_name.str) };
			this->champions_skins[champ_name].push_back({ champion->champion_name.str, skin_display_name_translated, i });

			if (i == 7 && champ_name == FNV("Lux")) {
				this->champions_skins[champ_name].push_back({ xorstr_("LuxAir"), xorstr_("Elementalist Air Lux"), i });
				this->champions_skins[champ_name].push_back({ xorstr_("LuxDark"), xorstr_("Elementalist Dark Lux"), i });
				this->champions_skins[champ_name].push_back({ xorstr_("LuxFire"), xorstr_("Elementalist Fire Lux"), i });
				this->champions_skins[champ_name].push_back({ xorstr_("LuxIce"), xorstr_("Elementalist Ice Lux"), i });
				this->champions_skins[champ_name].push_back({ xorstr_("LuxMagma"), xorstr_("Elementalist Magma Lux"), i });
				this->champions_skins[champ_name].push_back({ xorstr_("LuxMystic"), xorstr_("Elementalist Mystic Lux"), i });
				this->champions_skins[champ_name].push_back({ xorstr_("LuxNature"), xorstr_("Elementalist Nature Lux"), i });
				this->champions_skins[champ_name].push_back({ xorstr_("LuxStorm"), xorstr_("Elementalist Storm Lux"), i });
				this->champions_skins[champ_name].push_back({ xorstr_("LuxWater"), xorstr_("Elementalist Water Lux"), i });
			} else if (i == 6 && champ_name == FNV("Sona")) {
				this->champions_skins[champ_name].push_back({ xorstr_("SonaDJGenre02"), xorstr_("DJ Sona 2"), i });
				this->champions_skins[champ_name].push_back({ xorstr_("SonaDJGenre03"), xorstr_("DJ Sona 3"), i });
			}
		}
	}

	for (auto ward_skin_id{ 1u };; ++ward_skin_id) {
		const auto ward_display_name{ xorstr_("game_character_skin_displayname_SightWard_") + std::to_string(ward_skin_id) };
		const auto ward_display_name_translated{ cheatManager.memory->translateString(ward_display_name.c_str()) };
		
		if (ward_display_name == ward_display_name_translated)
			break;

		this->wards_skins.push_back({ ward_skin_id, ward_display_name_translated });
	}
}
