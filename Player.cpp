#include "Player.h"
#include "Input.h"
#include "CollisionChecker.h"
#include "CollisionManager.h"
#include "SimplifyImGui.h"

Player::Player(void)/* : isGrounded_(false)*/
{
    CollisionManager::GetInstance()->Register(&sphereCollider_);
    sphereCollider_.SetID("player");
    sphereCollider_.SetOnCollision(std::bind(&Player::OnCollision, this));

    sphereCollider_.radius = kRadius_;
    coordinate_.SetPosition({ 0,20,0 }); // 初期位置
    coordinate_.SetAxisForward({ 0,0,1,0 });
    coordinate_.SetAxisRight({ 1,0,0,0 });
    coordinate_.SetAxisUp({ 0,1,0,0 });
    coordinate_.SetIsPriority(false);

    appearance_->GetCoordinatePtr()->SetPosition(coordinate_.GetPosition());
    appearance_->GetCoordinatePtr()->SetAxisForward(coordinate_.GetForwardVec());
    appearance_->GetCoordinatePtr()->SetAxisRight(coordinate_.GetRightVec());
    appearance_->GetCoordinatePtr()->SetAxisUp(coordinate_.GetUpVec());
    appearance_->GetCoordinatePtr()->SetIsPriority(false);
}

void Player::Update(void)
{
    static Quaternion oldForward = coordinate_.GetForwardVec();
    static Quaternion oldRight = coordinate_.GetRightVec();
    static Quaternion oldUp = coordinate_.GetUpVec();

    // --- 1Frame遅い ---

    // 1frame前と現在frameの右ベクトルでの角度を求める
    float theta4Forward = std::acosf(Math::Vec3::Dot(oldRight.ExtractVector3().Normalize(), coordinate_.GetRightVec().ExtractVector3().Normalize()));
    // 今の正面ベクトルと角度から、正面ベクトル(roll)の任意軸回転クォータニオンを作る。
    Quaternion forward4Appearance = Math::QuaternionF::MakeAxisAngle(coordinate_.GetForwardVec().ExtractVector3().Normalize(), theta4Forward);

    // 1frame前と現在frameの上ベクトルでの角度を求める
    float theta4Right = std::acosf(Math::Vec3::Dot(oldUp.ExtractVector3().Normalize(), coordinate_.GetUpVec().ExtractVector3().Normalize()));
    // 今の右ベクトルと角度から、右ベクトル(pitch)の任意軸回転クォータニオンを作る。
    Quaternion right4Appearance = Math::QuaternionF::MakeAxisAngle(coordinate_.GetRightVec().ExtractVector3().Normalize(), theta4Right);

    // 1frame前と現在frameの正面ベクトルで角度を求める
    float theta4Up = std::acosf(Math::Vec3::Dot(oldForward.ExtractVector3().Normalize(), coordinate_.GetForwardVec().ExtractVector3().Normalize()));
    // 今の上ベクトルと角度から、上ベクトル(yaw)の任意軸回転クォータニオンを作る。
    Quaternion up4Appearance = Math::QuaternionF::MakeAxisAngle(coordinate_.GetUpVec().ExtractVector3().Normalize(), theta4Up);

    // 三軸のクォータニオンをセット
    appearance_->GetCoordinatePtr()->SetAxis({ forward4Appearance, right4Appearance, up4Appearance });
    // 更新された座標をセット（1frame遅れ)
    appearance_->GetCoordinatePtr()->SetPosition(coordinate_.GetPosition());
    appearance_->Update();

#ifdef _DEBUG
    GUI::Begin("player", ImVec2{ 300,500 });
    GUI::Text("pos(1frame late):     [%f,%f,%f]", coordinate_.GetPosition().x, coordinate_.GetPosition().y, coordinate_.GetPosition().z);
    GUI::End();
#endif // _DEBUG
    // ------------------

    // 移動量
    Quaternion velocity = Move();

    // 座標更新
    Quaternion currentPos = coordinate_.GetPosition();
    currentPos += velocity;
    coordinate_.SetPosition(currentPos.ExtractVector3());

    // コライダー更新
    sphereCollider_.center = coordinate_.GetPosition();

    // 更新直前の各軸ベクトルの保存
    oldForward = coordinate_.GetForwardVec();
    oldRight = coordinate_.GetRightVec();
    oldUp = coordinate_.GetUpVec();

    // 更新された座標から3方向の軸を再計算
    Quaternion newRight = Math::QuaternionF::CrossVector3Part(coordinate_.GetUpVec(), coordinate_.GetForwardVec()).Normalize();
    coordinate_.SetAxisRight(newRight);
    Quaternion newForward = Math::QuaternionF::CrossVector3Part(coordinate_.GetRightVec(), coordinate_.GetUpVec()).Normalize();
    coordinate_.SetAxisForward(newForward);
    Quaternion newUp = coordinate_.GetUpVec().Vec3Normalize();

    coordinate_.SetAxisUp(newUp);

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("pos(current):         [%f,%f,%f]", currentPos.x, currentPos.y, currentPos.z);
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    //GUI::Text("moveVec:              [%f,%f,%f]", moveVec.x, moveVec.y, moveVec.z);
    //GUI::Text("jumpVec:              [%f,%f,%f]", jumpVec.x, jumpVec.y, jumpVec.z);
    GUI::Text("jumpVecNorm:          [%f]", jumpVecNorm_);
    GUI::Text("kGravity:             [%f]", kGravity_);
    GUI::Space();
    GUI::Space();
    //GUI::Text("forward(1frame late): [%f,%f,%f]", forward.x, forward.y, forward.z);
    GUI::Text("forward(current):     [%f,%f,%f]", newForward.x, newForward.y, newForward.z);
    //GUI::Text("right(1frame late):   [%f,%f,%f]", right.x, right.y, right.z);
    GUI::Text("right(current):       [%f,%f,%f]", newRight.x, newRight.y, newRight.z);
    GUI::Text("up:                   [%f,%f,%f]", newUp.x, newUp.y, newUp.z);
    GUI::Text("up:                   [%f]", newUp.w);
    GUI::End();
#endif // _DEBUG
}

