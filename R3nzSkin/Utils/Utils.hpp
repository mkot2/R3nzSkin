#pragma once

#include <concepts>
#include <mutex>
#include <random>
#include <string>

#include "../pcg/pcg_random.hpp"

class RandomGenerator {
public:
    template <std::integral T>
    [[nodiscard]] static T random(T min, T max) noexcept
    {
        std::scoped_lock lock{ mutex };
        return std::uniform_int_distribution{ min, max }(gen);
    }

    template <std::floating_point T>
    [[nodiscard]] static T random(T min, T max) noexcept
    {
        std::scoped_lock lock{ mutex };
        return std::uniform_real_distribution{ min, max }(gen);
    }

    template <typename T>
    [[nodiscard]] static std::enable_if_t<std::is_enum_v<T>, T> random(T min, T max) noexcept
    {
        return static_cast<T>(random(static_cast<std::underlying_type_t<T>>(min), static_cast<std::underlying_type_t<T>>(max)));
    }
private:
    inline static pcg_extras::seed_seq_from<std::random_device> seed_source;
    inline static pcg32_c1024_fast gen{ seed_source };
    inline static std::mutex mutex;
};

template <typename T>
[[nodiscard]] T random(T min, T max) noexcept
{
    return RandomGenerator::random(min, max);
}

class KeyBind {
public:
    enum KeyCode : unsigned char {
        APOSTROPHE = 0, COMMA, MINUS, PERIOD, SLASH, KEY_0, KEY_1, KEY_2,
        KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, SEMICOLON, EQUALS,
        A, ADD, B, BACKSPACE, C, CAPSLOCK, D, DECIMAL, DEL, DIVIDE, DOWN, E,
        END, ENTER, F, F1, F10, F11, F12, F2, F3, F4, F5, F6, F7, F8, F9, G,
        H, HOME, I, INSERT, J, K, L, LALT, LCTRL, LEFT, LSHIFT, M, MOUSE1, MOUSE2,
        MOUSE3, MOUSE4, MOUSE5, MULTIPLY, MOUSEWHEEL_DOWN, MOUSEWHEEL_UP, N, NONE,
        NUMPAD_0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4, NUMPAD_5, NUMPAD_6, NUMPAD_7,
        NUMPAD_8, NUMPAD_9, O, P, PAGE_DOWN, PAGE_UP, Q, R, RALT, RCTRL, RIGHT, RSHIFT,
        S, SPACE, SUBTRACT, T, TAB, U, UP, V, W, X, Y, Z, LEFTBRACKET, BACKSLASH,
        RIGHTBRACKET, BACKTICK, MAX
    };

    KeyBind() = default;
    explicit KeyBind(KeyCode keyCode) noexcept;
    explicit KeyBind(const char* keyName) noexcept;

    bool operator==(KeyCode keyCode) const noexcept { return this->keyCode == keyCode; }
    friend bool operator==(const KeyBind& a, const KeyBind& b) noexcept { return a.keyCode == b.keyCode; }

    const char* toString() const noexcept;
    int getKey() const noexcept;
    bool isPressed() const noexcept;
    bool isDown() const noexcept;
    bool isSet() const noexcept { return keyCode != KeyCode::NONE; }

    bool setToPressedKey() noexcept;
private:
    KeyCode keyCode{ KeyCode::NONE };
};

class KeyBindToggle : public KeyBind {
public:
    using KeyBind::KeyBind;

    void handleToggle() noexcept;
    bool isToggled() const noexcept { return toggledOn; }
private:
    bool toggledOn = true;
};
