#include "BehindCamera.h"
#include "SimplifyImGui.h"
#include "MathUtil.h"
#include "WndAPI.h"
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

    distance_fromPlayer_ = 70.f; // PlayerCommonInformation::kDist_stoop_cameraLeave_;
}

void BehindCamera::Update(void)
{
    Vector3 vec_p2c = Vector3(pos_player_ - transform_.position).Normalize();
    // 0.706
    float a = vec_p2c.Dot(axes_player_.forward);

    // 座標 = プレイヤーの座標 + プレイヤーの上ベクトル * 10.f;
    //transform_.position = pos_player_ + (axes_player_.up * distance_fromPlayer_); // 初期
    //transform_.position = pos_player_;
    //transform_.position = pos_player_ + (axes_player_.up * distance_fromPlayer_) + (-axes_player_.right * distance_fromPlayer_); // 右用
    //transform_.position = pos_player_ + (axes_player_.up * distance_fromPlayer_) + (axes_player_.right * distance_fromPlayer_); // 前から

    // 姿勢ベクトル計算
    //// 正面 = プレイヤーの座標 - カメラの座標（カメラ->プレイヤー）
    //axes_.forward = pos_player_ - transform_.position;
    //// 右 = プレイヤーと同じ
    //axes_.right = axes_player_.right;
    //// 上 = プレイヤーの正面
    //axes_.up = axes_player_.forward;



    //// 正面 = プレイヤーの座標 - カメラの座標（カメラ->プレイヤー）
    //axes_.forward = Vector3(pos_player_ - transform_.position).Normalize();
    //// 右 = プレイヤーと同じ
    //axes_.right = axes_playerModel_.up.Cross(axes_.forward).Normalize();
    //// 上 = プレイヤーの正面
    //axes_.up = axes_playerModel_.forward.Cross(axes_.right).Normalize();
    //axes_.right = axes_playerModel_.up.Cross(axes_.forward).Normalize();

    //// 正面 = プレイヤーの座標 - カメラの座標（カメラ->プレイヤー）
    //axes_.forward = axes_player_.forward.Normalize();
    //// 右 = プレイヤーと同じ
    //axes_.right = axes_player_.right.Normalize();
    //// 上 = プレイヤーの正面
    //axes_.up = axes_player_.up.Normalize();

    //if (a > 0.706f)
    //{
    //    transform_.position = pos_player_ + (axes_player_.up * distance_fromPlayer_) + (-axes_player_.forward * distance_fromPlayer_);
    //    // 正面 = プレイヤーの座標 - カメラの座標（カメラ->プレイヤー）
    //    axes_.forward = Vector3(pos_player_ - transform_.position).Normalize();
    //    // 右 = プレイヤーと同じ
    //    axes_.right = axes_player_.up.Cross(axes_.forward).Normalize();
    //    // 上 = プレイヤーの正面
    //    axes_.up = axes_player_.forward.Cross(axes_.right).Normalize();
    //    axes_.right = axes_.up.Cross(axes_.forward).Normalize();
    //}
    //else
    //{

    //}

    if (numAlgorithm_ == 1) { WFunction(); }
    if (numAlgorithm_ == 2) { SFunction(); }
    if (numAlgorithm_ == 3) { DFunction(); }


    Vector2 screenPos = GetScreen().WorldToScreenPoint(matWorld_player_);

    GUI::Begin("BehindCamera");
    GUI::Text("%f", a);
    GUI::Text("x:%f,y:%f", screenPos.x, screenPos.y);
    ImGui::InputInt("numAlgo", &numAlgorithm_);
    GUI::End();

    if(numAlgorithm_ == 0) { SphiricalFunction(); }
    if(numAlgorithm_ != 0) { ICamera::Update(); }


    
}

void BehindCamera::WFunction(void)
{
    transform_.position = pos_player_ + (axes_player_.up * distance_fromPlayer_) + (-axes_player_.forward * distance_fromPlayer_); // 旧
    // 正面 = プレイヤーの座標 - カメラの座標（カメラ->プレイヤー）
    axes_.forward = Vector3(pos_player_ - transform_.position).Normalize();
    // 右 = プレイヤーと同じ
    axes_.right = axes_player_.up.Cross(axes_.forward).Normalize();
    // 上 = プレイヤーの正面
    axes_.up = axes_player_.forward.Cross(axes_.right).Normalize();
    axes_.right = axes_.up.Cross(axes_.forward).Normalize();
}

