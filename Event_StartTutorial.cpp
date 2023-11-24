#include "Event_StartTutorial.h"

Event_StartTutorial::Event_StartTutorial(void)
{
    // 画像サイズ
    const Vector2 size{ 1280.f,60.f };
    // 画像の色
    const Vector4 color{ 0.f,0.f,1.f,1.f };
    // 画像の位置
    Vector2 position{ 0.f,0.f };

    for (size_t i = 0; i < cinemas_.size(); i++)
    {
        cinemas_[i] = std::make_unique<Sprite>("Resources/white1x1.png");

        cinemas_[i]->SetSize(size);
        cinemas_[i]->SetColor(color);

        position = { 0.f, 660.f * i }; // 2つ目は画面の下の位置へ
        cinemas_[i]->SetPosition(position);
    }
}

void Event_StartTutorial::Execute(void)
{
    for (auto& sprite : cinemas_)
    {
        sprite->Update();
    }
}

void Event_StartTutorial::Draw(void)
{
    for (auto& sprite : cinemas_)
    {
        sprite->Draw();
    }
}
