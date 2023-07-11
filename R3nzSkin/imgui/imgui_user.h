#pragma once
#include <string>

#include "imgui.h"
#include "imgui_internal.h"
#include "../Utils/Utils.hpp"

namespace ImGui
{
    bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* userData = nullptr) noexcept;
    void textUnformattedCentered(const char* text) noexcept;
    void rainbowText() noexcept;
    void hotkey(const char* label, KeyBind& key, float samelineOffset = 0.0f, const ImVec2& size = { 100.0f, 0.0f }) noexcept;
    void hoverInfo(const char* desc) noexcept;
};