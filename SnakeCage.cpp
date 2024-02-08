#include "SnakeCage.h"

void SnakeCage::Initialize(CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr, Planet* arg_planetPtr)
{
    colMPtr_ = arg_colMPtr;
    lightMPtr_ = arg_lightManagerPtr;
    planetPtr_ = arg_planetPtr;

    Object3D::Load("Resources/model/cage/cage.obj"); //TODO: cage load

    // 影の適用
    circleShadows_num_ = lightMPtr_->UsableRightNum(LightType::CIRCLE_SHADOW);      // 使用可能な丸影の番号を取得
    lightMPtr_->SetLightActive(LightType::CIRCLE_SHADOW, circleShadows_num_, true); // 丸影を使用する

    //##コライダーの登録
    // "snakeCage_col"
    colMPtr_->Register(&sphere_collision_);
    sphere_collision_.SetID("snakeCage_col");
    sphere_collision_.callback_onCollision_ = std::bind(&SnakeCage::OnCollision, this);
    sphere_collision_.callback_onTrigger_ = std::bind(&SnakeCage::OnTrigger, this);
    sphere_collision_.radius = kCollisionRadius_;

    // 位置初期化
    transform_ = Transform::Initialize();
    // 姿勢初期化
    posture_ = Axis3::Initialize();

    transform_.position = { 0,90,-30 };
    transform_.scale = { kModelScale_, kModelScale_, kModelScale_ };
}

void SnakeCage::Update(void)
{
    // 処理順
   /*
    * 施錠後なら捕獲済みのため、捕獲可能フラグをfalse
    * 姿勢を惑星の表面に合わせる。
    * モデルの行列計算と更新処理
    * 移動処理（chickenEggは重力のみ）
    * コライダーの座標更新
    */


    // *施錠後なら捕獲済みのため、捕獲可能フラグをfalse
    if (is_lock_) { is_capture_ = false; }


    // *姿勢を惑星の表面に合わせる
    AdaptPosture();


    // *モデルの行列計算と更新処理
    // 1Frame遅い描画座標等更新 ※座標が確定した後に、当たり判定処理で座標を補正するため、1Frame遅らせないとガクつく可能性がある。
    matTrans_.mat_world = Math::Function::AffinTrans(transform_, posture_, &matTrans_); // ワールド行列を生成
    // モデルの更新処理
    Object3D::Update();


    // *移動処理（chickenEggは重力のみ）
    // 重力
    velocity_vertical_ -= kGravity_;
    // 垂直方向の移動量
    const Vector3 velocity_vertical = posture_.up * velocity_vertical_; // ※ローカル変数は3次元ベクトル。メンバ変数はfloat型
    const Vector3 pos = transform_.position + velocity_vertical;
    transform_.position = pos;


    // *コライダーの座標更新
    sphere_collision_.center = transform_.position;
}

void SnakeCage::Draw(void)
{
    Object3D::Draw();
}

void SnakeCage::Finalize(void)
{
    // コライダーをマネージャーから抹消
    colMPtr_->UnRegister(&sphere_collision_);

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

void SnakeCage::AdaptPosture(void)
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

void SnakeCage::OnCollision(void)
{
    if (sphere_collision_.GetOther()->GetID() == "gravityArea")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphere_collision_.GetOther());

        // 球状重力エリア内に入ってる場合に行う処理。
        Vector3 vec3_center2egg = sphere_collision_.center - other->center; // 星の中心から自分（鶏の卵）までのベクトル
        // 新しい上ベクトル用に保存。
        vec3_newUp_ = vec3_center2egg.Normalize();
    }
    if (sphere_collision_.GetOther()->GetID() == "terrainSurface")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphere_collision_.GetOther());

        // 現在の鶏卵の座標
        Vector3 currentPos = transform_.position;
        // 垂直方向の移動量を初期化。
        velocity_vertical_ = 0.f;

        // めり込み距離を出す (めり込んでいる想定 - 距離）なので結果はマイナス想定？？
        float diff = Vector3(sphere_collision_.center - other->center).Length() - (other->radius + sphere_collision_.radius);
        // 正規化された星の中心から自分までベクトル * めり込み距離
        currentPos += posture_.up.Normalize() * -diff; // ここをマイナス符号で値反転
        // 計算された座標を適用
        transform_.position = currentPos;
        sphere_collision_.center = currentPos;
    }
}

void SnakeCage::OnTrigger(void)
{
    if (sphere_collision_.GetOther()->GetID() == "snake_col")
    {
        // 施錠後なら終了
        if (is_lock_) { return; }

        // 捕獲フラグをtrue
        is_capture_ = true;
    }
}
