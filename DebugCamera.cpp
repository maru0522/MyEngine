#include "DebugCamera.h"
#include "Vector2.h"
#include "Input.h"
#include "MathUtil.h"

DebugCamera::DebugCamera(const std::string& arg_id) : ICamera()
{
    // id として、"ICamera" が既に入っているので初期化
    id_.clear();
    // デバッグカメラだと識別できるよう固有idを追加
    std::string id("DebugCamera_" + arg_id);
    // 新規idを代入
    id_ = id;
}

void DebugCamera::Update(void)
{
    using namespace Input;

    // デバッグでの移動量
    Vector3 velocity{};
    // デバッグでの回転量
    Vector3 rotation{};


    // マウスの移動量
    const Vector2& mouseVelocity = Mouse::GetCursorVec();

    // 回転
    if (Mouse::IsDown(Mouse::Click::RIGHT)) { // 右クリ押してる
        const float rotSpeed = 0.0025f;
        rotation.x += Mouse::GetCursorVec().y * rotSpeed;
        rotation.y += Mouse::GetCursorVec().x * rotSpeed;
    }

    // 平行移動
    if (!Mouse::IsDown(Mouse::Click::RIGHT) && Mouse::IsDown(Mouse::Click::CENTER)) { // 右クリ押してない && ホイール押してる
        const float moveSpeed = 0.05f;
        velocity += transformMatrix_.GetMatAxisX().Normalize() * -mouseVelocity.x * moveSpeed;
        velocity += transformMatrix_.GetMatAxisY().Normalize() * mouseVelocity.y * moveSpeed;
    }

    // 前後移動
    if (!Mouse::IsDown(Mouse::Click::RIGHT) && !Mouse::IsDown(Mouse::Click::CENTER)) { // 右クリ押してない && ホイール押してない
        const float moveSpeed = 0.01f;
        velocity += transformMatrix_.GetMatAxisZ().Normalize() * Mouse::GetScroll() * moveSpeed;
    }


    // デバッグで移動した分を加算
    transform_.position += velocity;
    // デバッグで回転した分を加算
    transform_.rotation += rotation;

    // 基底クラスの更新処理
    ICamera::Update();
}
