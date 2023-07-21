#include "imgui_user.h"
#include "../CheatManager.hpp"
#include "../Utils/obfuscate.h"

void ImGui::textUnformattedCentered(const char* text) noexcept
{
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) / 2.0f);
    ImGui::TextUnformatted(text);
}

void ImGui::hoverInfo(const char* desc) noexcept
{
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void ImGui::rainbowText() noexcept
{
    static float r{ 1.0f };
    static float g{ 0.f };
    static float b{ 0.f };

    if (cheatManager.config->rainbowText) {
        if (r == 1.f && g >= 0.f && b <= 0.f) {
            g += 0.005f;
            b = 0.f;
        }
        if (r <= 1.f && g >= 1.f && b == 0.f) {
            g = 1.f;
            r -= 0.005f;
        }
        if (r <= 0.f && g == 1.f && b >= 0.f) {
            r = 0.f;
            b += 0.005f;
        }
        if (r == 0.f && g <= 1.f && b >= 1.f) {
            b = 1.f;
            g -= 0.005f;
        }
        if (r >= 0.f && g <= 0.f && b == 1.f) {
            g = 0.f;
            r += 0.005f;
        }
        if (r >= 1.f && g >= 0.f && b <= 1.f) {
            r = 1.f;
            b -= 0.005f;
        }
        ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(r, g, b, 1.00f);
    }
    else {
        if (auto& clr{ ImGui::GetStyle().Colors[ImGuiCol_Text] }; clr.x != 0.92f && clr.y != 0.92f && clr.z != 0.92f)
            clr = ImVec4(0.92f, 0.92f, 0.92f, 0.92f);
    }
}

struct InputTextCallback_UserData {
    std::string* Str;
    ImGuiInputTextCallback ChainCallback;
    void* ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data) noexcept
{
    const auto user_data{ static_cast<InputTextCallback_UserData*>(data->UserData) };
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        const auto str{ user_data->Str };
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = const_cast<char*>(str->c_str());
    }
    else if (user_data->ChainCallback) {
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

bool ImGui::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* userData) noexcept
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;
    auto cbUserData{ InputTextCallback_UserData(str, callback, userData) };
    return InputText(label, const_cast<char*>(str->c_str()), str->capacity() + 1, flags, InputTextCallback, &cbUserData);
}

void ImGui::hotkey(const char* label, KeyBind& key, float samelineOffset, const ImVec2& size) noexcept
{
    const auto id{ GetID(label) };
    PushID(label);

    TextUnformatted(label);
    SameLine(samelineOffset);

    if (GetActiveID() == id) {
        PushStyleColor(ImGuiCol_Button, GetColorU32(ImGuiCol_ButtonActive));
        Button("..."_o, size);
        PopStyleColor();

        GetCurrentContext()->ActiveIdAllowOverlap = true;
        if ((!IsItemHovered() && GetIO().MouseClicked[0]) || key.setToPressedKey())
            ClearActiveID();
    }
    else if (Button(key.toString(), size)) {
        SetActiveID(id, GetCurrentWindow());
    }

    PopID();
}