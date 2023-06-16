#pragma once
#include "WndAPI.h"
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class ImGuiController
{
public:
    // ä÷êî
    ImGuiController(void) {};
    ~ImGuiController(void);

    void Initialize(WndAPI* wndPtr);
    void Begin(void);
    void End(void);
    void Draw(void);
};

