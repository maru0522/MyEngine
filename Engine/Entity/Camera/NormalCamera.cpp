#include "NormalCamera.h"

NormalCamera::NormalCamera(const std::string& arg_id) : ICamera()
{
    // id として、"ICamera" が既に入っているので初期化
    id_.clear();
    // 通常カメラだと識別できるよう固有idを追加
    std::string id("NormalCamera_" + arg_id);
    // 新規idを代入
    id_ = id;

    // アフィン変換に姿勢を使うか。
    is_affinUseAxes_ = true;
}

void NormalCamera::Update(void)
{
    ICamera::Update();
}
