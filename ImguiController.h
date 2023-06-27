#pragma once
#include <wrl.h>
#include "WndAPI.h"
#include "Vector4.h"
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class ImGuiController
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

