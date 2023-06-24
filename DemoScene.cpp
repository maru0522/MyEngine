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

    obj3d_->coordinate_.SetScale({ 20.f,0.5f,20.f });
    obj3d_->coordinate_.SetPosition({ 0.f,-5.f,0.f });

    lvdPtr_ = LevelData::Load("Resources/e3.json");
    DeployObj(lvdPtr_);
}

void DemoScene::Update(void)
{
    lightGroup_->Update();

    sprite_->Update();
    //obj3d_->Update();
    //obj3d2_->Update();
    //Vector3 rot = obj3d2_->coordinate_.GetRotation();
    //rot.y += 0.05f;
    //obj3d2_->coordinate_.SetRotation(rot);

    for (auto& object : objects_) {
        object->Update();
    }
}

void DemoScene::Draw3d(void)
{
    lightGroup_->Draw();
    //obj3d_->Draw();
    //obj3d2_->Draw();
    //// cubeの画像にスプライトで表示しているものを起用
    ////obj3d_->Draw("Resources/namida.png");
    for (auto& object : objects_) {
        object->Draw();
    }
}


void DemoScene::Draw2dFore(void)
{
}

void DemoScene::Draw2dBack(void)
{
    sprite_->Draw();
}

void DemoScene::Finalize(void)
{
}

void DemoScene::DeployObj(LevelData* lvdPtr)
{
    for (auto& objectData : lvdPtr->objects_) {
        if (objectData.type == "MESH") {
            objects_.emplace_back(new Object3D{ "Resources/model/cube/cube.obj" });
            objects_.back()->coordinate_.SetPosition(objectData.trans);
            objects_.back()->coordinate_.SetRotation(objectData.rot);
            objects_.back()->coordinate_.SetScale(objectData.scale);
        }
        if (objectData.type == "LIGHT") {
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
