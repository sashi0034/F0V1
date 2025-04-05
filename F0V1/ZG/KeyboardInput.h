#pragma once

#include "Script/ScriptBindMacros.h"

namespace ZG
{
    /// @brief メンバ関数ポインタからTを取得
    // template <typename T, typename R>
    // T get_class_type(R T::*);
}

namespace ZG
{
    class KeyboardInput
    {
    public:
        ASAPI_VALUE_CLASS(KeyboardInput, asOBJ_POD | asOBJ_APP_CLASS_ALLINTS);

        KeyboardInput() = default;

        constexpr KeyboardInput(uint8_t code) : m_code(code)
        {
        }

        bool down() const;
        ASAPI_CLASS_METHOD("bool down() const", down);

        bool pressed() const;
        ASAPI_CLASS_METHOD("bool pressed() const", pressed);

        bool up() const;
        ASAPI_CLASS_METHOD("bool up() const", up);

    private:
        uint8_t m_code{};
    };

    inline constexpr KeyboardInput KeyCancel{0x03};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyCancel", KeyCancel);

    inline constexpr KeyboardInput KeyBackspace{0x08};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyBackspace", KeyBackspace);

    inline constexpr KeyboardInput KeyTab{0x09};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyTab", KeyTab);

    inline constexpr KeyboardInput KeyClear{0x0C};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyClear", KeyClear);

    inline constexpr KeyboardInput KeyEnter{0x0D};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyEnter", KeyEnter);

    inline constexpr KeyboardInput KeyShift{0x10};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyShift", KeyShift);

    inline constexpr KeyboardInput KeyControl{0x11};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyControl", KeyControl);

    inline constexpr KeyboardInput KeyAlt{0x12};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyAlt", KeyAlt);

    inline constexpr KeyboardInput KeyPause{0x13};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyPause", KeyPause);

    inline constexpr KeyboardInput KeyEscape{0x1B};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyEscape", KeyEscape);

    inline constexpr KeyboardInput KeySpace{0x20};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeySpace", KeySpace);

    inline constexpr KeyboardInput KeyPageUp{0x21};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyPageUp", KeyPageUp);

    inline constexpr KeyboardInput KeyPageDown{0x22};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyPageDown", KeyPageDown);

    inline constexpr KeyboardInput KeyEnd{0x23};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyEnd", KeyEnd);

    inline constexpr KeyboardInput KeyHome{0x24};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyHome", KeyHome);

    inline constexpr KeyboardInput KeyLeft{0x25};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyLeft", KeyLeft);

    inline constexpr KeyboardInput KeyUp{0x26};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyUp", KeyUp);

    inline constexpr KeyboardInput KeyRight{0x27};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyRight", KeyRight);

    inline constexpr KeyboardInput KeyDown{0x28};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyDown", KeyDown);

    inline constexpr KeyboardInput KeyPrintScreen{0x2C};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyPrintScreen", KeyPrintScreen);

    inline constexpr KeyboardInput KeyInsert{0x2D};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyInsert", KeyInsert);

    inline constexpr KeyboardInput KeyDelete{0x2E};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyDelete", KeyDelete);

    inline constexpr KeyboardInput Key0{0x30};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput Key0", Key0);

    inline constexpr KeyboardInput Key1{0x31};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput Key1", Key1);

    inline constexpr KeyboardInput Key2{0x32};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput Key2", Key2);

    inline constexpr KeyboardInput Key3{0x33};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput Key3", Key3);

    inline constexpr KeyboardInput Key4{0x34};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput Key4", Key4);

    inline constexpr KeyboardInput Key5{0x35};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput Key5", Key5);

    inline constexpr KeyboardInput Key6{0x36};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput Key6", Key6);

    inline constexpr KeyboardInput Key7{0x37};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput Key7", Key7);

    inline constexpr KeyboardInput Key8{0x38};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput Key8", Key8);

    inline constexpr KeyboardInput Key9{0x39};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput Key9", Key9);

    inline constexpr KeyboardInput KeyA{0x41};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyA", KeyA);

    inline constexpr KeyboardInput KeyB{0x42};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyB", KeyB);

    inline constexpr KeyboardInput KeyC{0x43};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyC", KeyC);

    inline constexpr KeyboardInput KeyD{0x44};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyD", KeyD);

    inline constexpr KeyboardInput KeyE{0x45};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyE", KeyE);

    inline constexpr KeyboardInput KeyF{0x46};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF", KeyF);

