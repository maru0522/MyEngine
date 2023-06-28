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
#include "PostEffect.h"
#include "GaussianBlur.h"

class FrameWork
{
public:
    // ä÷êî
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
    // ïœêî
    InitDirectX* iDXPtr_{ InitDirectX::GetInstance() };                                         // ptr_InitDirectX
    PSOManager* psoMPtr_{ PSOManager::GetInstance() };                                          // ptr_GraphicsPipeline

    std::unique_ptr<WndAPI> wnd_{ std::make_unique<WndAPI>() };                                 // WndAPI
    std::unique_ptr<TextureManager> texM_{ std::make_unique<TextureManager>() };                // textureManager
    std::unique_ptr<ModelManager> modelM_{ std::make_unique<ModelManager>(texM_.get()) };       // ModelManager
    std::unique_ptr<AudioManager> audioM_{ std::make_unique<AudioManager>() };                  // audioManager
    std::unique_ptr<SceneManager> sceneM_{ std::make_unique<SceneManager>() };                  // sceneManager
    std::unique_ptr<ImGuiController> imguiController_{ std::make_unique<ImGuiController>() };   // imguiController

    std::unique_ptr<PostEffect> postEffect_;
};

