#pragma once
#include <wrl.h>
#include "WndAPI.h"
#include "Vector4.h"
/**
 * @file ImguiController.h
 * @brief ImGuiを使用する上での初期化処理や管理全般を行うクラス。
 */

class ImGuiController final
{
public:
    enum class UIStyle
    {
        CLASSIC,
        DARK,
        CUSTOM_SONICRIDERS,
        CUSTOM_CHERRY,
        CUSTOM_CRYSTALDEVIL,
        CUSTOM_ENEMYMOUSE,
    };

    // 関数
    ImGuiController(void);
    ~ImGuiController(void);

    void Initialize(WndAPI* wndPtr);
    void Begin(void);
    void End(void);
    void Draw(void);

private:
    void Settings(void);

    void SetColorScheme(void);
    void CustomSonicRiders(void);
    void CustomCherry(void);
    void CustomBlackDevil(void);
    void CustomEnemyMouse(void);

    // 変数
    UIStyle style_;

public:
    // setter・getter
    inline void SetStyle(UIStyle style) { style_ = style; }
};

