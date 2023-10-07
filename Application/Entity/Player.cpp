#include "Player.h"
#include "Input.h"
#include "Quaternion.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"
#include "CollisionManager.h"

Player::Player(CameraManager* camMPtr) : camMPtr_(camMPtr),pbm_(this,PlayerBehavior::IDLE)
{
    CollisionManager::GetInstance()->Register(&sphereCollider_);
    sphereCollider_.SetID("player");
    sphereCollider_.SetOnCollision(std::bind(&Player::OnCollision, this));

    sphereCollider_.radius = kRadius_;

    // 初期位置
    transform_.position = { 0,60,-10 };
    // 初期姿勢
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };

    moveVec_ = { 0,1,0 };
}

Player::~Player(void)
{
    CollisionManager::GetInstance()->UnRegister(&sphereCollider_);
}

void Player::Update(void)
{
    if (isFallHole1_)
    {
        transform_.position = { 0,-15,48 };
        isFallHole1_ = false;
    }
    if (isFallHole2_)
    {
        transform_.position = { 0,15,-48 };
        isFallHole2_ = false;
    }

    // 1Frame遅い描画座標等更新 ** 座標が確定した後に、当たり判定処理で座標を補正するため、1Frame遅らせないとガクつく可能性がある。
    appearance_->GetCoordinatePtr()->mat_world = coordinate_.mat_world;
    appearance_->Update();

    pbm_.ManagementBehavior();

    // コライダー更新
    sphereCollider_.center = transform_.position;

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
    coordinate_.mat_world = Math::Function::AffinTrans(transform_, axes_);

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
    GUI::Space();
    GUI::Space();
    //GUI::Text("forward(current):     [%f,%f,%f]", forwardFromOldAxis.x, forwardFromOldAxis.y, forwardFromOldAxis.z);
    //GUI::Text("right(current):       [%f,%f,%f]", rightFromOldAxis.x, rightFromOldAxis.y, rightFromOldAxis.z);

    ImGui::Text("matrix");
    Matrix4 p = coordinate_.mat_world;
    ImGui::Text("%f, %f, %f, %f", p.m[0][0], p.m[0][1], p.m[0][2], p.m[0][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[1][0], p.m[1][1], p.m[1][2], p.m[1][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[2][0], p.m[2][1], p.m[2][2], p.m[2][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[3][0], p.m[3][1], p.m[3][2], p.m[3][3]);
    GUI::Text("pos(current):         [%f,%f,%f]", transform_.position.x, transform_.position.y, transform_.position.z);
    GUI::Text("rot(current):         [%f,%f,%f]", transform_.rotation.x, transform_.rotation.y, transform_.rotation.z);
    GUI::Text("sca(current):         [%f,%f,%f]", transform_.scale.x, transform_.scale.y, transform_.scale.z);
    GUI::Space();
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

void Player::Move(Vector3& moveVec, Vector3& velocity)
{
    // 入力ベクトル
    Vector3 inputVec{};
    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    inputVec = inputVec.Normalize();

    // カメラ視点のプレイヤー移動ベクトル
    Vector3 pForwardFromCamera = Math::Vec3::Cross(camMPtr_->GetCurrentCamera()->GetAxis3Ptr()->right, axes_.up).Normalize(); // 正面Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(axes_.up, pForwardFromCamera).Normalize(); // 右Vec: cross(p.upVec, pForwardFromCamera)

    // 移動ベクトル
    moveVec += pForwardFromCamera * inputVec.y; // 入力ベクトルに応じて加算
    moveVec += redefinitionPRightFromCamera * inputVec.x;

    // カメラ座標用の値を補正
    SphericalCamera(inputVec);

    // 重力
    jumpVecNorm_ -= kGravity_;

    // ジャンプベクトル
    Vector3 jumpVec{};
    if (KEYS::IsTrigger(DIK_SPACE)) { jumpVecNorm_ = kJumpPower_; }
    jumpVec += axes_.up * jumpVecNorm_;

    // 移動量
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += jumpVec;

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("input:                [%f,%f]", inputVec.x, inputVec.y);
    GUI::Text("move:                 [%f,%f,%f]", moveVec.x, moveVec.y, moveVec.z);
    GUI::Text("pForwardFromCamera:   [%f,%f,%f]", pForwardFromCamera.x, pForwardFromCamera.y, pForwardFromCamera.z);
    //GUI::Text("pRightFromCamera:     [%f,%f,%f]", pRightFromCamera.x, pRightFromCamera.y, pRightFromCamera.z);
    GUI::Text("pRightFromCamera:     [%f,%f,%f]", redefinitionPRightFromCamera.x, redefinitionPRightFromCamera.y, redefinitionPRightFromCamera.z);
    GUI::Text("jumpVecNorm:          [%f]", jumpVecNorm_);
    GUI::Text("kGravity:             [%f]", kGravity_);
    GUI::Space();
    GUI::Space();
    GUI::End();
#endif // _DEBUG
}

void Player::SphericalCamera(Vector3& inputVec)
{
    if (jumpVecNorm_)
    {
        // カメラとプレイヤーの距離
        float dist = (camMPtr_->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - transform_.position).Length();

        // ジャンプ時にカメラの追従が軽減 ≒ 画面の揺れを抑制する目的
        // 内積が規定値未満の時ジャンプを繰り返すとカメラ距離どんどん遠くなっていく不具合が出てる
        current_rad_ = dist;
    }

    // プレイヤーの正面とカメラの正面の内積が "規定値" 未満の時
    // 規定値の値を小さくするほど、プレイヤーが画面中央に近い位置で、カメラの挙動が切り替わる。
    if (axes_.forward.Dot(camMPtr_->GetCurrentCamera()->GetAxis3Ptr()->forward) < 0.7f)
    {
        // カメラとプレイヤーの距離
        float dist = (camMPtr_->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - transform_.position).Length();

        // 該当距離が、本来設定されているプレイヤーとの距離より短い場合、該当距離を設定距離とする。
        if (dist < current_rad_)
        {
            // プレイヤーがカメラ側に向かって移動する際、カメラの座標を固定する意図
            // しかし、現状だとカメラが遠ざかる処理が上手く機能していない為コメントアウト。
            //current_rad_ = dist;
        }
    }
    else
    {
        // 現在距離(cureent_rad_)が、初期距離(default_rad_)より小さい値なら、現在距離を補正する。
        if (current_rad_ < default_rad_)
        {
            current_rad_ += 0.1f;
            //current_rad_ = Math::Ease::EaseInSine(current_rad_, current_rad_, default_rad_);
        }
        else if (current_rad_ > default_rad_)
        {
            current_rad_ -= 0.1f;
        }

        theta_ += 0.02f * inputVec.y;
        phi_ += 0.02f * inputVec.x;
        Math::Function::Loop(theta_, 0.f, 6.28319f);
        Math::Function::Loop(phi_, 0.f, 6.28319f);
    }
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
        //currentPos += player->body_->coordinate_.GetUpVec().ExtractVector3();

        // 正規化された球からプレイヤーまでのベクトル * めり込み距離
        currentPos += axes_.up * -diff; // ここをマイナス符号で値反転

        // 座標を補正
        transform_.position = currentPos;

        // 補正された値で行列を生成
        coordinate_.mat_world = Math::Function::AffinTrans(transform_, axes_);
    }
    if (sphereCollider_.GetOther()->GetID() == "rock")
    {
        // めり込み距離の算出方法が分からん。AABB側の半径どうやって算出するんや。

        // 現在座標
        Vector3 currentPos = transform_.position;

        // 移動した分だけ押し戻すようにする。
        currentPos -= velocity_;

        // 座標を補正
        transform_.position = currentPos;

        // 補正された値で行列を生成
        coordinate_.mat_world = Math::Function::AffinTrans(transform_, axes_);
    }
    if (sphereCollider_.GetOther()->GetID() == "rabbit")
    {
        captureCount_rabbit++;
    }
    if (sphereCollider_.GetOther()->GetID() == "tunnel1")
    {
        isFallHole1_ = true;
    }
    if (sphereCollider_.GetOther()->GetID() == "tunnel2")
    {
        isFallHole2_ = true;
    }
}
