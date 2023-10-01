#include "TitleScene.h"
#include "Input.h"
#include "SceneManager.h"

void TitleScene::Initialize(void)
{
}

void TitleScene::Update(void)
{
    if (KEYS::IsTrigger(DIK_SPACE))
    {
        SceneManager::GetInstance()->RequestChangeScene(SceneFactory::Usage::DEMO);
    }
}

void TitleScene::Draw3d(void)
{
}

void TitleScene::Draw2dFore(void)
{
}

void TitleScene::Draw2dBack(void)
{
}

void TitleScene::Finalize(void)
{
}
