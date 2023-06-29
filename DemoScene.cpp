#include "DemoScene.h"
#include "Input.h"

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

    // json読み込み&配置
    //lvdPtr_ = LevelData::Load("Resources/untitled.json");
    //DeployObj(lvdPtr_.get());
}

void DemoScene::Update(void)
{
    lightGroup_->Update();

    //for (auto& object : objects_) {
    //    object.second->Update();
    //}
    //if(KEYS::IsTrigger(DIK_R)) {
    //    lvdPtr_ = LevelData::Load("Resources/untitled.json");
    //    HotReload(lvdPtr_.get());
    //}
}

void DemoScene::Draw3d(void)
{
    lightGroup_->Draw();

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
