#pragma once
#include "WndAPI.h"
#include "InitDirectX.h"
#include "TextureManager.h"
#include "PSOManager.h"
#include "CameraManager.h"
#include "ModelManager.h"
#include "SceneManager.h"
#include <memory>
#include "ImguiController.h"
#include "AudioManager.h"
#include "PostEffectManager.h"
#include "UI.h"

class FrameWork
{
public:
    // 関数
    virtual ~FrameWork(void) = default;

    void Run(void);

    virtual void Initialize();
    virtual void Update(void);
    virtual void Draw(void) = 0;
    virtual void Finalize(void);

    virtual bool EndRequest(void);

private:
    void DebugGui(void);
    void ResetPostEffect(int num);

protected:
    // 変数
    InitDirectX* iDXPtr_{ InitDirectX::GetInstance() };                                         // ptr_InitDirectX
    PSOManager* psoMPtr_{ PSOManager::GetInstance() };                                          // ptr_GraphicsPipeline
    CameraManager* camMPtr_{ CameraManager::GetInstance() };                                    // ptr_CameraManager
    UI* uiPtr_{ UI::GetInstance() };

    std::unique_ptr<WndAPI> wnd_{ std::make_unique<WndAPI>() };                                 // WndAPI
    std::unique_ptr<TextureManager> texM_{ std::make_unique<TextureManager>() };                // textureManager
    std::unique_ptr<ModelManager> modelM_{ std::make_unique<ModelManager>(texM_.get()) };       // ModelManager
    std::unique_ptr<AudioManager> audioM_{ std::make_unique<AudioManager>() };                  // audioManager
    std::unique_ptr<ImGuiController> imguiController_{ std::make_unique<ImGuiController>() };   // imguiController

    PostEffectManager* postEffectMPtr_{ PostEffectManager::GetInstance() };
};

