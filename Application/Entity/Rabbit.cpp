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

        GUI::Begin("rabbit state");
        GUI::SliderFloat3("atten", atten, 0, 1);
        GUI::End();

        lightManagerPtr_->SetLightDir(type, circleShadows_num_, vec_rabbitTpPlanet);
        lightManagerPtr_->SetLightPos(type, circleShadows_num_, pos_myself - vec_rabbitTpPlanet * distAtRabbit);
        lightManagerPtr_->SetLightDistanceAtCaster(type, circleShadows_num_, distAtCaster);
        lightManagerPtr_->SetLightAtten(type, circleShadows_num_, atten);
        lightManagerPtr_->SetLightFactorAngle(type, circleShadows_num_, factorAngle);
    }

    // 1Frame遅い描画座標等更新 ** 座標が確定した後に、当たり判定処理で座標を補正するため、1Frame遅らせないとガクつく可能性がある。
    appearance_->GetCoordinatePtr()->mat_world = coordinate_.mat_world;
    appearance_->Update();

    static float sDetectRadius{ kDetectRadius_ };
    GUI::Begin("Rabbit");
    ImGui::SliderFloat("detectRadius", &sDetectRadius, 0.f, 200.f);
    detectPlayerCollider_.radius = sDetectRadius;
    GUI::End();

    // 移動量
    Vector3 moveVec{};
    Vector3 velocity{};
    Move(moveVec, velocity); // 参照渡しで受け取る。

    // 座標更新
    Vector3 currentPos = transform_.position;
    currentPos += velocity;
    transform_.position = currentPos;

    // コライダー更新
    sphereCollider_.center = currentPos;
    detectPlayerCollider_.center = currentPos;

    // 球面のどの位置にいるかに応じて、正しい姿勢にするために3軸を再計算
    Vector3 rightFromOldAxis = Math::Vec3::Cross(axes_.up.Normalize(), axes_.forward.Normalize()); // 右ベクトル：(更新された上ベクトル x 古い正面ベクトル)
    axes_.right = rightFromOldAxis;
    Vector3 forwardFromOldAxis = Math::Vec3::Cross(axes_.right.Normalize(), axes_.up.Normalize()); // 正面ベクトル：(更新された右ベクトル x 更新された上ベクトル)
    axes_.forward = forwardFromOldAxis;

    // 移動入力があった場合
    if (moveVec.IsNonZero())
    {
        // 移動方向を向くような、移動方向に合わせた姿勢にするために右向きベクトルを再計算
        Vector3 upFromAxis = axes_.up; // 上ベクトル：(更新された上ベクトルを取得）
        Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec.Normalize()); // 右ベクトル：(更新された上ベクトル x 移動ベクトル（移動方向 ≒ 正面ベクトル))
        axes_.right = rightFromMoveVec.Normalize();
        axes_.forward = moveVec.Normalize();
    }

    coordinate_.mat_world = Math::Function::AffinTrans(transform_, axes_);
}

void Rabbit::Draw(void)
{
    // 赤色のテクスチャを適用。（クソ見辛い）
    if (isCaptured_ == false) { appearance_->Draw(); }
    // デフォルト表示（対応するテクスチャがそもそもないので、MissingTextureに置き換わる。めっちゃlog出る。）
    //appearance_->Draw(/*"Resources/red1x1.png"*/);
}

void Rabbit::Move(Vector3& moveVec, Vector3& velocity)
{
    // 移動ベクトル
    //moveVec += pForwardFromCamera * inputVec.y; // 入力ベクトルに応じて加算
    //moveVec += redefinitionPRightFromCamera * inputVec.x;

    // プレイヤから兎方向へのベクトルをそのまま移動ベクトルとして起用する（仮）
    // [メモ]プレイヤーの向きと兎の向きを内積でとって、直角に近いほど速度をある程度減速させれば、ターンしたときでも捕まえやすくなるのでは？
    moveVec = (transform_.position - pPos_).Normalize();

    // 重力
    jumpVecNorm_ -= kGravity_;

    // ジャンプベクトル
    Vector3 jumpVec{};
    jumpVec += axes_.up.Normalize() * jumpVecNorm_;

    // 移動量
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += jumpVec;
}

void Rabbit::OnCollision(void)
{
    if (sphereCollider_.GetOther()->GetID() == "gravityArea")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // 球状重力エリア内に入ってる場合に行う処理。
        Vector3 center2PlayerVec = sphereCollider_.center - other->center;
        axes_.up = center2PlayerVec.Normalize();
    }
    if (sphereCollider_.GetOther()->GetID() == "terrainSurface")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // ジャンプ量
        jumpVecNorm_ = 0.f;

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
        isCaptured_ = true;
    }
}

void Rabbit::OnDetectPlayer(void)
{
    if (detectPlayerCollider_.GetOther()->GetID() == "player")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(detectPlayerCollider_.GetOther());

        // 検知したプレイヤの座標を記録する。
        pPos_ = other->center;
    }
}
