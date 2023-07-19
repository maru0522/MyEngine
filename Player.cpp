#include "Player.h"
#include "Input.h"
#include "Quaternion.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"
#include "CollisionManager.h"

Player::Player(CameraManager* camMPtr) : camMPtr_(camMPtr)
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
    Vector3 inputVec{};
    Vector3 moveVec{};
    Vector3 velocity{};
    Move(inputVec, moveVec, velocity);

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

    // 移動ベクトルと今のプレイヤーの正面ベクトルとの角度
    float rad = -std::acosf(Math::Vec3::Dot(newForward.Normalize(), moveVec.Normalize())); // 左手系では回転が左ねじのためマイナスをつけて時計回り回転に
    // 入力ベクトルで角度が±のどちらか判別できる。
    if (inputVec.x < 0) rad = -rad; // 反時計回り回転に

    // 上向きベクトル軸のrad回転した状態を表すクォータニオン
    Quaternion rotAxisup = Math::QuaternionF::MakeAxisAngle(coordinate_.GetUpVec().Normalize(), rad);
    newForward = Math::QuaternionF::RotateVector(newForward, rotAxisup);
    newRight = Math::QuaternionF::RotateVector(newRight, rotAxisup);

    // 見た目の正面と右ベクトルを回転
    // **更新は次にUpdate()が呼ばれたときの先頭で行われるので1freme遅い
    //appearance_->GetCoordinatePtr()->SetAxisForward(newForward);
    //appearance_->GetCoordinatePtr()->SetAxisRight(newRight);

    //    // 入力ベクトル **これ移動ベクトルと両方やってんのめっちゃ冗長に感じる。
    //    Vector2 inputVec{};
    //    inputVec.x += KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    //    inputVec.y += KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    //    // 入力されたベクトルとの角度を算出
    //    //** 角度算出は入力があった時のみ。
    //    float rad{};
    //    if (inputVec.IsNonZero())
    //    {
    //        Vector3 curForward = coordinate_.GetForwardVec().Normalize();
    //        Vector2 forwardVec2d = Vector2{ curForward.x,curForward.y }.Normalize();
    //
    //        // 傾いた状態での正面ベクトルと入力ベクトルとの角度が取れてれば（内積の180°とかの諸問題なく）要らない処理
    //#pragma region 本来ならいらない処理
    //        // 2次元化した正面ベクトルと{0,1}ベクトルとの角度
    //        rad = std::acosf(Math::Vec2::Dot(Vector2{ 0,1 }, inputVec.Normalize()));
    //        // 2次元化した正面ベクトルと入力ベクトルを回転
    //        Vector2 rotedForwardVec2d = Math::Vec2::RotateVector(forwardVec2d, rad);
    //        Vector2 rotedInputVec = Math::Vec2::RotateVector(inputVec.Normalize(), rad);
    //
    //        // 回転後の、2次元化した正面ベクトルと入力ベクトルとの角度
    //        rad = std::acosf(Math::Vec2::Dot(rotedForwardVec2d.Normalize(), rotedInputVec.Normalize()));
    //#pragma endregion
    //
    //        //rad = std::acosf(Math::Vec2::Dot(Vector2{ 0,1 }.Normalize(), inputVec.Normalize())); // {0,1}とinputVecの角度
    //        constexpr float pi = Math::Function::ToRadian(180.f);
    //        if (inputVec.x < 0) rad = pi + (pi - rad); // 180度を超えた場合の処理。
    //
    //        constexpr float epsilon = 0.00001f;
    //        // 角度が0度および180度以外の場合
    //        if (Math::Function::FixEpsilon(rad, 0.f, epsilon) != 0.f && Math::Function::FixEpsilon(rad, 3.141592f, epsilon) != 3.141592f)
    //        {
    //            // 正面ベクトルと右ベクトルを上ベクトルを軸に角度分回転
    //            Quaternion rotAxisUp = Math::QuaternionF::MakeAxisAngle(coordinate_.GetUpVec().Normalize(), -rad); // 上ベクトルを軸に回転させた状態を表すクォータニオン ** -radなのは左手系の回転方向は左ねじなのに対し、角度を右ねじ想定で算出したため。
    //            Vector3 rotedForward = Math::QuaternionF::RotateVector(coordinate_.GetForwardVec().Normalize(), rotAxisUp);
    //            Vector3 rotedRight = Math::QuaternionF::RotateVector(coordinate_.GetRightVec().Normalize(), rotAxisUp);
    //            coordinate_.SetAxisForward(rotedForward);
    //            coordinate_.SetAxisRight(rotedRight);
    //        }
    //    }

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
    //GUI::Text("rad:                  [%f]", rad);
    //GUI::Text("inputVec:             [%f,%f]", inputVec.x, inputVec.y);
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

void Player::Move(Vector3& inputVec, Vector3& moveVec, Vector3& velocity)
{
    // 入力ベクトル
    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);

    // カメラ視点のプレイヤー移動ベクトル
    Vector3 pForwardFromCamera = Math::Vec3::Cross(camMPtr_->GetCurrentCamera()->GetCoordinatePtr()->GetRightVec().Normalize(), coordinate_.GetUpVec().Normalize()).Normalize(); // 正面Vec: cross(camera.rightVec, p.upVec)
    // 1行下のpRightFromCameraをコメントアウトするという事は、絶対にカメラが追従し、プレイヤーは星の縁に立てないものとする（カメラから見て縁に立っているように見えることは絶対ないものとする。）
    //Vector3 pRightFromCamera = Math::Vec3::Cross(coordinate_.GetUpVec().Normalize(), camMPtr_->GetCurrentCamera()->GetCoordinatePtr()->GetForwardVec().Normalize()); // 右Vec: cross(p.upVec, camera.forwardVec)
    //Vector3 pFFC = Math::Vec3::Cross()

    // pForwardFromCameraから再定義されたプレイヤー移動ベクトルの右
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(coordinate_.GetUpVec().Normalize(), pForwardFromCamera).Normalize();

    // 移動ベクトル
    moveVec += pForwardFromCamera * inputVec.y; // 入力ベクトルに応じて加算
    //moveVec += pRightFromCamera * inputVec.x;
    moveVec += redefinitionPRightFromCamera * inputVec.x;

    //Vector3 forward = coordinate_.GetForwardVec().Normalize();
    //Vector3 right = coordinate_.GetRightVec().Normalize();

    //// 移動ベクトル
    //Vector3 moveVec{};
    //if (KEYS::IsDown(DIK_W)) moveVec += forward;
    //if (KEYS::IsDown(DIK_S)) moveVec -= forward;
    //if (KEYS::IsDown(DIK_A)) moveVec -= right;
    //if (KEYS::IsDown(DIK_D)) moveVec += right;

    // 重力
    jumpVecNorm_ -= kGravity_;

    // ジャンプベクトル
    Vector3 jumpVec{};
    if (KEYS::IsTrigger(DIK_SPACE)) { jumpVecNorm_ = kJumpPower_; }
    jumpVec += coordinate_.GetUpVec().Normalize() * jumpVecNorm_;

    // 正面ベクトル更新
    //coordinate_.SetAxisForward(moveVec.Normalize());

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
