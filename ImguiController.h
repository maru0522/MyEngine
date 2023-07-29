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

    // �֐�
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

    // �ϐ�
    UIStyle style_;

public:
    // setter�Egetter
    inline void SetStyle(UIStyle style) { style_ = style; }
};

