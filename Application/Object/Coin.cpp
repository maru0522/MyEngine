#include "Coin.h"
#include "SimplifyImGui.h"

Coin::Coin(CollisionManager* arg_colMPtr) : Object3D("Resources/model/coin/coin.obj"), colMPtr_(arg_colMPtr)
{
    // コイン用コライダーの登録
    arg_colMPtr->Register(&collision_contact_);
    collision_contact_.SetID("coin_contact");
    collision_contact_.callback_onTrigger_ = std::bind(&Coin::Collision_onTrigger, this);     // trigger
    collision_contact_.callback_onCollision_ = std::bind(&Coin::Collision_onCollision, this); // collision
    collision_contact_.radius = kRadius_;

    // 初期姿勢
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };

    collision_contact_.center = { 3000,3000,3000 };

    // コインSEの音量調整
    se_getCoin_.SetVolume(0.3f);
}

Coin::~Coin(void)
{
    // 登録を抹消
    colMPtr_->UnRegister(&collision_contact_);

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

void Coin::Update(void)
{
    //GUI::Begin("coin");
    //GUI::Text("pos:(%f,%f,%f)", transform_.position.x, transform_.position.y, transform_.position.z);
    //GUI::SliderFloat3("transform_pos", transform_.position, -200.f, 200.f);
    //GUI::SliderFloat3("transform_rot", transform_.rotation, 0.f, 6.28319f);
    //GUI::End();

    //if (is_takenPre_ == false && is_taken_)
    //{
    //    if (se_getCoin_.GetPlaying()) se_getCoin_.Stop();
    //    se_getCoin_.Play();
    //}
    //is_takenPre_ = is_taken_;

    // 姿勢（axes_)の要素が全て0ではない
    const bool isNonZeroAll = axes_.forward.IsNonZero() && axes_.right.IsNonZero() && vec3_newUp_.IsNonZero();

    // 姿勢の調整
    axes_.up = vec3_newUp_; // 新規上ベクトルの設定。 vec3_newUpの更新は"Collision_onCollision()"内にて
    if (isNonZeroAll)
    {
        axes_.right = Math::Vec3::Cross(axes_.up, axes_.forward).Normalize(); // 新規右ベクトルの設定。
        axes_.forward = Math::Vec3::Cross(axes_.right, axes_.up).Normalize(); // 新規正面ベクトルの設定
    }

    // 上ベクトルを軸に"raddian_"だけ回転させた状態を表すクオータニオン
    Quaternion rotateQ = Math::QuaternionF::MakeAxisAngle(axes_.up, kRotationRadian_);
    // クオータニオンを用いて正面と右方向のベクトルを回転
    axes_.forward = Math::QuaternionF::RotateVector(axes_.forward, rotateQ);
    axes_.right = Math::QuaternionF::RotateVector(axes_.right, rotateQ);

    matTrans_.mat_world = Math::Function::AffinTrans(transform_, axes_, &matTrans_);

    Object3D::Update();
}

void Coin::Draw(void)
{
    Object3D::Draw();
}

void Coin::Collision_onTrigger(void)
{
    // nullチェック
    if (!collision_contact_.GetOther()) { return; }

    if (collision_contact_.GetOther()->GetID() == "player")
    {
        is_taken_ = true;
        if (se_getCoin_.GetPlaying())
        {
            se_getCoin_.Stop();
        }
        se_getCoin_.Play();
    }
}

void Coin::Collision_onCollision(void)
{
    // nullチェック
    if (!collision_contact_.GetOther()) { return; }

    if (collision_contact_.GetOther()->GetID() == "gravityArea")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(collision_contact_.GetOther());

        // 球状重力エリア内に入ってる場合に行う処理。
        const Vector3 planet2Coin = Vector3(collision_contact_.center - other->center).Normalize(); // 星の中心からコインまでのベクトル
        // 新しい上ベクトル用に保存。
        vec3_newUp_ = planet2Coin;
    }
}

void Coin::SetPosition(const Vector3& arg_pos)
{
    transform_.position = arg_pos;
    collision_contact_.center = arg_pos;
}

void Coin::SetupCircleShadows(Planet* arg_planetPtr, LightManager* arg_lightManagerPtr, const Vector3& arg_atten, const Vector2& arg_factorAngle, float arg_distAtCaster)
{
    // ライトマネージャーのポインタを保存
    lightManagerPtr_ = arg_lightManagerPtr;
    // 使用可能な丸影の番号を取得
    circleShadows_num_ = arg_lightManagerPtr->UsableRightNum(LightType::CIRCLE_SHADOW);

    // 丸影が使用可能なら
    if (circleShadows_num_ >= 0)
    {
        // LightManagerに渡す用のライトタイプ
        LightType type = LightType::CIRCLE_SHADOW;

        // キャスターの位置がコイン自体からどの程度離れているか
        float distAtCoin = 1.5f;
        // コインから星までのベクトル
        Vector3 vec_coinTpPlanet = (arg_planetPtr->GetPosition() - GetPosition()).Normalize();

        arg_lightManagerPtr->SetLightActive(type, circleShadows_num_, true);
        arg_lightManagerPtr->SetLightDir(type, circleShadows_num_, vec_coinTpPlanet);
        arg_lightManagerPtr->SetLightPos(type, circleShadows_num_, GetPosition() + vec_coinTpPlanet * distAtCoin);
        arg_lightManagerPtr->SetLightDistanceAtCaster(type, circleShadows_num_, arg_distAtCaster);
        arg_lightManagerPtr->SetLightAtten(type, circleShadows_num_, arg_atten);
        arg_lightManagerPtr->SetLightFactorAngle(type, circleShadows_num_, arg_factorAngle);
    }
}
