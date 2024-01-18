#include "Player.h"
#include "Input.h"
#include "Quaternion.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"
#include "BehindCamera.h"

Player::Player(CameraManager* arg_camMPtr, CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr, Planet* arg_planetPtr)
    : colMPtr_(arg_colMPtr), lightManagerPtr_(arg_lightManagerPtr), planetPtr_(arg_planetPtr), pbm_(this, PlayerBehavior::IDLE)
{
    // 共通の情報を生成
    commonInfo_ = std::make_shared<Player_CommonInfomation>();
    commonInfo_->camMPtr_ = arg_camMPtr;
    pbm_.GetStatePtr()->CopySharedCommonInfo(commonInfo_);

    arg_colMPtr->Register(&sphereCollider_);
    sphereCollider_.SetID("player");
    sphereCollider_.callback_onCollision_ = std::bind(&Player::OnCollision, this);
    sphereCollider_.callback_onTrigger_ = std::bind(&Player::OnTrigger, this);

    sphereCollider_.radius = commonInfo_->kRadius_;

    // 初期位置
    commonInfo_->transform_.position = { 0,60,-10 };
    commonInfo_->transform_.scale = { 2.2f,2.2f,2.2f };

    // 初期姿勢
    commonInfo_->axes_.forward = { 0,0,1 };
    commonInfo_->axes_.right = { 1,0,0 };
    commonInfo_->axes_.up = { 0,1,0 };

    playerUI_.SetUIPtr(UI::GetInstance());
    playerUI_.SetRabbitCountPtr(&captureCount_rabbit);
    playerUI_.Initialize();
    operateGuideUI_.SetUIPtr(UI::GetInstance());
    operateGuideUI_.SetCamMPtr(commonInfo_->camMPtr_);
    operateGuideUI_.Initialize();
}

Player::~Player(void)
{
    colMPtr_->UnRegister(&sphereCollider_);
}