void Player::Draw(void)
{
    // 赤色のテクスチャを適用。（クソ見辛い）
    appearance_->Draw("Resources/red1x1.png");
    // デフォルト表示（対応するテクスチャがそもそもないので、MissingTextureに置き換わる。めっちゃlog出る。）
    //appearance_->Draw(/*"Resources/red1x1.png"*/);
}

Quaternion Player::Move(void)
{
    Quaternion forward = coordinate_.GetForwardVec();
    Quaternion right = coordinate_.GetRightVec();

    // 移動ベクトル
    Quaternion moveVec{};
    if (KEYS::IsDown(DIK_W)) moveVec += forward.Vec3Normalize();
    if (KEYS::IsDown(DIK_S)) moveVec -= forward.Vec3Normalize();
    if (KEYS::IsDown(DIK_A)) moveVec -= right.Vec3Normalize();
    if (KEYS::IsDown(DIK_D)) moveVec += right.Vec3Normalize();

    // 重力
    jumpVecNorm_ -= kGravity_;

    // ジャンプベクトル
    Quaternion jumpVec{};
    if (KEYS::IsTrigger(DIK_SPACE)) { jumpVecNorm_ = kJumpPower_; }
    jumpVec += coordinate_.GetUpVec().ExtractVector3().Normalize() * jumpVecNorm_;

    // 移動量
    Quaternion velocity{};
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += jumpVec;

    return velocity;
}

void Player::OnCollision(void)
{
    if (sphereCollider_.GetColInfo().id == "gravityArea")
    {
        // 本来は球状重力エリア内に入ってる場合に行う処理。
        Vector3 center2PlayerVec = sphereCollider_.center - sphereCollider_.GetColInfo().v;
        coordinate_.SetAxisUp({ center2PlayerVec.Normalize(), coordinate_.GetUpVec().w });
    }
    if (sphereCollider_.GetColInfo().id == "terrainSurface")
    {
        //isGrounded_ = true;
        //jumpVec_ = { 0,0,0 };
        jumpVecNorm_ = 0.f;

        // めり込み距離を出す (めり込んでいる想定 - 距離）なので結果はマイナス想定？？
        float diff = Vector3(sphereCollider_.center - sphereCollider_.GetColInfo().v).Length() - (sphereCollider_.GetColInfo().f + sphereCollider_.radius);

        Vector3 currentPos = coordinate_.GetPosition();
        //currentPos += player->body_->coordinate_.GetUpVec().ExtractVector3();

        // 正規化された球からプレイヤーまでのベクトル * めり込み距離
        currentPos += coordinate_.GetUpVec().ExtractVector3().Normalize() * -diff; // ここをマイナス符号で値反転

        coordinate_.SetPosition(currentPos);
    }
}
