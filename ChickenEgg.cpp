#include "ChickenEgg.h"

void ChickenEgg::Initialize(CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr_, Planet* arg_planetPtr_)
{
    // 各ptrの受け取り
    colMPtr_ = arg_colMPtr;
    lightMPtr_ = arg_lightManagerPtr_;
    planetPtr_ = arg_planetPtr_;

    // モデル読み込み
    model_[0].Load("Resources/model/chickenEgg/egg_remain1.obj");
    model_[1].Load("Resources/model/chickenEgg/egg_remain2.obj");
    model_[2].Load("Resources/model/chickenEgg/egg_remain3.obj");
    model_[3].Load("Resources/model/chickenEgg/egg_remain4.obj");
    model_[4].Load("Resources/model/chickenEgg/egg_remain5.obj");

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

    transform_.position = { 0,90,-30 };
    transform_.scale = { kModelScale_, kModelScale_, kModelScale_ };
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
    // 処理順
    /*
     * 姿勢を惑星の表面に合わせる。
     * モデルの行列計算と更新処理
     * 移動処理（chickenEggは重力のみ）
     * コライダーの座標更新
     * 蛇のカウント初期化
     */

    // 鶏卵の数が0以下かどうか
    if (eggNum_ <= 0)
    {

    }


    // *姿勢を惑星の表面に合わせる
    AdaptPosture();
    

    // *モデルの行列計算と更新処理
    // 鶏の卵が1つ以上残っているか？ ※卵の数を要素数として使うため
    const bool isEggsLeft = IsChikenEgg();
    if (isEggsLeft) // 残っているなら更新処理
    {
        // 1Frame遅い描画座標等更新 ※座標が確定した後に、当たり判定処理で座標を補正するため、1Frame遅らせないとガクつく可能性がある。
        // モデルの行列のptrを受け取る。
        TransformMatrix* TMPtr = model_[eggNum_ - 1].GetCoordinatePtr();
        // モデルのワールド行列 ※要素数の為に -1
        TMPtr->mat_world = Math::Function::AffinTrans(transform_, posture_, TMPtr); // ワールド行列を生成
        // モデルの更新処理
        model_[eggNum_ - 1].Update(); // 要素数の為に -1
    }


    // *移動処理（chickenEggは重力のみ）
    // 重力
    velocity_vertical_ -= kGravity_;
    // 垂直方向の移動量
    const Vector3 velocity_vertical = posture_.up * velocity_vertical_; // ※ローカル変数は3次元ベクトル。メンバ変数はfloat型
    const Vector3 pos = transform_.position + velocity_vertical;
    transform_.position = pos;


    // *コライダーの座標更新
    sphere_collision_.center = transform_.position;
    sphere_detectSnake_.center = transform_.position;


    // *蛇のカウント初期化 ※蛇が範囲内に何体いるのかを毎F更新するため
    snakeCount_ = 0;
}

void ChickenEgg::Draw(void)
{
    // 鶏の卵が1つ以上残っているか？ ※卵の数を要素数として使うため
    const bool isEggsLeft = IsChikenEgg();
    if (isEggsLeft == false) { return; } // 残っていないなら終了
    // モデルの描画処理
    model_[eggNum_ - 1].Draw(); // 要素数の為に -1
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
        sphere_detectSnake_.center = currentPos;
    }
}

void ChickenEgg::OnCollision_DetectSnake(void)
{
    if (sphere_detectSnake_.GetOther()->GetID() == "snake_col")
    {
        // 蛇を感知するたびカウントを +1
        snakeCount_++;
    }
}
