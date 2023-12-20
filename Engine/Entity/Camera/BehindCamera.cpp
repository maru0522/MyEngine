#include "BehindCamera.h"

BehindCamera::BehindCamera(const std::string& arg_id) : ICamera()
{
    // id として、"ICamera" が既に入っているので初期化
    id_.clear();
    // 通常カメラだと識別できるよう固有idを追加
    std::string id("BehindCamera_" + arg_id);
    // 新規idを代入
    id_ = id;

    // アフィン変換に姿勢を使うか。
    is_affinUseAxes_ = true;
}

void BehindCamera::Update(void)
{
    // 座標 = プレイヤーの座標 + プレイヤーの上ベクトル * 10.f;
    transform_.position = pos_player_ + (axes_player_.up * 70.f);

    // 姿勢ベクトル計算
    // 正面 = プレイヤーの座標 - カメラの座標（カメラ->プレイヤー）
    axes_.forward = pos_player_ - transform_.position;
    // 右 = プレイヤーと同じ
    axes_.right = axes_player_.right;
    // 上 = プレイヤーの正面
    axes_.up = axes_player_.forward;

    ICamera::Update();
}
