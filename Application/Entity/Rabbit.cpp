#include "Rabbit.h"
#include "SimplifyImGui.h"

Rabbit::Rabbit(CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr, Planet* arg_planetPtr)
    : colMPtr_(arg_colMPtr), lightManagerPtr_(arg_lightManagerPtr), planetPtr_(arg_planetPtr)
{
    arg_colMPtr->Register(&sphereCollider_);
    arg_colMPtr->Register(&detectPlayerCollider_);

    sphereCollider_.SetID("rabbit");
    detectPlayerCollider_.SetID("rabbit_detectPlayer");

    sphereCollider_.callback_onCollision_ = std::bind(&Rabbit::OnCollision, this);
    detectPlayerCollider_.callback_onCollision_ = std::bind(&Rabbit::OnDetectPlayer, this);

    sphereCollider_.radius = kRadius_;
    sphereCollider_.center = { 0,60,20 };
    detectPlayerCollider_.radius = kDetectRadius_;

    // 初期位置
    transform_.position = { 0,60,20 };
    // 初期姿勢
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };
}

Rabbit::~Rabbit(void)
{
    colMPtr_->UnRegister(&sphereCollider_);
    colMPtr_->UnRegister(&detectPlayerCollider_);

    // 丸影を使用している
    if (circleShadows_num_ >= 0)
    {
        // LightManagerに渡す用のライトタイプ
        LightType type = LightType::CIRCLE_SHADOW;

        // 使用していた丸影を初期化
        lightManagerPtr_->SetLightActive(type, circleShadows_num_, false);
        lightManagerPtr_->SetLightDir(type, circleShadows_num_, { 0,0,0 });
        lightManagerPtr_->SetLightPos(type, circleShadows_num_, { 0,0,0 });
        lightManagerPtr_->SetLightDistanceAtCaster(type, circleShadows_num_, 0.f);
        lightManagerPtr_->SetLightAtten(type, circleShadows_num_, { 0.f,0.f,0.f });
        lightManagerPtr_->SetLightFactorAngle(type, circleShadows_num_, { 0.f,0.f });
    }
}

void Rabbit::Update(void)
{
    // プレイヤーに触れられた後なら処理をスキップ
    if (isCaptured_) { return; }

    // 丸影処理
    Process_CircleShadow();

    // 1Frame遅い描画座標等更新 ** 座標が確定した後に、当たり判定処理で座標を補正するため、1Frame遅らせないとガクつく可能性がある。
    appearance_->GetCoordinatePtr()->mat_world = transformMatrix_.mat_world;
    appearance_->Update();

    // 1F前の姿勢として記録
    axes_old_ = axes_;
    // 新規上ベクトルの設定
    axes_.up = vec3_newUp_;
    // 移動する方向を正面ベクトルとして設定
    axes_.forward = vec3_moveDirection_;
    // 新規の上 x 正面のベクトルで右ベクトルを計算
    axes_.right = Math::Vec3::Cross(axes_.up, axes_.forward).Normalize(); // 姿勢を正常に保つため && あとで、正面ベクトルを再定義するため。

    // 移動
    Move();

    // コライダー更新
    sphereCollider_.center = transform_.position;
    detectPlayerCollider_.center = transform_.position;

    // 現在の上ベクトルと右ベクトルから正面ベクトルを再定義
    // 正面ベクトルが、プレイヤーを検知した瞬間のままだと、移動するにつれ兎がつぶれる（姿勢が正常ではないため）
    const Vector3 forward = Math::Vec3::Cross(axes_.right, axes_.up);
    // 再定義したベクトルを正面ベクトルに設定
    axes_.forward = forward;
    // 再定義したベクトルを移動方向ベクトルに設定。
    vec3_moveDirection_ = forward;

    transformMatrix_.mat_world = Math::Function::AffinTrans(transform_, axes_);
}

void Rabbit::Draw(void)
{
    // 赤色のテクスチャを適用。（クソ見辛い）
    if (isCaptured_ == false) { appearance_->Draw(); }
    // デフォルト表示（対応するテクスチャがそもそもないので、MissingTextureに置き換わる。めっちゃlog出る。）
    //appearance_->Draw(/*"Resources/red1x1.png"*/);
}

