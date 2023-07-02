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

    Vector3 currentPos = player_->body_->coordinate_.GetPosition();
    Vector3 pForwardVec = player_->body_->coordinate_.GetAxisZ();
    Vector3 camDistance = -pForwardVec.normalize() * 5.f;
    if(debugCamFollow_) cameraPtr->eye_ = currentPos + camDistance;

    Vector3 cForwardVec = cameraPtr->GetForwardVec();
    Vector3 rightVec = cameraPtr->GetRightVec();

    if (KEYS::IsDown(DIK_W)) currentPos += cForwardVec;
    if (KEYS::IsDown(DIK_S)) currentPos -= cForwardVec;
    if (KEYS::IsDown(DIK_A)) currentPos -= rightVec;
    if (KEYS::IsDown(DIK_D)) currentPos += rightVec;

    player_->body_->coordinate_.SetPosition(currentPos);

    player_->Update();
    planet_->Update();

    DemoCollision(player_.get(), planet_.get());

    //for (auto& object : objects_) {
    //    object.second->Update();
    //}
    //if(KEYS::IsTrigger(DIK_R)) {
    //    lvdPtr_ = LevelData::Load("Resources/untitled.json");
    //    HotReload(lvdPtr_.get());
    //}

    DebudGui();
}

void DemoScene::Draw3d(void)
{
    lightGroup_->Draw();

    player_->Draw();
    if(debugPlanetDraw_) planet_->Draw();

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

    Vector3 rotate;
    rotate.x = -std::atan2f(player->upVec_.y, player_->upVec_.x);
    rotate.y = -std::atan2f(std::sqrtf(player->upVec_.x * player->upVec_.x + player->upVec_.y * player->upVec_.y), player->upVec_.z);
    rotate.z = -std::asinf(player->upVec_.z / std::sqrtf(player->upVec_.x * player->upVec_.x + player->upVec_.y * player->upVec_.y) + player->upVec_.z * player->upVec_.z);
    player->body_->coordinate_.SetRotation(rotate);

    if (Collision::SphereToSphere(player->sphereCollider_, planet->sphereCollider_))
    {
        Vector3 currentPos = player->body_->coordinate_.GetPosition();
        currentPos += player->upVec_ * 0.1f;

        player->body_->coordinate_.SetPosition(currentPos);
    }
}

void DemoScene::DebudGui(void)
{
    GUI::Begin("demoInfo", { 200,400 });
    GUI::ChildFrameBegin("player", { 300,200 });
    const Vector3& pPos = player_->body_->coordinate_.GetPosition();
    const Vector3& pRot = player_->body_->coordinate_.GetRotation();
    const Vector3& pSca = player_->body_->coordinate_.GetScale();
    ImGui::Text("player");
    ImGui::Text("pos: (%f,%f,%f)", pPos.x, pPos.y, pPos.z);
    ImGui::Text("rot: (%f,%f,%f)", pRot.x, pRot.y, pRot.z);
    ImGui::Text("sca: (%f,%f,%f)", pSca.x, pSca.y, pSca.z);
    GUI::ChildFrameEnd();
    GUI::ChildFrameBegin("camera", { 300,200 });
    const Vector3& cPos = cameraPtr->eye_;
    const Vector3& cRot = cameraPtr->rotation_;
    const Vector3& cUp = cameraPtr->up_;
    ImGui::Text("camera");
    ImGui::Text("pos: (%f,%f,%f)", cPos.x, cPos.y, cPos.z);
    ImGui::Text("rot: (%f,%f,%f)", cRot.x, cRot.y, cRot.z);
    ImGui::Text("up : (%f,%f,%f)", cUp.x, cUp.y, cUp.z);
    if (GUI::ButtonTrg("camera"))
        debugCamFollow_ ?
        debugCamFollow_ = false :
        debugCamFollow_ = true;
    ImGui::Text(debugCamFollow_ ? "debugCamFollow : true" : "debugCanFollow : false");
    GUI::ChildFrameEnd();

    GUI::ChildFrameBegin("other", { 300,200 });
    ImGui::Text("other");
    if (GUI::ButtonTrg("drawPlanet"))
        debugPlanetDraw_ ?
        debugPlanetDraw_ = false :
        debugPlanetDraw_ = true;
    ImGui::Text(debugPlanetDraw_ ? "debugPlanetDraw : true" : "debugPlanetDraw : false");
    GUI::ChildFrameEnd();
    GUI::End();
}