    inline constexpr KeyboardInput KeyG{0x47};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyG", KeyG);

    inline constexpr KeyboardInput KeyH{0x48};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyH", KeyH);

    inline constexpr KeyboardInput KeyI{0x49};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyI", KeyI);

    inline constexpr KeyboardInput KeyJ{0x4A};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyJ", KeyJ);

    inline constexpr KeyboardInput KeyK{0x4B};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyK", KeyK);

    inline constexpr KeyboardInput KeyL{0x4C};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyL", KeyL);

    inline constexpr KeyboardInput KeyM{0x4D};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyM", KeyM);

    inline constexpr KeyboardInput KeyN{0x4E};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyN", KeyN);

    inline constexpr KeyboardInput KeyO{0x4F};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyO", KeyO);

    inline constexpr KeyboardInput KeyP{0x50};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyP", KeyP);

    inline constexpr KeyboardInput KeyQ{0x51};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyQ", KeyQ);

    inline constexpr KeyboardInput KeyR{0x52};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyR", KeyR);

    inline constexpr KeyboardInput KeyS{0x53};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyS", KeyS);

    inline constexpr KeyboardInput KeyT{0x54};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyT", KeyT);

    inline constexpr KeyboardInput KeyU{0x55};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyU", KeyU);

    inline constexpr KeyboardInput KeyV{0x56};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyV", KeyV);

    inline constexpr KeyboardInput KeyW{0x57};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyW", KeyW);

    inline constexpr KeyboardInput KeyX{0x58};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyX", KeyX);

    inline constexpr KeyboardInput KeyY{0x59};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyY", KeyY);

    inline constexpr KeyboardInput KeyZ{0x5A};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyZ", KeyZ);

    inline constexpr KeyboardInput KeyNum0{0x60};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNum0", KeyNum0);

    inline constexpr KeyboardInput KeyNum1{0x61};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNum1", KeyNum1);

    inline constexpr KeyboardInput KeyNum2{0x62};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNum2", KeyNum2);

    inline constexpr KeyboardInput KeyNum3{0x63};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNum3", KeyNum3);

    inline constexpr KeyboardInput KeyNum4{0x64};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNum4", KeyNum4);

    inline constexpr KeyboardInput KeyNum5{0x65};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNum5", KeyNum5);

    inline constexpr KeyboardInput KeyNum6{0x66};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNum6", KeyNum6);

    inline constexpr KeyboardInput KeyNum7{0x67};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNum7", KeyNum7);

    inline constexpr KeyboardInput KeyNum8{0x68};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNum8", KeyNum8);

    inline constexpr KeyboardInput KeyNum9{0x69};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNum9", KeyNum9);

    inline constexpr KeyboardInput KeyNumMultiply{0x6A};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNumMultiply", KeyNumMultiply);

    inline constexpr KeyboardInput KeyNumAdd{0x6B};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNumAdd", KeyNumAdd);

    inline constexpr KeyboardInput KeyNumEnter{0x6C};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNumEnter", KeyNumEnter);

    inline constexpr KeyboardInput KeyNumSubtract{0x6D};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNumSubtract", KeyNumSubtract);

    inline constexpr KeyboardInput KeyNumDecimal{0x6E};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNumDecimal", KeyNumDecimal);

    inline constexpr KeyboardInput KeyNumDivide{0x6F};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNumDivide", KeyNumDivide);

    inline constexpr KeyboardInput KeyF1{0x70};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF1", KeyF1);

    inline constexpr KeyboardInput KeyF2{0x71};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF2", KeyF2);

    inline constexpr KeyboardInput KeyF3{0x72};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF3", KeyF3);

    inline constexpr KeyboardInput KeyF4{0x73};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF4", KeyF4);

    inline constexpr KeyboardInput KeyF5{0x74};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF5", KeyF5);

    inline constexpr KeyboardInput KeyF6{0x75};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF6", KeyF6);

    inline constexpr KeyboardInput KeyF7{0x76};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF7", KeyF7);

    inline constexpr KeyboardInput KeyF8{0x77};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF8", KeyF8);

    inline constexpr KeyboardInput KeyF9{0x78};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF9", KeyF9);

    inline constexpr KeyboardInput KeyF10{0x79};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF10", KeyF10);

    inline constexpr KeyboardInput KeyF11{0x7A};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF11", KeyF11);

    inline constexpr KeyboardInput KeyF12{0x7B};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF12", KeyF12);

    inline constexpr KeyboardInput KeyF13{0x7C};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF13", KeyF13);

