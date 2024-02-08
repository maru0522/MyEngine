#include "Snake.h"
#include "SimplifyImGui.h"
#include "MathUtil.h"
#include "ChickenEgg.h"

void Snake::Initialize(CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr, Planet* arg_planetPtr, ChickenEgg* arg_chickenEggPtr)
{
    colMPtr_ = arg_colMPtr;
    lightManagerPtr_ = arg_lightManagerPtr; 
    planetPtr_ = arg_planetPtr;
    chickenEggPtr_ = arg_chickenEggPtr;


    // 共通情報のインスタンス生成
    commonInfo_ = std::make_shared<SnakeCommonInfomation>();

    //##コライダーの登録
    // "snake_col"
    arg_colMPtr->Register(&sphere_collision_);
    sphere_collision_.SetID("snake_col");
    sphere_collision_.callback_onCollision_ = std::bind(&Snake::OnCollision, this);
    sphere_collision_.callback_onTrigger_ = std::bind(&Snake::OnTrigger, this);
    sphere_collision_.radius = SnakeCommonInfomation::kRadius_col_;
    sphere_collision_.center = { 0,60,20 };

    // "snake_detect"
    arg_colMPtr->Register(&sphere_detect_);
    sphere_detect_.SetID("snake_detect");
    sphere_detect_.callback_onCollision_ = std::bind(&Snake::OnDetect, this);
    // 検知用コライダーの半径を、複数ある検知半径の中で最も大きいものを適用
    const float radius_detect = (std::max)(SnakeCommonInfomation::kRadius_detectPlayer_, SnakeCommonInfomation::kRadius_detectEgg_);
    sphere_detect_.radius = radius_detect;



    // 初期位置
    commonInfo_->transform_.position = { 0,60,20 };
    commonInfo_->transform_.scale = { SnakeCommonInfomation::kModelScale_,SnakeCommonInfomation::kModelScale_,SnakeCommonInfomation::kModelScale_ };
    // 初期姿勢
    commonInfo_->axes_.forward = { 0,0,1 };
    commonInfo_->axes_.right = { 1,0,0 };
    commonInfo_->axes_.up = { 0,1,0 };

    exclamationMark_ = std::make_unique<ExclamationMark>();

    // behaviorMachine経由で、behavior生成
    snakeBehaviorMachine_.Initialize(this, SnakeBehavior::IDLE);

    SetupLightCircleShadows();
}

void Snake::Update(void)
{
    // 丸影処理
    Process_CircleShadow();

    // 1Frame遅い描画座標等更新 ** 座標が確定した後に、当たり判定処理で座標を補正するため、1Frame遅らせないとガクつく可能性がある。
    appearance_->GetCoordinatePtr()->mat_world = commonInfo_->transformMatrix_.mat_world;
    appearance_->Update();

    // ケージの中に入っているなら、
    if (commonInfo_->is_Caged_) 
    {
        // 座標はケージと同一にする。
        commonInfo_->transform_.position = *commonInfo_->pos_cage_;
        // 行列を更新
        commonInfo_->transformMatrix_.mat_world = Math::Function::AffinTrans(commonInfo_->transform_, commonInfo_->axes_);
        // 更新処理終了
        return;
    }

    // ビックリマークに関する処理
    exclamationMark_->SetNewUp(commonInfo_->vec3_newUp_);
    Transform exclamationTransform = exclamationMark_->GetTransform();
    exclamationTransform.position = commonInfo_->transform_.position + commonInfo_->axes_.up * 2.2f;
    exclamationMark_->SetTransform(exclamationTransform);
    exclamationMark_->Update();
    if (commonInfo_->is_detectPlayer_)
    {
        timer_visibleExclamationMark_.Update();
        const float rate = timer_visibleExclamationMark_.GetTimeRate();

        if (rate >= 1.f)
        {
            commonInfo_->is_detectPlayer_ = false;
        }
    }

    // 蛇の"振舞い"による行動全般の管理
    snakeBehaviorMachine_.ManagementBehavior();

    // コライダー更新
    sphere_collision_.center = commonInfo_->transform_.position;
    sphere_detect_.center = commonInfo_->transform_.position;

    // 行列更新
    commonInfo_->transformMatrix_.mat_world = Math::Function::AffinTrans(commonInfo_->transform_, commonInfo_->axes_);

    // 毎フレーム着地フラグをfalseにする。けど着地していたら、ここの処理を通るまではtrueになってる。
    commonInfo_->is_landing_ = false;
    commonInfo_->is_detectEgg_ = false;
    commonInfo_->is_detectPlayer_ = false;
}