void Player::Update(void)
{
    UI::GetInstance()->GetUISpritePtr("circle_red")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    UI::GetInstance()->GetUISpritePtr("circle_green")->SetAnchorPoint(Vector2{ 0.5f,0.5f });


    ControlUI();
    playerUI_.Update();
    operateGuideUI_.Update(commonInfo_->matTrans_.mat_world);

    // 1Frame遅い描画座標等更新 ** 座標が確定した後に、当たり判定処理で座標を補正するため、1Frame遅らせないとガクつく可能性がある。
    //appearance_->GetCoordinatePtr()->mat_world *= commonInfo_->camMPtr_->GetCurrentCamera()->GetTransformMatrix().mat_rot;
    appearance_->GetCoordinatePtr()->mat_world = Math::Function::AffinTrans(commonInfo_->transform_, commonInfo_->axes_4model_);
    //appearance_->GetCoordinatePtr()->mat_world *= Math::Mat4::Inverse(commonInfo_->camMPtr_->GetCurrentCamera()->GetMatView());
    appearance_->Update();

    // 新規姿勢の上ベクトルを代入
    commonInfo_->axes_.up = commonInfo_->vec3_newUp_;
    //if (commonInfo_->eventState_ == PlayerEventState::NONE) { pbm_.ManagementBehavior(); }
    pbm_.ManagementBehavior();

    // 現在使用しているカメラが、"BehindCamera_follow_player0"である
    if (commonInfo_->camMPtr_->GetCurrentCamera()->GetId() == "BehindCamera_follow_player0")
    {
        // カメラのptrを取得
        BehindCamera* behindCam = static_cast<BehindCamera*>(commonInfo_->camMPtr_->GetCurrentCamera());

        float distance{};
        if (commonInfo_->is_close_)
        {
            // カメラが近づく用タイマーの更新処理を実行
            commonInfo_->timer_stoop_cameraClose_.Update();
            const float rate = commonInfo_->timer_stoop_cameraClose_.GetTimeRate();
            const float currentDist = behindCam->GetDistance();
            distance = Math::Ease::EaseOutSine(rate, currentDist, commonInfo_->kDist_stoop_cameraClose_);
            behindCam->SetDistance(distance);
        }
        else
        {
            // カメラが離れる用タイマーの更新処理を実行
            commonInfo_->timer_stoop_cameraLeave_.Update();
            const float rate = commonInfo_->timer_stoop_cameraLeave_.GetTimeRate();
            const float currentDist = behindCam->GetDistance();
            distance = Math::Ease::EaseOutSine(rate, currentDist, commonInfo_->kDist_stoop_cameraLeave_);
            behindCam->SetDistance(distance);
        }
    }

#ifdef _DEBUG
    GUI::Begin("Debug");
    static float da;
    if (pbm_.GetStatePtr()->debug_aaaaa_) da = pbm_.GetStatePtr()->debug_aaaaa_;
    GUI::Text("dot_pf2cf:       %f", da);
    static float db;
    if (pbm_.GetStatePtr()->debug_aaaaa_) db = pbm_.GetStatePtr()->debug_bbbbb_;
    GUI::Text("dot_abs_pr2cf:   %f", db);
    static float dc;
    if (pbm_.GetStatePtr()->debug_aaaaa_) dc = pbm_.GetStatePtr()->debug_ccccc_;
    GUI::Text("dot_pf2cf_2:     %f", dc);
    static float dd;
    if (pbm_.GetStatePtr()->debug_aaaaa_) dd = pbm_.GetStatePtr()->debug_ddddd_;
    GUI::Text("dot_abs_pr2cf_2: %f", dd);

    GUI::BlankLine();
    static Vector3 de;
    if (pbm_.GetStatePtr()->debug_eeeee_.IsNonZero()) de = pbm_.GetStatePtr()->debug_eeeee_;
    GUI::Text("vec_pf:          %f,%f,%f", de.x, de.y, de.z);
    static Vector3 df;
    if (pbm_.GetStatePtr()->debug_fffff_.IsNonZero()) df = pbm_.GetStatePtr()->debug_fffff_;
    GUI::Text("vec_cf:          %f,%f,%f", df.x, df.y, df.z);
    static Vector3 dg;
    if (pbm_.GetStatePtr()->debug_ggggg_.IsNonZero()) dg = pbm_.GetStatePtr()->debug_ggggg_;
    GUI::Text("vec_pr:          %f,%f,%f", dg.x, dg.y, dg.z);
    GUI::End();
#endif // _DEBUG

    // コライダー更新
    sphereCollider_.center = commonInfo_->transform_.position;

    // 丸影が使用可能なら
    if (circleShadows_num_ >= 0)
    {
        // LightManagerに渡す用のライトタイプ
        LightType type = LightType::CIRCLE_SHADOW;

        // 自分自身の座標
        const Vector3& pos_myself = commonInfo_->transform_.position;
        // プレイヤーから星までのベクトル
        const Vector3& vec_playerTpPlanet = (planetPtr_->GetPosition() - pos_myself).Normalize();

        // キャスターとプレイヤー自体の距離
        float distAtPlayer = 15.f;
        // 丸影用の光源とキャスターの距離
        float distAtCaster = 3.f;
        // ライトの減衰率の各値
        static Vector3 atten = { 0.01f,0.02f,0.01f };
        // ライトの角度減衰の外値と内値
        Vector2 factorAngle = { 6.f,7.f };

        lightManagerPtr_->SetLightDir(type, circleShadows_num_, vec_playerTpPlanet);
        lightManagerPtr_->SetLightPos(type, circleShadows_num_, pos_myself - vec_playerTpPlanet * distAtPlayer);
        lightManagerPtr_->SetLightDistanceAtCaster(type, circleShadows_num_, distAtCaster);
        lightManagerPtr_->SetLightAtten(type, circleShadows_num_, atten);
        lightManagerPtr_->SetLightFactorAngle(type, circleShadows_num_, factorAngle);
    }

    //// 球面のどの位置にいるかに応じて、正しい姿勢にするために3軸を再計算
    //Vector3 rightFromOldAxis = Math::Vec3::Cross(axes_.up, axes_.forward); // 右ベクトル：(更新された上ベクトル x 古い正面ベクトル)
    //axes_.right = rightFromOldAxis.Normalize();
    //Vector3 forwardFromOldAxis = Math::Vec3::Cross(axes_.right, axes_.up); // 正面ベクトル：(更新された右ベクトル x 更新された上ベクトル)
    //axes_.forward = forwardFromOldAxis.Normalize();

    //// 移動入力があった場合
    //if (moveVec_.IsNonZero())
    //{
    //    // 移動方向を向くような、移動方向に合わせた姿勢にするために右向きベクトルを再計算
    //    Vector3 upFromAxis = axes_.up; // 上ベクトル：(更新された上ベクトルを取得）
    //    Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec_.Normalize()); // 右ベクトル：(更新された上ベクトル x 移動ベクトル（移動方向 ≒ 正面ベクトル))
    //    axes_.right = rightFromMoveVec.Normalize();
    //    axes_.forward = moveVec_.Normalize();
    //}

    // 現在の座標で行列を生成（重力によってめり込んでいる。）　-> めり込み補正はOnCollision()に引継ぎ
    // 計算量を減らしたい場合、コミットID a02ba1f80360bda078a7dbb7ea2e8447064e6e9d を参照
    commonInfo_->matTrans_.mat_world = Math::Function::AffinTrans(commonInfo_->transform_, commonInfo_->axes_);

    // 着地フラグは毎フレームfalseになるが、着地してるならOnCollisionでtrueになる。
    commonInfo_->isLanding_ = false;

#ifdef _DEBUG

    GUI::Begin("player");

    switch (pbm_.GetStatePtr()->GetCurState())
    {
    case PlayerBehavior::IDLE:
        GUI::Text("Behavior: Idle");
        break;
    case PlayerBehavior::MOVE:
        GUI::Text("Behavior: Move");
        break;
    case PlayerBehavior::STOOP:
        GUI::Text("Behavior: STOOP");
        break;
    case PlayerBehavior::MOVE_STOOP:
        GUI::Text("Behavior: MOVE_STOOP");
        break;
    case PlayerBehavior::JUMP:
        GUI::Text("Behavior: JUMP");
        break;
    default:
        GUI::Text("Behavior: Unknown");
        break;
    }
    //GUI::Text("pos(current):         [%f,%f,%f]", currentPos.x, currentPos.y, currentPos.z);
    //GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::Text("jumpVecNorm:          [%f]", commonInfo_->jumpVecNorm_);
    GUI::Text("kGravity:             [%f]", commonInfo_->kGravity_);
    GUI::BlankLine();
    GUI::BlankLine();
    //GUI::Text("forward(current):     [%f,%f,%f]", forwardFromOldAxis.x, forwardFromOldAxis.y, forwardFromOldAxis.z);
    //GUI::Text("right(current):       [%f,%f,%f]", rightFromOldAxis.x, rightFromOldAxis.y, rightFromOldAxis.z);

    ImGui::Text("matrix");
    Matrix4 p = commonInfo_->matTrans_.mat_world;
    ImGui::Text("%f, %f, %f, %f", p.m[0][0], p.m[0][1], p.m[0][2], p.m[0][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[1][0], p.m[1][1], p.m[1][2], p.m[1][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[2][0], p.m[2][1], p.m[2][2], p.m[2][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[3][0], p.m[3][1], p.m[3][2], p.m[3][3]);
    GUI::Text("pos(collision):       [%f,%f,%f]", sphereCollider_.center.x, sphereCollider_.center.y, sphereCollider_.center.z);
    GUI::Text("pos(current):         [%f,%f,%f]", commonInfo_->transform_.position.x, commonInfo_->transform_.position.y, commonInfo_->transform_.position.z);
    GUI::Text("rot(current):         [%f,%f,%f]", commonInfo_->transform_.rotation.x, commonInfo_->transform_.rotation.y, commonInfo_->transform_.rotation.z);
    GUI::Text("sca(current):         [%f,%f,%f]", commonInfo_->transform_.scale.x, commonInfo_->transform_.scale.y, commonInfo_->transform_.scale.z);
    GUI::BlankLine();
    GUI::Text("forward(current):     [%f,%f,%f]", commonInfo_->axes_.forward.x, commonInfo_->axes_.forward.y, commonInfo_->axes_.forward.z);
    GUI::Text("right(current):       [%f,%f,%f]", commonInfo_->axes_.right.x, commonInfo_->axes_.right.y, commonInfo_->axes_.right.z);
    GUI::Text("up(current):          [%f,%f,%f]", commonInfo_->axes_.up.x, commonInfo_->axes_.up.y, commonInfo_->axes_.up.z);
    GUI::End();
#endif // _DEBUG
}

void Player::Draw3d(void)
{
    // 赤色のテクスチャを適用。（クソ見辛い）
    //appearance_->Draw("Resources/red1x1.png");
    // デフォルト表示（対応するテクスチャがそもそもないので、MissingTextureに置き換わる。めっちゃlog出る。）
    if (pbm_.GetStatePtr()->GetCurState() == PlayerBehavior::STOOP || pbm_.GetStatePtr()->GetCurState() == PlayerBehavior::MOVE_STOOP) { appearance_->Draw("Resources/red1x1.png"); }
    else { appearance_->Draw(/*"Resources/red1x1.png"*/); }
}

void Player::Draw2dFore(void)
{
    playerUI_.Draw();
    operateGuideUI_.Draw();
}

void Player::HandOverToBehindCamera(const std::string& arg_camId)
{
    ICamera* ptr_cam = commonInfo_->camMPtr_->GetCamera(arg_camId);
    // カメラが適切か確認
    if (ptr_cam->GetId().starts_with("BehindCamera_") == false) { return; }

    // キャスト
    BehindCamera* ptr_cam_behind = static_cast<BehindCamera*>(ptr_cam);
    ptr_cam_behind->axes_player_ = commonInfo_->axes_;
    ptr_cam_behind->pos_player_ = commonInfo_->transform_.position;
}

void Player::ControlUI(void)
{
    // プレイヤーの正面ベクトル
    const Vector3& vec_pForward = commonInfo_->axes_.forward;
    // カメラの正面ベクトル
    const Vector3& vec_cForward = commonInfo_->camMPtr_->GetCurrentCamera()->GetAxis3().forward;

    const float a = vec_pForward.Dot(vec_cForward);
    const bool is_dotValue_smaller_07f = a < 0.7f;

    static int32_t ftimer{};
    if (is_dotValue_smaller_07f)
    {
        // 何かキー入力を行っているか
        if (Input::Keyboard::IsSomeDown() || Input::XPad::GetLStick().Length())
        {
            // ftimerの値が0より大きいなら、0にする。
            ftimer = (std::min)(ftimer, 0);
            // ftimerの値を減らす。
            ftimer--;
        }
        else
        {
            // ftimerの値が0より小さいなら、0にする
            ftimer = (std::max)(ftimer, 0);
            // ftimerの値を増やす
            ftimer++;
        }
    }

    if (ftimer > 40)
    {
        playerUI_.SetIsVisible(true);
    }
    else if (ftimer < 0)
    {
        playerUI_.SetIsVisible(false);
    }

#ifdef _DEBUG
    GUI::Begin("PlayerUIDEBUG");
    GUI::Text("dot :    %f", a);
    GUI::Text("ftimer : %d", ftimer);
    GUI::Text(playerUI_.GetIsEnd() ? "bool UI : true" : "bool UI : false");
    GUI::End();
#endif // _DEBUG
}

void Player::OnCollision(void)
{

    if (sphereCollider_.GetOther()->GetID() == "gravityArea")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // 本来は球状重力エリア内に入ってる場合に行う処理。
        //Vector3 center2PlayerVec = sphereCollider_.center - sphereCollider_.GetColInfo().v;
        Vector3 center2PlayerVec = sphereCollider_.center - other->center;
        commonInfo_->vec3_newUp_ = center2PlayerVec.Normalize();
    }

    // イベント中は当たり判定スキップ
    if (commonInfo_->eventState_ == PlayerEventState::PLANET_HOLE) { return; }

    if (sphereCollider_.GetOther()->GetID() == "terrainSurface")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // ジャンプする量
        commonInfo_->jumpVecNorm_ = 0.f;
        commonInfo_->isLanding_ = true;

        // めり込み距離を出す (めり込んでいる想定 - 距離）なので結果はマイナス想定？？
        float diff = Vector3(sphereCollider_.center - other->center).Length() - (other->radius + sphereCollider_.radius);

        Vector3 currentPos = commonInfo_->transform_.position;
        //currentPos += player->body_->matTrans_.GetUpVec().ExtractVector3();

        // 正規化された球からプレイヤーまでのベクトル * めり込み距離
        currentPos += commonInfo_->axes_.up * -diff; // ここをマイナス符号で値反転

        // 座標を補正
        commonInfo_->transform_.position = currentPos;

        // 補正された値で行列を生成
        commonInfo_->matTrans_.mat_world = Math::Function::AffinTrans(commonInfo_->transform_, commonInfo_->axes_);
    }
    if (sphereCollider_.GetOther()->GetID() == "rock")
    {
        // めり込み距離の算出方法が分からん。AABB側の半径どうやって算出するんや。

        // 現在座標
        Vector3 currentPos = commonInfo_->transform_.position;

        // 移動した分だけ押し戻すようにする。
        currentPos -= commonInfo_->velocity_;

        // 座標を補正
        commonInfo_->transform_.position = currentPos;

        // 補正された値で行列を生成
        commonInfo_->matTrans_.mat_world = Math::Function::AffinTrans(commonInfo_->transform_, commonInfo_->axes_);
    }
    if (sphereCollider_.GetOther()->GetID() == "pipe_pushback")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // プレイヤーから土管方向へのベクトル
        Vector3 vec_player2pipe = (other->center - sphereCollider_.center).Normalize();
        // プレイヤーの座標
        Vector3 currentPos = commonInfo_->transform_.position;
        currentPos -= commonInfo_->velocity_;
        commonInfo_->transform_.position = currentPos;
        sphereCollider_.center = commonInfo_->transform_.position;

        // めり込み距離を出す
        float diff = Vector3(sphereCollider_.center - other->center).Length() - (other->radius + sphereCollider_.radius);

        // 正規化された球からプレイヤーまでのベクトル * めり込み距離
        currentPos += -vec_player2pipe * -diff;

        // 座標を補正
        commonInfo_->transform_.position = currentPos;

        // 補正された値で行列を生成
        commonInfo_->matTrans_.mat_world = Math::Function::AffinTrans(commonInfo_->transform_, commonInfo_->axes_);
    }
}

void Player::OnTrigger(void)
{
    if (sphereCollider_.GetOther()->GetID() == "coin_contact")
    {
        commonInfo_->coinNum_++;
    }
    if (sphereCollider_.GetOther()->GetID() == "rabbit")
    {
        captureCount_rabbit++;
    }
}