    inline constexpr KeyboardInput KeyF14{0x7D};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF14", KeyF14);

    inline constexpr KeyboardInput KeyF15{0x7E};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF15", KeyF15);

    inline constexpr KeyboardInput KeyF16{0x7F};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF16", KeyF16);

    inline constexpr KeyboardInput KeyF17{0x80};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF17", KeyF17);

    inline constexpr KeyboardInput KeyF18{0x81};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF18", KeyF18);

    inline constexpr KeyboardInput KeyF19{0x82};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF19", KeyF19);

    inline constexpr KeyboardInput KeyF20{0x83};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF20", KeyF20);

    inline constexpr KeyboardInput KeyF21{0x84};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF21", KeyF21);

    inline constexpr KeyboardInput KeyF22{0x85};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF22", KeyF22);

    inline constexpr KeyboardInput KeyF23{0x86};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF23", KeyF23);

    inline constexpr KeyboardInput KeyF24{0x87};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyF24", KeyF24);

    inline constexpr KeyboardInput KeyNumLock{0x90};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNumLock", KeyNumLock);

    inline constexpr KeyboardInput KeyLShift{0xA0};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyLShift", KeyLShift);

    inline constexpr KeyboardInput KeyRShift{0xA1};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyRShift", KeyRShift);

    inline constexpr KeyboardInput KeyLControl{0xA2};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyLControl", KeyLControl);

    inline constexpr KeyboardInput KeyRControl{0xA3};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyRControl", KeyRControl);

    inline constexpr KeyboardInput KeyLAlt{0xA4};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyLAlt", KeyLAlt);

    inline constexpr KeyboardInput KeyRAlt{0xA5};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyRAlt", KeyRAlt);

    inline constexpr KeyboardInput KeyNextTrack{0xB0};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyNextTrack", KeyNextTrack);

    inline constexpr KeyboardInput KeyPreviousTrack{0xB1};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyPreviousTrack", KeyPreviousTrack);

    inline constexpr KeyboardInput KeyStopMedia{0xB2};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyStopMedia", KeyStopMedia);

    inline constexpr KeyboardInput KeyPlayPauseMedia{0xB3};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyPlayPauseMedia", KeyPlayPauseMedia);

    inline constexpr KeyboardInput KeyColon_JIS{0xBA};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyColon_JIS", KeyColon_JIS);

    inline constexpr KeyboardInput KeySemicolon_US{0xBA};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeySemicolon_US", KeySemicolon_US);

    inline constexpr KeyboardInput KeySemicolon_JIS{0xBB};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeySemicolon_JIS", KeySemicolon_JIS);

    inline constexpr KeyboardInput KeyEqual_US{0xBB};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyEqual_US", KeyEqual_US);

    inline constexpr KeyboardInput KeyComma{0xBC};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyComma", KeyComma);

    inline constexpr KeyboardInput KeyMinus{0xBD};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyMinus", KeyMinus);

    inline constexpr KeyboardInput KeyPeriod{0xBE};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyPeriod", KeyPeriod);

    inline constexpr KeyboardInput KeySlash{0xBF};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeySlash", KeySlash);

    inline constexpr KeyboardInput KeyGraveAccent{0xC0};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyGraveAccent", KeyGraveAccent);

    inline constexpr KeyboardInput KeyCommand{0xD8};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyCommand", KeyCommand);

    inline constexpr KeyboardInput KeyLeftCommand{0xD9};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyLeftCommand", KeyLeftCommand);

    inline constexpr KeyboardInput KeyRightCommand{0xDA};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyRightCommand", KeyRightCommand);

    inline constexpr KeyboardInput KeyLBracket{0xDB};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyLBracket", KeyLBracket);

    inline constexpr KeyboardInput KeyYen_JIS{0xDC};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyYen_JIS", KeyYen_JIS);

    inline constexpr KeyboardInput KeyBackslash_US{0xDC};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyBackslash_US", KeyBackslash_US);

    inline constexpr KeyboardInput KeyRBracket{0xDD};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyRBracket", KeyRBracket);

    inline constexpr KeyboardInput KeyCaret_JIS{0xDE};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyCaret_JIS", KeyCaret_JIS);

    inline constexpr KeyboardInput KeyApostrophe_US{0xDE};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyApostrophe_US", KeyApostrophe_US);

    inline constexpr KeyboardInput KeyUnderscore_JIS{0xE2};
    ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyUnderscore_JIS", KeyUnderscore_JIS);
}
