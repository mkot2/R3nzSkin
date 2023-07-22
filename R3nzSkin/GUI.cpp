#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include "CheatManager.hpp"
#include "GUI.hpp"
#include "Memory.hpp"
#include "SkinDatabase.hpp"
#include "utils/Utils.hpp"
#include "utils/fnv_hash.hpp"
#include "imgui/imgui.h"

#include "Utils/obfuscate.h"

inline static void footer() noexcept
{
	// Clowning for xorstr
	static const auto buildText = std::vformat("Build time: {} - {}"_o, std::make_format_args(__DATE__, __TIME__));
	ImGui::Separator();
	ImGui::textUnformattedCentered(buildText.c_str());
	ImGui::textUnformattedCentered("Copyright (C) 2021-2023 R3nzTheCodeGOD"_o);
}

static void changeTurretSkin(const std::int32_t skinId, const std::int32_t team) noexcept
{
	if (skinId == -1)
		return;

	const auto turrets{ cheatManager.memory->turretList };
	const auto playerTeam{ cheatManager.memory->localPlayer->get_team() };

	for (auto i{ 0u }; i < turrets->length; ++i) {
		const auto turret{ turrets->list[i] };
		if (turret->get_team() == team) {
			if (playerTeam == team) {
				turret->get_character_data_stack()->base_skin.skin = skinId * 2;
				turret->get_character_data_stack()->update(true);
			} else {
				turret->get_character_data_stack()->base_skin.skin = skinId * 2 + 1;
				turret->get_character_data_stack()->update(true);
			}
		}
	}
}

