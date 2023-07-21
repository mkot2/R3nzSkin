#pragma warning(disable : 6387 4715)

#include <Windows.h>
#include <array>
#include <clocale>
#include <chrono>
#include <cstdint>
#include <thread>

#include "CheatManager.hpp"

#include "Config.hpp"
#include "GUI.hpp"
#include "Hooks.hpp"
#include "Memory.hpp"

#include "SDK/GameState.hpp"

#include "Utils/lazy_importer.hpp"
#include "Utils/obfuscate.h"

bool WINAPI HideThread(const HANDLE hThread) noexcept
{
	__try {
		using FnSetInformationThread = NTSTATUS(NTAPI*)(HANDLE ThreadHandle, UINT ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength);
		const auto NtSetInformationThread{ reinterpret_cast<FnSetInformationThread>(LI_FN(GetProcAddress)(LI_FN(GetModuleHandle)("ntdll.dll"_o), "NtSetInformationThread"_o)) };

		if (!NtSetInformationThread)
			return false;

		if (const auto status{ NtSetInformationThread(hThread, 0x11u, nullptr, 0ul) }; status == 0x00000000)
			return true;
	} __except (TRUE) {
		return false;
	}
}

__declspec(safebuffers) static void WINAPI DllAttach([[maybe_unused]] LPVOID lp) noexcept
{
	using namespace std::chrono_literals;

	cheatManager.start();
	if (HideThread(LI_FN(GetCurrentThread)()))
		cheatManager.logger->addLog("Thread Hided!\n"_o);

	cheatManager.memory->Search(true);
	while (true) {
		std::this_thread::sleep_for(1s);
		
		if (!cheatManager.memory->client)
			cheatManager.memory->Search(true);
		else if (cheatManager.memory->client->game_state == GGameState_s::Running)
			break;
	}
	cheatManager.logger->addLog("GameClient found!\n"_o);
	
	std::this_thread::sleep_for(500ms);
	cheatManager.memory->Search(false);
	cheatManager.logger->addLog("All offsets found!\n"_o);
	std::this_thread::sleep_for(500ms);
	
	cheatManager.config->init();
	cheatManager.config->load();
	cheatManager.logger->addLog("CFG loaded!\n"_o);
	
	cheatManager.hooks->install();
		
	while (cheatManager.cheatState)
		std::this_thread::sleep_for(250ms);

	LI_FN(ExitProcess)(0u);
}

__declspec(safebuffers) BOOL APIENTRY DllMain(const HMODULE hModule, const DWORD reason, [[maybe_unused]] LPVOID reserved)
{
	LI_FN(DisableThreadLibraryCalls)(hModule);

	if (reason != DLL_PROCESS_ATTACH)
		return FALSE;

	HideThread(hModule);
	std::setlocale(LC_ALL, ".utf8"_o);

	LI_FN(_beginthreadex)(nullptr, 0u, reinterpret_cast<_beginthreadex_proc_type>(DllAttach), nullptr, 0u, nullptr);
	LI_FN(CloseHandle)(hModule);
	return TRUE;
}
