#include "Player.h"
#include "Input.h"
#include "Quaternion.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"

Player::Player(CameraManager* arg_camMPtr, CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr, Planet* arg_planetPtr)
    : camMPtr_(arg_camMPtr), colMPtr_(arg_colMPtr), lightManagerPtr_(arg_lightManagerPtr), planetPtr_(arg_planetPtr) ,pbm_(this,PlayerBehavior::IDLE)
{
    arg_colMPtr->Register(&sphereCollider_);
    sphereCollider_.SetID("player");
    sphereCollider_.SetCallback_onCollision(std::bind(&Player::OnCollision, this));
    sphereCollider_.SetCallback_onTrigger(std::bind(&Player::OnTrigger, this));

    sphereCollider_.radius = kRadius_;

    // 初期位置
    transform_.position = { 0,60,-10 };
    transform_.scale = { 3,3,3 };

    // 初期姿勢
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };

    moveVec_ = { 0,1,0 };
}

Player::~Player(void)
{
    colMPtr_->UnRegister(&sphereCollider_);
}

void Player::Update(void)
{

    // 1Frame遅い描画座標等更新 ** 座標が確定した後に、当たり判定処理で座標を補正するため、1Frame遅らせないとガクつく可能性がある。
    appearance_->GetCoordinatePtr()->mat_world = matTrans_.mat_world;
    appearance_->Update();

    pbm_.ManagementBehavior();
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

    // コライダー更新
    sphereCollider_.center = transform_.position;

    // 丸影が使用可能なら
    if (circleShadows_num_ >= 0)
    {
        // LightManagerに渡す用のライトタイプ
        LightType type = LightType::CIRCLE_SHADOW;

        // 自分自身の座標
        const Vector3& pos_myself = transform_.position;
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

        GUI::Begin("player state");
        GUI::SliderFloat3("atten", atten, 0, 1);
        GUI::End();

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
    matTrans_.mat_world = Math::Function::AffinTrans(transform_, axes_);

    // 着地フラグは毎フレームfalseになるが、着地してるならOnCollisionでtrueになる。
    isLanding_ = false;

#ifdef _DEBUG
    GUI::Begin("Control");
    GUI::Text("Move:WASD");
    GUI::Text("Jump:SPACE");
    GUI::End();

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
    GUI::Text("jumpVecNorm:          [%f]", jumpVecNorm_);
    GUI::Text("kGravity:             [%f]", kGravity_);
    GUI::BlankLine();
    GUI::BlankLine();
    //GUI::Text("forward(current):     [%f,%f,%f]", forwardFromOldAxis.x, forwardFromOldAxis.y, forwardFromOldAxis.z);
    //GUI::Text("right(current):       [%f,%f,%f]", rightFromOldAxis.x, rightFromOldAxis.y, rightFromOldAxis.z);

    ImGui::Text("matrix");
    Matrix4 p = matTrans_.mat_world;
    ImGui::Text("%f, %f, %f, %f", p.m[0][0], p.m[0][1], p.m[0][2], p.m[0][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[1][0], p.m[1][1], p.m[1][2], p.m[1][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[2][0], p.m[2][1], p.m[2][2], p.m[2][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[3][0], p.m[3][1], p.m[3][2], p.m[3][3]);
    GUI::Text("pos(current):         [%f,%f,%f]", transform_.position.x, transform_.position.y, transform_.position.z);
    GUI::Text("rot(current):         [%f,%f,%f]", transform_.rotation.x, transform_.rotation.y, transform_.rotation.z);
    GUI::Text("sca(current):         [%f,%f,%f]", transform_.scale.x, transform_.scale.y, transform_.scale.z);
    GUI::BlankLine();
    GUI::Text("forward(current):     [%f,%f,%f]", axes_.forward.x, axes_.forward.y, axes_.forward.z);
    GUI::Text("right(current):       [%f,%f,%f]", axes_.right.x, axes_.right.y, axes_.right.z);
    GUI::Text("up(current):          [%f,%f,%f]", axes_.up.x, axes_.up.y, axes_.up.z);
    GUI::End();
#endif // _DEBUG
}

void Player::Draw(void)
{
    // 赤色のテクスチャを適用。（クソ見辛い）
    //appearance_->Draw("Resources/red1x1.png");
    // デフォルト表示（対応するテクスチャがそもそもないので、MissingTextureに置き換わる。めっちゃlog出る。）
    if (pbm_.GetStatePtr()->GetCurState() == PlayerBehavior::STOOP || pbm_.GetStatePtr()->GetCurState() == PlayerBehavior::MOVE_STOOP) { appearance_->Draw("Resources/red1x1.png"); }
    else { appearance_->Draw(/*"Resources/red1x1.png"*/); }
}

void Player::OnCollision(void)
{
    if (sphereCollider_.GetOther()->GetID() == "gravityArea")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // 本来は球状重力エリア内に入ってる場合に行う処理。
        //Vector3 center2PlayerVec = sphereCollider_.center - sphereCollider_.GetColInfo().v;
        Vector3 center2PlayerVec = sphereCollider_.center - other->center;
        axes_.up = center2PlayerVec.Normalize();
    }
    if (sphereCollider_.GetOther()->GetID() == "terrainSurface")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // ジャンプする量
        jumpVecNorm_ = 0.f;
        isLanding_ = true;

        // めり込み距離を出す (めり込んでいる想定 - 距離）なので結果はマイナス想定？？
        float diff = Vector3(sphereCollider_.center - other->center).Length() - (other->radius + sphereCollider_.radius);

        Vector3 currentPos = transform_.position;
        //currentPos += player->body_->matTrans_.GetUpVec().ExtractVector3();

        // 正規化された球からプレイヤーまでのベクトル * めり込み距離
        currentPos += axes_.up * -diff; // ここをマイナス符号で値反転

        // 座標を補正
        transform_.position = currentPos;

        // 補正された値で行列を生成
        matTrans_.mat_world = Math::Function::AffinTrans(transform_, axes_);
    }
    if (sphereCollider_.GetOther()->GetID() == "rock")
    {
        // めり込み距離の算出方法が分からん。AABB側の半径どうやって算出するんや。

        // 現在座標
        Vector3 currentPos = transform_.position;

        // 移動した分だけ押し戻すようにする。
        currentPos -= moveVec_;

        // 座標を補正
        transform_.position = currentPos;

        // 補正された値で行列を生成
        matTrans_.mat_world = Math::Function::AffinTrans(transform_, axes_);
    }
    if (sphereCollider_.GetOther()->GetID() == "rabbit")
    {
        captureCount_rabbit++;
    }
    if (sphereCollider_.GetOther()->GetID() == "pipe_pushback")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // プレイヤーから土管方向へのベクトル
        Vector3 vec_player2pipe = (other->center - sphereCollider_.center).Normalize();
        // プレイヤーの座標
        Vector3 currentPos = transform_.position;
        currentPos -= velocity_;
        transform_.position = currentPos;
        sphereCollider_.center = transform_.position;

        // めり込み距離を出す
        float diff = Vector3(sphereCollider_.center - other->center).Length() - (other->radius + sphereCollider_.radius);

        // 正規化された球からプレイヤーまでのベクトル * めり込み距離
        currentPos += -vec_player2pipe * -diff;

        // 座標を補正
        transform_.position = currentPos;

        // 補正された値で行列を生成
        matTrans_.mat_world = Math::Function::AffinTrans(transform_, axes_);
    }
}

void Player::OnTrigger(void)
{
    if (sphereCollider_.GetOther()->GetID() == "coin_contact")
    {
        coinNum_++;
    }
}
