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
    coordinate_.SetPosition({ 0,60,0 }); // 初期位置
    coordinate_.SetAxisForward({ 0,0,1 });
    coordinate_.SetAxisRight({ 1,0,0 });
    coordinate_.SetAxisUp({ 0,1,0 });
    coordinate_.SetIsPriority(false);

    appearance_->GetCoordinatePtr()->SetPosition(coordinate_.GetPosition());
    appearance_->GetCoordinatePtr()->SetAxisForward(coordinate_.GetForwardVec());
    appearance_->GetCoordinatePtr()->SetAxisRight(coordinate_.GetRightVec());
    appearance_->GetCoordinatePtr()->SetAxisUp(coordinate_.GetUpVec());
    appearance_->GetCoordinatePtr()->SetIsPriority(false);
}

void Player::Update(void)
{
    // --- 1Frame遅い ---
    appearance_->GetCoordinatePtr()->SetAxisUp(coordinate_.GetUpVec().Normalize());
    appearance_->GetCoordinatePtr()->SetAxisForward(coordinate_.GetForwardVec().Normalize());
    appearance_->GetCoordinatePtr()->SetAxisRight(coordinate_.GetRightVec().Normalize());
    appearance_->GetCoordinatePtr()->SetPosition(coordinate_.GetPosition());
    appearance_->Update();

#ifdef _DEBUG
    GUI::Begin("player", ImVec2{ 300,500 });
    GUI::Text("pos(1frame late):     [%f,%f,%f]", coordinate_.GetPosition().x, coordinate_.GetPosition().y, coordinate_.GetPosition().z);
    GUI::End();
#endif // _DEBUG
    // ------------------

    // 移動量
    Vector3 velocity = Move();

    // 座標更新
    Vector3 currentPos = coordinate_.GetPosition();
    currentPos += velocity;
    coordinate_.SetPosition(currentPos);

    // コライダー更新
    sphereCollider_.center = coordinate_.GetPosition();


    // 更新された各軸から3方向の軸を再計算
    Vector3 newRight = Math::Vec3::Cross(coordinate_.GetUpVec().Normalize(), coordinate_.GetForwardVec().Normalize()).Normalize();
    coordinate_.SetAxisRight(newRight);
    Vector3 newForward = Math::Vec3::Cross(coordinate_.GetRightVec().Normalize(), coordinate_.GetUpVec().Normalize());
    coordinate_.SetAxisForward(newForward);
    Vector3 newUp = coordinate_.GetUpVec().Normalize();
    coordinate_.SetAxisUp(newUp);

    // 入力ベクトル **これ移動ベクトルと両方やってんのめっちゃ冗長に感じる。
    Vector2 inputVec{};
    inputVec.x += KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y += KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    // 入力されたベクトルとの角度を算出
    //** 角度算出は入力があった時のみ。
    float rad{};
    if (inputVec.IsNonZero())
    {
        rad = std::acosf(Math::Vec2::Dot(Vector2{ 0,1 }.Normalize(), inputVec.Normalize())); // {0,1}とinputVecの角度
        constexpr float pi = Math::Function::ToRadian(180.f);
        if (inputVec.x < 0) rad = pi + (pi - rad); // 180度を超えた場合の処理。

        constexpr float epsilon = 0.00001f;
        // 角度が0度および180度以外の場合
        if (Math::Function::FixEpsilon(rad, 0.f, epsilon) != 0.f && Math::Function::FixEpsilon(rad, 3.141592f, epsilon) != 3.141592f)
        {
            // 正面ベクトルと右ベクトルを上ベクトルを軸に角度分回転
            Quaternion rotAxisUp = Math::QuaternionF::MakeAxisAngle(coordinate_.GetUpVec().Normalize(), -rad); // 上ベクトルを軸に回転させた状態を表すクォータニオン ** -radなのは左手系の回転方向は左ねじなのに対し、角度を右ねじ想定で算出したため。
            Vector3 rotedForward = Math::QuaternionF::RotateVector(coordinate_.GetForwardVec().Normalize(), rotAxisUp);
            Vector3 rotedRight = Math::QuaternionF::RotateVector(coordinate_.GetRightVec().Normalize(), rotAxisUp);
            coordinate_.SetAxisForward(rotedForward);
            coordinate_.SetAxisRight(rotedRight);
        }
    }

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
    GUI::Text("rad:                  [%f]", rad);
    GUI::Text("inputVec:             [%f,%f]", inputVec.x, inputVec.y);
    GUI::End();
#endif // _DEBUG
}

void Player::Draw(void)
{
    // 赤色のテクスチャを適用。（クソ見辛い）
    //appearance_->Draw("Resources/red1x1.png");
    // デフォルト表示（対応するテクスチャがそもそもないので、MissingTextureに置き換わる。めっちゃlog出る。）
    appearance_->Draw(/*"Resources/red1x1.png"*/);
}

Vector3 Player::Move(void)
{
    Vector3 forward = coordinate_.GetForwardVec().Normalize();
    Vector3 right = coordinate_.GetRightVec().Normalize();

    // 移動ベクトル
    Vector3 moveVec{};
    if (KEYS::IsDown(DIK_W)) moveVec += forward;
    if (KEYS::IsDown(DIK_S)) moveVec -= forward;
    if (KEYS::IsDown(DIK_A)) moveVec -= right;
    if (KEYS::IsDown(DIK_D)) moveVec += right;

    // 重力
    jumpVecNorm_ -= kGravity_;

    // ジャンプベクトル
    Vector3 jumpVec{};
    if (KEYS::IsTrigger(DIK_SPACE)) { jumpVecNorm_ = kJumpPower_; }
    jumpVec += coordinate_.GetUpVec().Normalize() * jumpVecNorm_;

    // 正面ベクトル更新
    //coordinate_.SetAxisForward(moveVec.Normalize());

    // 移動量
    Vector3 velocity{};
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
        coordinate_.SetAxisUp(center2PlayerVec.Normalize());
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
        currentPos += coordinate_.GetUpVec().Normalize() * -diff; // ここをマイナス符号で値反転

        coordinate_.SetPosition(currentPos);
    }
}
