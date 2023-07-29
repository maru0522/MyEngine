#pragma once
#include <wrl.h>
#include "WndAPI.h"
#include "Vector4.h"
#include <d3d12.h>

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

    // ä÷êî
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

    // ïœêî
    UIStyle style_;

public:
    // setterÅEgetter
    inline void SetStyle(UIStyle style) { style_ = style; }
};

