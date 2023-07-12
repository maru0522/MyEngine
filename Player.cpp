#include "Player.h"
#include "Input.h"
#include "CollisionChecker.h"
#include "CollisionManager.h"
#include "SimplifyImGui.h"

Player::Player(void)
{
    CollisionManager::GetInstance()->Register(&sphereCollider_);

    sphereCollider_.radius = kRadius_;
    coordinate_.SetPosition({ 0,20,0 }); // 初期位置
    coordinate_.SetAxisForward({ 0,0,1,0 });
    coordinate_.SetAxisRight({ 1,0,0,0 });
    coordinate_.SetAxisUp({ 0,1,0,0 });
    coordinate_.SetIsPriority(true);

    appearance_->GetCoordinatePtr()->SetPosition(coordinate_.GetPosition());
    appearance_->GetCoordinatePtr()->SetAxisForward(coordinate_.GetForwardVec());
    appearance_->GetCoordinatePtr()->SetAxisRight(coordinate_.GetRightVec());
    appearance_->GetCoordinatePtr()->SetAxisUp(coordinate_.GetUpVec());
    appearance_->GetCoordinatePtr()->SetIsPriority(true);
}

void Player::Update(void)
{
    // --- 1Frame遅い ---
    OnCollision();
    appearance_->SetCoordinate(coordinate_);
    appearance_->Update();
    // ------------------

    sphereCollider_.center = coordinate_.GetPosition();

    Quaternion right = Math::QuaternionF::CrossVector3Part(coordinate_.GetUpVec(), coordinate_.GetForwardVec()).Normalize();
    coordinate_.SetAxisRight(right);
    Quaternion forward = Math::QuaternionF::CrossVector3Part(coordinate_.GetRightVec(), coordinate_.GetUpVec()).Normalize();
    coordinate_.SetAxisForward(forward);
    coordinate_.SetAxisUp(coordinate_.GetUpVec().ExtractVector3().Normalize());

    // 座標計算
    Vector3 gravity = -coordinate_.GetUpVec().ExtractVector3();
    gravity *= 0.4f;

    Vector3 moveVec = { 0.f,0.f,0.f };
    if (KEYS::IsDown(DIK_W)) moveVec += forward.ExtractVector3();
    if (KEYS::IsDown(DIK_S)) moveVec -= forward.ExtractVector3();
    if (KEYS::IsDown(DIK_A)) moveVec -= right.ExtractVector3();
    if (KEYS::IsDown(DIK_D)) moveVec += right.ExtractVector3();

    Vector3 velocity = { 0.f,0.f,0.f };
    if (KEYS::IsDown(DIK_SPACE))
    {
        //  じゃんぷ
        velocity += coordinate_.GetUpVec().ExtractVector3().Normalize() * kJumpPower_;
    }
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += gravity;

    Vector3 currentPos = coordinate_.GetPosition();
    currentPos += velocity;
    coordinate_.SetPosition(currentPos);
}

void Player::Draw(void)
{
    appearance_->Draw("Resources/red1x1.png");
}

void Player::OnCollision(void)
{
    if (sphereCollider_.GetIsHit())
    {
        GUI::Begin("phit");
        ImGui::Text("Hit!");
        GUI::End();
        sphereCollider_.SetIsHit(false);

        // 本来は球状重力エリア内に入ってる場合に行う処理。
        Vector3 center2PlayerVec = sphereCollider_.center - sphereCollider_.GetColInfo().v;
        coordinate_.SetAxisUp(center2PlayerVec.Normalize());

        // めり込み距離を出す (めり込んでいる想定 - 距離）なので結果はマイナス想定？？
        float diff = Vector3(sphereCollider_.center - sphereCollider_.GetColInfo().v).Length() - sphereCollider_.GetColInfo().f - sphereCollider_.radius;

        Vector3 currentPos = coordinate_.GetPosition();
        //currentPos += player->body_->coordinate_.GetUpVec().ExtractVector3();

        // 正規化された球からプレイヤーまでのベクトル * めり込み距離
        currentPos += center2PlayerVec.Normalize() * -diff; // ここをマイナス符号で値反転

        coordinate_.SetPosition(currentPos);
    }
}
