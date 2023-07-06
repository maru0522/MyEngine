#include "Input.h"
#include "DemoScene.h"
#include "Collision.h"
#include "SimplifyImGui.h"

void DemoScene::Initialize(void)
{
    // カメラの座標設定
    cameraPtr->eye_.z = -20;
    // カメラのデバッグカメラモードをON
    cameraPtr->SetIsDebugMode(true);
    // カメラをマネージャーにセット
    CameraManager::GetInstance()->SetCurrentCamera(cameraPtr.get());

    //sprite_->SetSize({500,500});
    Object3D::SetLightGroup(lightGroup_.get());

    // 使う
    lightGroup_->SetDirLightActive(0, true);
    lightGroup_->SetDirLightColor(0, { 1,1,1 });
    lightGroup_->SetLightDir(0, { 0,-1,0 });

    // json読み込み&配置
    //lvdPtr_ = LevelData::Load("Resources/untitled.json");
    //DeployObj(lvdPtr_.get());
}

void DemoScene::Update(void)
{
    lightGroup_->Update();

    //Vector3 currentPos = player_->body_->coordinate_.GetPosition();
    //Vector3 forwardVec = cameraPtr->GetForwardVec();
    //Vector3 camDistance = forwardVec.normalize() * 5.f;
    ////cameraPtr->eye_ = currentPos - camDistance;

    //Vector3 rightVec = cameraPtr->GetRightVec();

    //if (KEYS::IsDown(DIK_W)) currentPos += forwardVec;
    //if (KEYS::IsDown(DIK_S)) currentPos -= forwardVec;
    //if (KEYS::IsDown(DIK_A)) currentPos -= rightVec;
    //if (KEYS::IsDown(DIK_D)) currentPos += rightVec;

    //player_->body_->coordinate_.SetPosition(currentPos);

    player_->Update();
    planet_->Update();

    DemoCollision(player_.get(), planet_.get());

    if (camFollowDev_) {
        cameraPtr->eye_ = player_->body_->coordinate_.GetPosition() - player_->forwardVec_.normalize() * 8.f;
        cameraPtr->up_ = player_->upVec_;
        cameraPtr->eyeDirection_ = player_->forwardVec_;
    }

    //for (auto& object : objects_) {
    //    object.second->Update();
    //}
    //if(KEYS::IsTrigger(DIK_R)) {
    //    lvdPtr_ = LevelData::Load("Resources/untitled.json");
    //    HotReload(lvdPtr_.get());
    //}

    DebugGui();
}

void DemoScene::Draw3d(void)
{
    lightGroup_->Draw();

    player_->Draw();
    planet_->Draw();

    //for (auto& object : objects_) {
    //    object.second->Draw();
    //}
}


void DemoScene::Draw2dFore(void)
{
}

void DemoScene::Draw2dBack(void)
{
}

void DemoScene::Finalize(void)
{
}

void DemoScene::DeployObj(LevelData* lvdPtr)
{
    for (auto& objectData : lvdPtr->objects_) {
        if (objectData.type == "MESH") {
            objects_.emplace(objectData.name, new Object3D{ "Resources/model/cube/cube.obj" });
            objects_[objectData.name]->coordinate_.SetPosition(objectData.trans);
            objects_[objectData.name]->coordinate_.SetRotation(objectData.rot);
            objects_[objectData.name]->coordinate_.SetScale(objectData.scale);
            objects_[objectData.name]->SetIsInvisible(objectData.isInvisible);
        }
        if (objectData.type == "LIGHT") {
            if (!objectData.isInvisible) lightGroup_->SetPointLightActive(0, true);
            lightGroup_->SetPointLightColor(0, { 1,1,1 });
            lightGroup_->SetPointLightPos(0, objectData.trans);
            lightGroup_->SetPointLightAtten(0, { 0.3f,0.1f,0.1f });
        }
        if (objectData.type == "CAMERA") {
            cameraPtr->eye_ = objectData.trans;
            cameraPtr->rotation_ = objectData.rot;
        }
    }
}

void DemoScene::HotReload(LevelData* lvdPtr)
{
    for (auto& objectData : lvdPtr->objects_) {
        if (objectData.type == "MESH") {
            if (objects_.find(objectData.name) == objects_.end()) {
                objects_.emplace(objectData.name, new Object3D{ "Resources/model/cube/cube.obj" });
                objects_[objectData.name]->coordinate_.SetPosition(objectData.trans);
                objects_[objectData.name]->coordinate_.SetRotation(objectData.rot);
                objects_[objectData.name]->coordinate_.SetScale(objectData.scale);
                objects_[objectData.name]->SetIsInvisible(objectData.isInvisible);
            }
            objects_[objectData.name]->coordinate_.SetPosition(objectData.trans);
            objects_[objectData.name]->coordinate_.SetRotation(objectData.rot);
            objects_[objectData.name]->coordinate_.SetScale(objectData.scale);
            objects_[objectData.name]->SetIsInvisible(objectData.isInvisible);
        }
        if (objectData.type == "LIGHT") {
            objectData.isInvisible ?
                lightGroup_->SetPointLightActive(0, false) :
                lightGroup_->SetPointLightActive(0, true);
            lightGroup_->SetPointLightColor(0, { 1,1,1 });
            lightGroup_->SetPointLightPos(0, objectData.trans);
            lightGroup_->SetPointLightAtten(0, { 0.3f,0.1f,0.1f });
        }
        if (objectData.type == "CAMERA") {
            cameraPtr->eye_ = objectData.trans;
            cameraPtr->rotation_ = objectData.rot;
        }
    }
}

void DemoScene::DemoCollision(Player* player, Planet* planet)
{
    Vector3 center2PlayerVec = player->sphereCollider_.center - planet->sphereCollider_.center;
    player->upVec_ = center2PlayerVec.normalize();

    //if (Collision::SphereToSphere(player->sphereCollider_, planet->sphereCollider_))
    //{
        // めり込み距離を出す (めり込んでいる想定 - 距離）なので結果はマイナス想定？？
        float diff = Vector3(player->sphereCollider_.center - planet->sphereCollider_.center).length() - planet->sphereCollider_.radius - player->sphereCollider_.radius;

        Vector3 currentPos = player->body_->coordinate_.GetPosition();
        //currentPos += player->upVec_ * 0.1f;

        // 正規化された球からプレイヤーまでのベクトル * めり込み距離
        currentPos += center2PlayerVec.normalize() * -diff;// ここをマイナス符号で値反転

        player->body_->coordinate_.SetPosition(currentPos);
    //}
}

void DemoScene::DebugGui(void)
{
    Gui::Begin("Debug", { 300,500 });
    ImGui::Text("camPos : (%f,%f,%f)", cameraPtr->eye_.x, cameraPtr->eye_.y, cameraPtr->eye_.z);
    ImGui::Text("forwardV : (%f,%f,%f)", cameraPtr->eyeDirection_.x, cameraPtr->eyeDirection_.y, cameraPtr->eyeDirection_.z);
    Gui::End();
}
