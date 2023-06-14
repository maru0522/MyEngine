#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <array>
#include <dinput.h>
#include <Xinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "xinput.lib")
#include <wrl.h>

#include "WndAPI.h"
#include "Vector2.h"

namespace Input {
    void InitializeAll(WndAPI* p_wndapi);
    void UpdateAll(void);

    class Keyboard
    {
    public:
        // 関数
        static void Initialize(WndAPI* p_wndapi);
        static void Update(void);

        // 押した瞬間
        static inline bool IsTrigger(uint8_t key) { return !sKeysPre_[key] && sKeys_[key]; }

        // 押されている時
        static inline bool IsDown(uint8_t key) { return sKeys_[key]; }

        // 離された瞬間
        static inline bool IsReleased(uint8_t key) { return sKeysPre_[key] && !sKeys_[key]; }

    private:
        // 変数
        // キーボードデバイス
        static Microsoft::WRL::ComPtr<IDirectInputDevice8> sKeyboard_;
        static std::array<BYTE, 256> sKeysPre_;
        static std::array<BYTE, 256> sKeys_;
    };

    class DIPad
    {
    public:
        // 関数
        //static void Initialize(WndAPI* p_wndapi);
        //static void Update(void);

        // 押した瞬間
        static inline bool IsTrigger(uint8_t button) { return !sDiStatePre_.rgbButtons[button] && sDiState_.rgbButtons[button]; }

        // 押されている時
        static inline bool IsDown(uint8_t button) { return sDiState_.rgbButtons[button]; }

        // 離された瞬間
        static inline bool IsReleased(uint8_t button) { return sDiStatePre_.rgbButtons[button] && !sDiState_.rgbButtons[button]; }

    private:
        // 関数
        //static BOOL __stdcall DeviceFindCallBack(const DIDEVICEINSTANCE* pdidInstance, void* pContext);

    private:
        // 変数
        static Microsoft::WRL::ComPtr<IDirectInputDevice8> sDiPad_;
        static DIJOYSTATE sDiStatePre_;
        static DIJOYSTATE sDiState_;
        static bool sIsConnect_;
    };

    class XPad
    {
    public:
        // 定義
        struct CustomDeadZone
        {
            uint16_t xLeftValueX_{};
            uint16_t xLeftValueY_{};
            uint16_t xRightValueX_{};
            uint16_t xRightValueY_{};
        };

    public:
        // 関数
        static void Initialize(void);
        static void Update(void);

        // 押した瞬間
        static inline bool IsTrigger(uint16_t button) { return sXStatePre_.Gamepad.wButtons != button && sXState_.Gamepad.wButtons == button; }

        // 押されている時
        static inline bool IsDown(uint16_t button) { return sXState_.Gamepad.wButtons == button; }

        // 離された瞬間
        static inline bool IsReleased(uint16_t button) { return sXStatePre_.Gamepad.wButtons == button && sXState_.Gamepad.wButtons != button; }

        static const Vector2 GetLStick(void);
        static const Vector2 GetLStickRaw(void);
        static const Vector2 GetLStickCustom(void);

        static const Vector2 GetRStick(void);
        static const Vector2 GetRStickRaw(void);
        static const Vector2 GetRStickCustom(void);

        static inline bool IsTriggerLT(void) { return sXState_.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD; }
        static inline uint8_t GetLT(void) { return sXState_.Gamepad.bLeftTrigger; }

        static inline bool IsTriggerRT(void) { return sXState_.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD; }
        static inline uint8_t GetRT(void) { return sXState_.Gamepad.bRightTrigger; }

        static void Vibrate(int32_t lPower = 0, int32_t rPower = 0); // 引数なしで振動停止

        //setter
        static void SetDeadZone(const Vector2& leftValueXY, const Vector2& rightValueXY);
        static inline void SetDeadZone(const CustomDeadZone& deadzone) { sDeadZone_ = deadzone; }

    private:
        // 変数
        static CustomDeadZone sDeadZone_;
        static XINPUT_STATE sXStatePre_;
        static XINPUT_STATE sXState_;
        static bool sIsConnect_;
    };

    class Mouse
    {
    public:
        // 定義
        struct MouseState
        {
            DIMOUSESTATE mState_;
            // 2D画面上の座標
            Vector2 cursorPos2d_;
            // スクロール量
            float scroll_;

            // 初期化関数
            void Initialize(void) {
                for (size_t i = 0; i < 4; i++) { mState_.rgbButtons[i] = 0; } // ボタン
                mState_.lX = mState_.lY = mState_.lZ = 0;                     // 各軸の移動量
                cursorPos2d_ = { 0.f,0.f };                                   // 位置
                scroll_ = 0.f;                                                // スクロール
            }
        };

        enum class Click
        {
            LEFT,
            RIGHT,
            CENTER,
        };
        
        // 関数
        static void Initialize(WndAPI* p_wndapi);
        static void Update(void);
        
        // 押した瞬間
        static inline bool IsTrigger(Click click) { return !sMouseStatePre_.mState_.rgbButtons[static_cast<size_t>(click)] && sMouseState_.mState_.rgbButtons[static_cast<size_t>(click)]; }
        // 押されている時
        static inline bool IsDown(Click click) { return sMouseState_.mState_.rgbButtons[static_cast<size_t>(click)]; }
        // 離された瞬間
        static inline bool IsReleased(Click click) { return sMouseStatePre_.mState_.rgbButtons[static_cast<size_t>(click)] && !sMouseState_.mState_.rgbButtons[static_cast<size_t>(click)]; }

        static inline const Vector2& GetCursorPos2D(void) { return sMouseState_.cursorPos2d_; }
        static inline float GetScroll(void) { return sMouseState_.scroll_; }
    private:
        // 変数
        static Microsoft::WRL::ComPtr<IDirectInputDevice8> sMouse_;
        static MouseState sMouseStatePre_;
        static MouseState sMouseState_;
        static WndAPI* sWndApiPtr_;
    };
}

#ifndef NON_NAMESPACE_Input // 定義でusing解除

    using KEYS = Input::Keyboard;
    using DPAD = Input::DIPad;
    using XPAD = Input::XPad;

#endif