void Snake::Draw(void)
{
    // 描画処理
    appearance_->Draw();
    // もし捕まっているなら、!マークは出さないように、ここで終了。
    if (commonInfo_->is_Caged_) { return; }
    // デフォルト表示（対応するテクスチャがそもそもないので、MissingTextureに置き換わる。めっちゃlog出る。）
    //appearance_->Draw(/*"Resources/red1x1.png"*/);
    if (commonInfo_->is_detectPlayer_) { exclamationMark_->Draw(); }
}

void Snake::Finalize(void)
{
    colMPtr_->UnRegister(&sphere_collision_);
    colMPtr_->UnRegister(&sphere_detect_);

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

void Snake::SnakeRobChickenEgg(void)
{
    int32_t eggNum = chickenEggPtr_->eggNum_;
    chickenEggPtr_->eggNum_ = eggNum - 1;
}

void Snake::Caged(Vector3* arg_cagePosPtr)
{
    // 捕まえられたかのフラグをtrue
    commonInfo_->is_Caged_ = true;
    // 蛇の座標をケージ内に固定するための座標ptr
    commonInfo_->pos_cage_ = arg_cagePosPtr;
}

void Snake::RotateDirection(float arg_radian)
{
    //##姿勢の上方向を軸に "arg_radian" 回転する
    // 回転用クオータニオン
    Quaternion rotQ = Math::QuaternionF::MakeAxisAngle(commonInfo_->axes_.up, arg_radian);
    // クオータニオンを使用して、正面ベクトルを回転
    commonInfo_->axes_.forward = Math::QuaternionF::RotateVector(commonInfo_->axes_.forward, rotQ);
    // クオータニオンを使用して、右ベクトルを回転
    commonInfo_->axes_.right = Math::QuaternionF::RotateVector(commonInfo_->axes_.right, rotQ);
}

void Snake::Process_CircleShadow(void)
{
    // 丸影が使用可能なら
    if (circleShadows_num_ >= 0)
    {
        // LightManagerに渡す用のライトタイプ
        LightType type = LightType::CIRCLE_SHADOW;

        // 自分自身の座標
        const Vector3& pos_myself = commonInfo_->transform_.position;
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

void Snake::OnCollision(void)
{
    if (sphere_collision_.GetOther()->GetID() == "gravityArea")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphere_collision_.GetOther());

        // 球状重力エリア内に入ってる場合に行う処理。
        Vector3 center2PlayerVec = sphere_collision_.center - other->center; // 星の中心からプレイヤーまでのベクトル
        // 新しい上ベクトル用に保存。
        commonInfo_->vec3_newUp_ = center2PlayerVec.Normalize();
    }

    if (sphere_collision_.GetOther()->GetID() == "terrainSurface")
    {
        // ジャンプ量
        commonInfo_->velocity_vertical_ = 0.f;
        commonInfo_->is_landing_ = true;

        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphere_collision_.GetOther());


        // めり込み距離を出す (めり込んでいる想定 - 距離）なので結果はマイナス想定？？
        float diff = Vector3(sphere_collision_.center - other->center).Length() - (other->radius + sphere_collision_.radius);

        Vector3 currentPos = commonInfo_->transform_.position;
        //currentPos += player->body_->coordinate_.GetUpVec().ExtractVector3();

        // 正規化された球からプレイヤーまでのベクトル * めり込み距離
        currentPos += commonInfo_->axes_.up.Normalize() * -diff; // ここをマイナス符号で値反転

        commonInfo_->transform_.position = currentPos;
    }

    if (sphere_collision_.GetOther()->GetID() == "snake_col")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphere_collision_.GetOther());

        // 蛇から他の蛇へのベクトル
        const Vector3& snake2OtherSnake = Vector3(other->center - sphere_collision_.center);
        // めり込み距離を出す (二つの半径の和 - 二つの中心点の距離 ）
        float diff = (other->radius + sphere_collision_.radius) - snake2OtherSnake.Length();

        // 現在座標の取得
        Vector3 currentPos = commonInfo_->transform_.position;
        // 蛇から他の蛇への反対方向のベクトル * めり込み距離 ////
        currentPos += -snake2OtherSnake.Normalize() * diff * 1.5f;
        // 座標反映
        commonInfo_->transform_.position = currentPos;
        sphere_collision_.center = commonInfo_->transform_.position;
        sphere_detect_.center = commonInfo_->transform_.position;
    }

    if (sphere_collision_.GetOther()->GetID() == "snakeCage_col")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphere_collision_.GetOther());

        // ケージから蛇へのベクトル
        const Vector3& snakeCageToSnake = Vector3(sphere_collision_.center - other->center);
        // めり込み距離を出す (二つの半径の和 - 二つの中心点の距離 ）
        float diff = (other->radius + sphere_collision_.radius) - snakeCageToSnake.Length();

        // 現在座標の取得
        Vector3 currentPos = commonInfo_->transform_.position;
        // ケージから蛇への反対方向のベクトル * めり込み距離 ////
        currentPos += snakeCageToSnake.Normalize() * diff;
        // 座標反映
        commonInfo_->transform_.position = currentPos;
        sphere_collision_.center = commonInfo_->transform_.position;
        sphere_detect_.center = commonInfo_->transform_.position;
    }

    //if (sphere_collision_.GetOther()->GetID() == "player")
    //{
    //    // 捕獲されたフラグをtrue
    //    isCaptured_ = true;
    //    SetCircleShadowsIsActive(false);
    //}

    if (sphere_collision_.GetOther()->GetID() == "chickenEgg_col")
    {
        // 卵を食べたかどうかのフラグをtrue
        commonInfo_->is_eatChickenEgg_ = true; // snakeBehavior内でfalseにする処理を行う
        // 鶏卵の数を1減らす関数を実行 ※friend関数
        SnakeRobChickenEgg();
    }
}