void BehindCamera::SFunction(void)
{
    transform_.position = pos_player_ + (axes_player_.up * distance_fromPlayer_) + (axes_player_.forward * distance_fromPlayer_); // 旧
    // 正面 = プレイヤーの座標 - カメラの座標（カメラ->プレイヤー）
    axes_.forward = Vector3(pos_player_ - transform_.position).Normalize();
    // 右 = プレイヤーと同じ
    axes_.right = axes_player_.up.Cross(axes_.forward).Normalize();
    // 上 = プレイヤーの正面
    axes_.up = axes_player_.forward.Cross(axes_.right).Normalize();
    axes_.right = axes_.up.Cross(axes_.forward).Normalize();
}

void BehindCamera::DFunction(void)
{
    transform_.position = pos_player_ + (axes_player_.up * distance_fromPlayer_) + (-axes_player_.right * distance_fromPlayer_); // 旧
    // 正面 = プレイヤーの座標 - カメラの座標（カメラ->プレイヤー）
    axes_.forward = Vector3(pos_player_ - transform_.position).Normalize();
    // 右 = プレイヤーと同じ
    axes_.right = Math::Vec3::Cross(axes_player_.up,axes_.forward).Normalize();
    // 上 = プレイヤーの正面
    axes_.up = axes_.forward.Cross(axes_.right).Normalize();
    axes_.right = axes_.up.Cross(axes_.forward).Normalize();

    isAlreadyD_ = true;
}

void BehindCamera::SphiricalFunction(void)
{
    Vector3 center = pos_player_; // FPS

    using namespace Math;

    static float radius_ = 0;
    static float psi_ = 0;
    static float theta_ = 0.623f; // ->1.135
    static float phi_ = 0;

    // ビュー行列d
    matView_ = Math::Mat4::Inverse(transformMatrix_.mat_world);

    // 射影行列
    matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);

    //## 球面座標処理込みの行列生成
    Matrix4 matWorld = Mat4::Identity();
    Matrix4 matRotate = Mat4::Identity();

    // プレイヤーとの距離だけ、球面座標の中心点から下がらせる
    matWorld *= Mat4::Translate(matWorld, { 0, 0, -radius_ });
    // 球面座標系の座標仕様に合わせて回転（移動）させる
    matRotate = Mat4::RotationZ(psi_) * Mat4::RotationX(theta_) * Mat4::RotationY(phi_);
    matWorld *= matRotate;
    // ワールド座標の位置に持っていく（原点での計算後、プレイヤーの座標へ移動）
    matWorld.m[3][0] += center.x;
    matWorld.m[3][1] += center.y;
    matWorld.m[3][2] += center.z;

    //transform_.position = { matWorld.m[3][0],matWorld.m[3][1],matWorld.m[3][2] };

    transformMatrix_.mat_pos = Mat4::Translation(center);
    transformMatrix_.mat_rot = matRotate;
    transformMatrix_.mat_world = matWorld;

    // 正面 = プレイヤーの座標 - カメラの座標（カメラ->プレイヤー）
    Vector3 camPos = { matWorld.m[3][0] ,matWorld.m[3][1] ,matWorld.m[3][2] };
    axes_CameraInSphirical_.forward = Vector3(pos_player_ - camPos).Normalize();
    // 右 = プレイヤーと同じ
    axes_CameraInSphirical_.right = Math::Vec3::Cross(-axes_player_.right, axes_CameraInSphirical_.forward).Normalize();
    // 上 = プレイヤーの正面
    axes_CameraInSphirical_.up = axes_CameraInSphirical_.forward.Cross(axes_CameraInSphirical_.right).Normalize();
    axes_CameraInSphirical_.right = axes_CameraInSphirical_.up.Cross(axes_CameraInSphirical_.forward).Normalize();
    //##

    GUI::Begin("BehindCamera");
    GUI::SliderFloat("rad", &radius_, 0, 200);
    GUI::SliderFloat("psi", &psi_, 0, 3.14f);
    GUI::SliderFloat("theta", &theta_, 0, 3.14f);
    GUI::SliderFloat("phi", &phi_, 0, 3.14f);
    GUI::End();
}
