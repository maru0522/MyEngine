#include "ChickenEgg.h"

void ChickenEgg::Initialize(CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr_, Planet* arg_planetPtr_)
{
    // 各ptrの受け取り
    colMPtr_ = arg_colMPtr;
    lightMPtr_ = arg_lightManagerPtr_;
    planetPtr_ = arg_planetPtr_;

    // 影の適用
    circleShadows_num_ = lightMPtr_->UsableRightNum(LightType::CIRCLE_SHADOW);      // 使用可能な丸影の番号を取得
    lightMPtr_->SetLightActive(LightType::CIRCLE_SHADOW, circleShadows_num_, true); // 丸影を使用する

    //##コライダーの登録
    // "chickenEgg_col"
    colMPtr_->Register(&sphere_collision_);
    sphere_collision_.SetID("chickenEgg_col");
    sphere_collision_.callback_onCollision_ = std::bind(&ChickenEgg::OnCollision_Col, this);
    sphere_collision_.radius = kRadius_col_;

    // "chickenEgg_detectSnake"
    colMPtr_->Register(&sphere_detectSnake_);
    sphere_detectSnake_.SetID("chickenEgg_detectSnake");
    sphere_detectSnake_.callback_onCollision_ = std::bind(&ChickenEgg::OnCollision_DetectSnake, this);
    sphere_detectSnake_.radius = kRadius_detectSnake_;

    // 初期位置
    transform_ = Transform::Initialize();
    // 初期姿勢
    posture_ = Axis3::Initialize();
}

void ChickenEgg::Finalize(void)
{
    // コライダーをマネージャーから抹消
    colMPtr_->UnRegister(&sphere_collision_);
    colMPtr_->UnRegister(&sphere_detectSnake_);

    // 丸影を使用していないなら終了 ※例外スロー回避
    if (circleShadows_num_ < 0) { return; }
    // LightManagerに渡す用のライトタイプ
    LightType type = LightType::CIRCLE_SHADOW;
    // 使用していた丸影を初期化
    lightMPtr_->SetLightActive(type, circleShadows_num_, false);
    lightMPtr_->SetLightDir(type, circleShadows_num_, { 0,0,0 });
    lightMPtr_->SetLightPos(type, circleShadows_num_, { 0,0,0 });
    lightMPtr_->SetLightDistanceAtCaster(type, circleShadows_num_, 0.f);
    lightMPtr_->SetLightAtten(type, circleShadows_num_, { 0.f,0.f,0.f });
    lightMPtr_->SetLightFactorAngle(type, circleShadows_num_, { 0.f,0.f });
}

void ChickenEgg::Update(void)
{
    // 姿勢を惑星のための向きに修正
    AdaptPosture();
    // ワールド行列を生成
    transformMatrix_.mat_world = Math::Function::AffinTrans(transform_, posture_, &transformMatrix_);
    // コライダーの座標更新
    sphere_collision_.center = transform_.position;
    sphere_detectSnake_.center = transform_.position;
    // モデルの更新処理
}

void ChickenEgg::Draw(void)
{

}

void ChickenEgg::AdaptPosture(void)
{
    // 現在の正面ベクトル/新規上ベクトルが、0でないならtrue
    const bool isNonZero = posture_.forward.IsNonZero() && vec3_newUp_.IsNonZero(); // 最低限2つ（正面と上）があれば3軸を定義できる。
    // trueでないなら終了。
    if (isNonZero == false) { return; }

    // 姿勢の調整
    posture_.up = vec3_newUp_; // 新規上ベクトルの設定。 vec3_newUpの更新は"Collision_onCollision()"内にて
    posture_.right = Math::Vec3::Cross(posture_.up, posture_.forward).Normalize(); // 新規右ベクトルの設定。
    posture_.forward = Math::Vec3::Cross(posture_.right, posture_.up).Normalize(); // 新規正面ベクトルの設定
}

void ChickenEgg::OnCollision_Col(void)
{
}

void ChickenEgg::OnCollision_DetectSnake(void)
{
}