void Snake::OnTrigger(void)
{
    if (sphere_collision_.GetOther()->GetID() == "snakeCage_col")
    {
        // 既に捕獲済みなら終了
        if (commonInfo_->is_Caged_) { return; }
        // ケージに触れたかどうかのフラグをtrue
        commonInfo_->is_touchCage_ = true;
    }
}

void Snake::OnDetect(void)
{
    if (sphere_detect_.GetOther()->GetID() == "player")
    {
        // 接触相手のコライダー(プレイヤー）を基底クラスから復元。
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphere_detect_.GetOther());
        // 蛇からプレイヤーへの方向ベクトル（プレイヤーの座標 - 蛇の座標）
        const Vector3& vec3_player2Snake = Vector3(other->center - commonInfo_->transform_.position);
        // 蛇からプレイヤーまでの距離が、規定値"kRadius_detectPlayer_"より大きいなら（範囲内にないのなら）終了。
        const float distance = vec3_player2Snake.Length();
        if (distance > SnakeCommonInfomation::kRadius_detectPlayer_) { return; }

        // 移動方向を決定。
        commonInfo_->vec3_toPlayer_ = vec3_player2Snake.Normalize();
        commonInfo_->is_detectPlayer_ = true;
    }

    if (sphere_detect_.GetOther()->GetID() == "chickenEgg_col")
    {
        // 接触相手のコライダー(プレイヤー）を基底クラスから復元。
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphere_detect_.GetOther());
        // 鶏の巣（卵）の座標を記録 #SnakeBehavior::LEAVE_EGGの為
        commonInfo_->pos_chickenEgg_ = other->center;

        // 蛇から卵への方向ベクトル (蛇の座標 - 卵の座標） ※正規化されていない。
        const Vector3& vec3_egg2Snake = Vector3(other->center - commonInfo_->transform_.position);
        // 蛇から卵までの距離が、規定値"kRadius_detectEgg_"より大きいなら（範囲内にないのなら）終了。
        const float distance = vec3_egg2Snake.Length();
        if (distance > SnakeCommonInfomation::kRadius_detectEgg_) { return; }

        // 移動方向を設定。#SnakeBehavior::SNEAKの為
        commonInfo_->vec3_toEgg_ = vec3_egg2Snake.Normalize();
        commonInfo_->is_detectEgg_ = true;
    }
}