void Rabbit::Move(void)
{
    // 移動ベクトル
    //moveVec += pForwardFromCamera * inputVec.y; // 入力ベクトルに応じて加算
    //moveVec += redefinitionPRightFromCamera * inputVec.x;

    // [メモ]プレイヤーの向きと兎の向きを内積でとって、直角に近いほど速度をある程度減速させれば、ターンしたときでも捕まえやすくなるのでは？

    // 移動可能距離が、移動速度よりも大きいか
    const bool isBiggerDist = moveDist_ > kMoveSpeed_;

    // 重力
    velocity_vertical_ -= kGravity_;

    // 垂直方向の移動量
    const Vector3 velocity_vertical = axes_.up * velocity_vertical_; // ※ローカル変数は3次元ベクトル。メンバ変数はfloat型
    // 水平方向の移動量
    Vector3 velocity_horizontal{};
    isBiggerDist ?
        velocity_horizontal = vec3_moveDirection_ * kMoveSpeed_ : // 移動可能距離が、移動速度より小さいなら
        velocity_horizontal = vec3_moveDirection_ * moveDist_;    // 移動可能距離を超えて移動することは出来ない。

    // 合計の移動量
    const Vector3 velocity_total = velocity_vertical + velocity_horizontal;
    // 座標更新
    const Vector3 pos = transform_.position + velocity_total;
    transform_.position = pos;

    // 移動可能距離を減らす（既に移動する処理は済ませたので）
    moveDist_ -= kMoveSpeed_;
    // 移動可能距離（変数: moveDist_の値は0未満にならない）
    moveDist_ = (std::max)(moveDist_, 0.f);
}

void Rabbit::Process_CircleShadow(void)
{
    // 丸影が使用可能なら
    if (circleShadows_num_ >= 0)
    {
        // LightManagerに渡す用のライトタイプ
        LightType type = LightType::CIRCLE_SHADOW;

        // 自分自身の座標
        const Vector3& pos_myself = transform_.position;
        // 兎から星までのベクトル
        const Vector3& vec_rabbitTpPlanet = (planetPtr_->GetPosition() - pos_myself).Normalize();

        // キャスターと兎自体の距離
        float distAtRabbit = 3.f;
        // 丸影用の光源とキャスターの距離
        float distAtCaster = 3.f;
        // ライトの減衰率の各値
        static Vector3 atten = { 0.1f,0.08f,0.075f };
        // ライトの角度減衰の外値と内値
        Vector2 factorAngle = { 8.f,9.f };

        lightManagerPtr_->SetLightDir(type, circleShadows_num_, vec_rabbitTpPlanet);
        lightManagerPtr_->SetLightPos(type, circleShadows_num_, pos_myself - vec_rabbitTpPlanet * distAtRabbit);
        lightManagerPtr_->SetLightDistanceAtCaster(type, circleShadows_num_, distAtCaster);
        lightManagerPtr_->SetLightAtten(type, circleShadows_num_, atten);
        lightManagerPtr_->SetLightFactorAngle(type, circleShadows_num_, factorAngle);
    }
}

void Rabbit::OnCollision(void)
{
    if (sphereCollider_.GetOther()->GetID() == "gravityArea")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // 球状重力エリア内に入ってる場合に行う処理。
        Vector3 center2PlayerVec = sphereCollider_.center - other->center; // 星の中心からプレイヤーまでのベクトル
        // 新しい上ベクトル用に保存。
        vec3_newUp_ = center2PlayerVec.Normalize();
    }
    if (sphereCollider_.GetOther()->GetID() == "terrainSurface")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // ジャンプ量
        velocity_vertical_ = 0.f;

        // めり込み距離を出す (めり込んでいる想定 - 距離）なので結果はマイナス想定？？
        float diff = Vector3(sphereCollider_.center - other->center).Length() - (other->radius + sphereCollider_.radius);

        Vector3 currentPos = transform_.position;
        //currentPos += player->body_->coordinate_.GetUpVec().ExtractVector3();

        // 正規化された球からプレイヤーまでのベクトル * めり込み距離
        currentPos += axes_.up.Normalize() * -diff; // ここをマイナス符号で値反転

        transform_.position = currentPos;
    }
    if (sphereCollider_.GetOther()->GetID() == "player")
    {
        // 捕獲されたフラグをtrue
        isCaptured_ = true;
    }
}

void Rabbit::OnDetectPlayer(void)
{
    if (detectPlayerCollider_.GetOther()->GetID() == "player")
    {
        // 接触相手のコライダー(プレイヤー）を基底クラスから復元。
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(detectPlayerCollider_.GetOther());

        // 検知したプレイヤーから遠ざかるように、移動方向を記録する。
        vec3_moveDirection_ = Vector3(transform_.position - other->center).Normalize(); // (兎の座標 - プレイヤーの座標）
        // 検知した地点を原点としてどの程度移動するかを設定
        moveDist_ = kMoveDist_;
    }
}