void GUI::render() noexcept
{
	const auto player{ cheatManager.memory->localPlayer };
	const auto heroes{ cheatManager.memory->heroList };
	static const auto my_team{ player ? player->get_team() : 100 };
	static int gear{ player ? player->get_character_data_stack()->base_skin.gear : 0 };

	static const auto vector_getter_skin = [](void* vec, std::int32_t idx, const char** out_text) noexcept {
		const auto& vector{ *static_cast<std::vector<SkinDatabase::skin_info>*>(vec) };
		if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) return false;
		*out_text = idx == 0 ? "Default"_o : vector.at(idx - 1).skin_name.c_str();
		return true;
	};

	static const auto vector_getter_ward_skin = [](void* vec, std::int32_t idx, const char** out_text) noexcept {
		const auto& vector{ *static_cast<std::vector<std::pair<std::int32_t, const char*>>*>(vec) };
		if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) return false;
		*out_text = idx == 0 ? "Default"_o : vector.at(idx - 1).second;
		return true;
	};

	static auto vector_getter_gear = [](void* vec, std::int32_t idx, const char** out_text) noexcept {
		const auto& vector{ *static_cast<std::vector<const char*>*>(vec) };
		if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) return false;
		*out_text = vector[idx];
		return true;
	};

	static auto vector_getter_default = [](void* vec, std::int32_t idx, const char** out_text) noexcept {
		const auto& vector{ *static_cast<std::vector<const char*>*>(vec) };
		if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) return false;
		*out_text = idx == 0 ? "Default"_o : vector.at(idx - 1);
		return true;
	};

	ImGui::Begin("R3nzSkin"_o, nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize);
	{
		ImGui::rainbowText();
		if (ImGui::BeginTabBar("TabBar"_o, ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip)) {
			if (player) {
				if (ImGui::BeginTabItem("Local Player"_o)) {
					auto& values{ cheatManager.database->champions_skins[fnv::hash_runtime(player->get_character_data_stack()->base_skin.model.str)] };
					ImGui::TextUnformatted("Player Skins Settings:"_o);

					if (ImGui::Combo("Current Skin"_o, &cheatManager.config->current_combo_skin_index, vector_getter_skin, static_cast<void*>(&values), values.size() + 1))
						if (cheatManager.config->current_combo_skin_index > 0)
							player->change_skin(values[cheatManager.config->current_combo_skin_index - 1].model_name, values[cheatManager.config->current_combo_skin_index - 1].skin_id);
					
					const auto playerHash{ fnv::hash_runtime(player->get_character_data_stack()->base_skin.model.str) };
					if (const auto it{ std::find_if(cheatManager.database->specialSkins.begin(), cheatManager.database->specialSkins.end(),
						[&skin = player->get_character_data_stack()->base_skin.skin, &ph = playerHash](const SkinDatabase::specialSkin& x) noexcept -> bool
						{
							return x.champHash == ph && (x.skinIdStart <= skin && x.skinIdEnd >= skin);
						}) };
						it != cheatManager.database->specialSkins.end())
					{
						const auto stack{ player->get_character_data_stack() };
						gear = stack->base_skin.gear;

						if (ImGui::Combo("Current Gear"_o, &gear, vector_getter_gear, static_cast<void*>(&it->gears), it->gears.size())) {
							player->get_character_data_stack()->base_skin.gear = static_cast<std::int8_t>(gear);
							player->get_character_data_stack()->update(true);
						}
						ImGui::Separator();
					}

					if (ImGui::Combo("Current Ward Skin"_o, &cheatManager.config->current_combo_ward_index, vector_getter_ward_skin, static_cast<void*>(&cheatManager.database->wards_skins), cheatManager.database->wards_skins.size() + 1))
						cheatManager.config->current_ward_skin_index = cheatManager.config->current_combo_ward_index == 0 ? -1 : cheatManager.database->wards_skins.at(cheatManager.config->current_combo_ward_index - 1).first;
					footer();
					ImGui::EndTabItem();
				}
			}

			if (ImGui::BeginTabItem("Other Champs"_o)) {
				ImGui::TextUnformatted("Other Champs Skins Settings:"_o);
				std::int32_t last_team{ 0 };
				for (auto i{ 0u }; i < heroes->length; ++i) {
					const auto hero{ heroes->list[i] };

					if (hero == player)
						continue;

					const auto champion_name_hash{ fnv::hash_runtime(hero->get_character_data_stack()->base_skin.model.str) };
					if (champion_name_hash == FNV("PracticeTool_TargetDummy"))
						continue;

					const auto hero_team{ hero->get_team() };
					const auto is_enemy{ hero_team != my_team };

					if (last_team == 0 || hero_team != last_team) {
						if (last_team != 0)
							ImGui::Separator();
						if (is_enemy)
							ImGui::TextUnformatted(" Enemy champions"_o);
						else
							ImGui::TextUnformatted(" Ally champions"_o);
						last_team = hero_team;
					}

					auto& config_array{ is_enemy ? cheatManager.config->current_combo_enemy_skin_index : cheatManager.config->current_combo_ally_skin_index };
					const auto config_entry{ config_array.insert({ champion_name_hash, 0 }) };

					ImFormatString(str_buffer, sizeof(str_buffer), "%s (%s)##%X"_o, hero->get_name()->c_str(), hero->get_character_data_stack()->base_skin.model.str, reinterpret_cast<std::uintptr_t>(hero));

					auto& values{ cheatManager.database->champions_skins[champion_name_hash] };
					if (ImGui::Combo(str_buffer, &config_entry.first->second, vector_getter_skin, static_cast<void*>(&values), values.size() + 1))
						if (config_entry.first->second > 0)
							hero->change_skin(values[config_entry.first->second - 1].model_name, values[config_entry.first->second - 1].skin_id);
				}
				footer();
				ImGui::EndTabItem();
			}


			if (ImGui::BeginTabItem("Global Skins"_o)) {
				ImGui::TextUnformatted("Global Skins Settings:"_o);
				if (ImGui::Combo("Minion Skins:"_o, &cheatManager.config->current_combo_minion_index, vector_getter_default, static_cast<void*>(&cheatManager.database->minions_skins), cheatManager.database->minions_skins.size() + 1))
					cheatManager.config->current_minion_skin_index = cheatManager.config->current_combo_minion_index - 1;
				ImGui::Separator();
				if (ImGui::Combo("Order Turret Skins:"_o, &cheatManager.config->current_combo_order_turret_index, vector_getter_default, static_cast<void*>(&cheatManager.database->turret_skins), cheatManager.database->turret_skins.size() + 1))
					changeTurretSkin(cheatManager.config->current_combo_order_turret_index - 1, 100);
				if (ImGui::Combo("Chaos Turret Skins:"_o, &cheatManager.config->current_combo_chaos_turret_index, vector_getter_default, static_cast<void*>(&cheatManager.database->turret_skins), cheatManager.database->turret_skins.size() + 1))
					changeTurretSkin(cheatManager.config->current_combo_chaos_turret_index - 1, 200);
				ImGui::Separator();
				ImGui::TextUnformatted("Jungle Mobs Skins Settings:"_o);
				for (auto& it : cheatManager.database->jungle_mobs_skins) {
					ImFormatString(str_buffer, sizeof(str_buffer), "Current %s skin"_o, it.name);
					const auto config_entry{ cheatManager.config->current_combo_jungle_mob_skin_index.insert({ it.name_hashes.front(), 0 }) };
					if (ImGui::Combo(str_buffer, &config_entry.first->second, vector_getter_default, static_cast<void*>(&it.skins), it.skins.size() + 1))
						for (const auto& hash : it.name_hashes)
							cheatManager.config->current_combo_jungle_mob_skin_index[hash] = config_entry.first->second;
				}
				footer();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Logger"_o)) {
				cheatManager.logger->draw();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Extras"_o)) {
				ImGui::hotkey("Menu Key"_o, cheatManager.config->menuKey);
				ImGui::Checkbox("Rainbow Text"_o, &cheatManager.config->rainbowText);
				ImGui::Checkbox("Quick Skin Change"_o, &cheatManager.config->quickSkinChange);
				ImGui::hoverInfo("It allows you to change skin without opening the menu with the key you assign from the keyboard."_o);

				if (cheatManager.config->quickSkinChange) {
					ImGui::Separator();
					ImGui::hotkey("Previous Skin Key"_o, cheatManager.config->previousSkinKey);
					ImGui::hotkey("Next Skin Key"_o, cheatManager.config->nextSkinKey);
					ImGui::Separator();
				}

				if (player)
					ImGui::InputText("Change Nick"_o, player->get_name());

				if (ImGui::Button("No skins except local player"_o)) {
					for (auto& enemy : cheatManager.config->current_combo_enemy_skin_index)
						enemy.second = 1;

					for (auto& ally : cheatManager.config->current_combo_ally_skin_index)
						ally.second = 1;

					for (auto i{ 0u }; i < heroes->length; ++i) {
						const auto hero{ heroes->list[i] };
						if (hero != player)
							hero->change_skin(hero->get_character_data_stack()->base_skin.model.str, 0);
					}
				} ImGui::hoverInfo("Sets the skins of all champions except the local player to the default skin."_o);

				if (ImGui::Button("Random Skins"_o)) {
					for (auto i{ 0u }; i < heroes->length; ++i) {
						const auto hero{ heroes->list[i] };
						const auto championHash{ fnv::hash_runtime(hero->get_character_data_stack()->base_skin.model.str) };
						
						if (championHash == FNV("PracticeTool_TargetDummy"))
							continue;
						
						const auto skinCount{ cheatManager.database->champions_skins[championHash].size() };
						auto& skinDatabase{ cheatManager.database->champions_skins[championHash] };
						auto& config{ (hero->get_team() != my_team) ? cheatManager.config->current_combo_enemy_skin_index : cheatManager.config->current_combo_ally_skin_index };

						if (hero == player) {
							cheatManager.config->current_combo_skin_index = random(1ull, skinCount);
							hero->change_skin(skinDatabase[cheatManager.config->current_combo_skin_index - 1].model_name, skinDatabase[cheatManager.config->current_combo_skin_index - 1].skin_id);
						} else {
							auto& data{ config[championHash] };
							data = random(1ull, skinCount);
							hero->change_skin(skinDatabase[data - 1].model_name, skinDatabase[data - 1].skin_id);
						}
					}
				} ImGui::hoverInfo("Randomly changes the skin of all champions."_o);

				ImGui::SliderFloat("Font Scale"_o, &cheatManager.config->fontScale, 1.0f, 2.0f, "%.3f"_o);
				if (ImGui::GetIO().FontGlobalScale != cheatManager.config->fontScale) {
					ImGui::GetIO().FontGlobalScale = cheatManager.config->fontScale;
				} ImGui::hoverInfo("Changes the menu font scale."_o);
				
				if (ImGui::Button("Force Close"_o))
					cheatManager.hooks->uninstall();
				ImGui::hoverInfo("You will be returned to the reconnect screen."_o);
				ImGui::Text("FPS: %.0f FPS"_o, ImGui::GetIO().Framerate);
				footer();
				ImGui::EndTabItem();
			}
		}
	}
	ImGui::End();
}
