#include <fstream>
#include <string>
#include <system_error>

#include <Windows.h>
#include <ShlObj.h>

#include "Json/json.hpp"

#include "CheatManager.hpp"
#include "Memory.hpp"
#include "utils/Utils.hpp"

#include "utils/lazy_importer.hpp"
#include "utils/obfuscate.h"

void Config::init() noexcept
{
	if (PWSTR pathToDocuments; SUCCEEDED(LI_FN(SHGetKnownFolderPath)(FOLDERID_Documents, 0, nullptr, &pathToDocuments))) {
		this->path = pathToDocuments;
		CoTaskMemFree(pathToDocuments);
	}

	this->path /= "R3nzSkin"_o;
}

void Config::save() noexcept
{
	const auto player{ cheatManager.memory->localPlayer };
	std::error_code ec;
	std::filesystem::create_directory(this->path, ec);
	auto out{ std::ofstream(this->path / "R3nzSkin64"_o)};

	if (!out.good())
		return;

	if (player)
		config_json[std::string(player->get_character_data_stack()->base_skin.model.str) + ".current_combo_skin_index"_o] = this->current_combo_skin_index;

	config_json["menuKey"_o] = this->menuKey.toString();
	config_json["nextSkinKey"_o] = this->nextSkinKey.toString();
	config_json["previousSkinKey"_o] = this->previousSkinKey.toString();
	config_json["rainbowText"_o] = this->rainbowText;
	config_json["quickSkinChange"_o] = this->quickSkinChange;
	config_json["fontScale"_o] = this->fontScale;
	config_json["current_combo_ward_index"_o] = this->current_combo_ward_index;
	config_json["current_ward_skin_index"_o] = this->current_ward_skin_index;
	config_json["current_combo_minion_index"_o] = this->current_combo_minion_index;
	config_json["current_minion_skin_index"_o] = this->current_minion_skin_index;

	for (const auto& [fst, snd] : this->current_combo_ally_skin_index)
		config_json["current_combo_ally_skin_index"_o][std::to_string(fst)] = snd;

	for (const auto& [fst, snd] : this->current_combo_enemy_skin_index)
		config_json["current_combo_enemy_skin_index"_o][std::to_string(fst)] = snd;

	for (const auto& [fst, snd] : this->current_combo_jungle_mob_skin_index)
		config_json["current_combo_jungle_mob_skin_index"_o][std::to_string(fst)] = snd;

	out << config_json.dump();
	out.close();
}

void Config::load() noexcept
{
	const auto player{ cheatManager.memory->localPlayer };
	auto in{ std::ifstream(this->path / "R3nzSkin64"_o) };

	if (!in.good())
		return;

	if (json j{ json::parse(in, nullptr, false, true) }; j.is_discarded())
		return;
	else
		config_json = j;

	if (player)
		this->current_combo_skin_index = config_json.value(std::string(player->get_character_data_stack()->base_skin.model.str) + ".current_combo_skin_index"_o, 0);

	this->menuKey = KeyBind(config_json.value("menuKey"_o, "INSERT"_o).c_str());
	this->nextSkinKey = KeyBind(config_json.value("nextSkinKey"_o, "PAGE_UP"_o).c_str());
	this->previousSkinKey = KeyBind(config_json.value("previousSkinKey"_o, "PAGE_DOWN"_o).c_str());
	this->rainbowText = config_json.value("rainbowText"_o, false);
	this->quickSkinChange = config_json.value("quickSkinChange"_o, false);
	this->fontScale = config_json.value("fontScale"_o, 1.0f);
	this->current_combo_ward_index = config_json.value("current_combo_ward_index"_o, 0);
	this->current_ward_skin_index = config_json.value("current_ward_skin_index"_o, -1);
	this->current_combo_minion_index = config_json.value("current_combo_minion_index"_o, 0);	// we probably dont need 2 vars for the almost same thing
	this->current_minion_skin_index = config_json.value("current_minion_skin_index"_o, -1);

	const auto ally_skins{ config_json.find("current_combo_ally_skin_index"_o) };
	if (ally_skins != config_json.end())
		for (const auto& it : ally_skins.value().items())
			this->current_combo_ally_skin_index[std::stoull(it.key())] = it.value().get<std::int32_t>();

	const auto enemy_skins{ config_json.find("current_combo_enemy_skin_index"_o) };
	if (enemy_skins != config_json.end())
		for (const auto& it : enemy_skins.value().items())
			this->current_combo_enemy_skin_index[std::stoull(it.key())] = it.value().get<std::int32_t>();

	const auto jungle_mobs_skins{ config_json.find("current_combo_jungle_mob_skin_index"_o) };
	if (jungle_mobs_skins != config_json.end())
		for (const auto& it : jungle_mobs_skins.value().items())
			this->current_combo_jungle_mob_skin_index[std::stoull(it.key())] = it.value().get<std::int32_t>();

	in.close();
}

void Config::reset() noexcept
{
	this->menuKey = KeyBind(KeyBind::INSERT);
	this->nextSkinKey = KeyBind(KeyBind::PAGE_UP);
	this->previousSkinKey = KeyBind(KeyBind::PAGE_DOWN);
	this->rainbowText = false;
	this->quickSkinChange = false;
	this->fontScale = 1.0f;
	this->current_combo_skin_index = 0;
	this->current_combo_ward_index = 0;
	this->current_ward_skin_index = -1;
	this->current_combo_minion_index = 0;
	this->current_minion_skin_index = -1;	
	this->current_combo_order_turret_index = 0;
	this->current_combo_chaos_turret_index = 0;
	this->current_combo_ally_skin_index.clear();
	this->current_combo_enemy_skin_index.clear();
	this->current_combo_jungle_mob_skin_index.clear();
}
