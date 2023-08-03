#include "Input.h"
#include "DemoScene.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"

void DemoScene::Initialize(void)
{
    // カメラの座標設定
    cameraPtr->GetCoordinatePtr()->SetPosition({ 0,0,-70 });
    colCameraPtr->GetCoordinatePtr()->SetPosition({ 0,0,-70 });
    // カメラのデバッグカメラモードをON
    cameraPtr->SetIsDebugMode(true);
    colCameraPtr->SetIsDebugMode(true);
    colCameraPtr->GetCoordinatePtr()->SetIsPriority(true);
    // 座標計算法をクォータニオン優先
    //cameraPtr->GetCoordinatePtr()->SetIsPriority(false);
    // カメラをマネージャーにセット
    /*CameraManager::GetInstance()->SetCurrentCamera(cameraPtr.get());*/
    CameraManager::GetInstance()->SetCurrentCamera(colCameraPtr.get());

    //sprite_->SetSize({500,500});
    Object3D::SetLightGroup(lightGroup_.get());

    // 使う
    lightGroup_->SetDirLightActive(0, true);
    lightGroup_->SetDirLightColor(0, { 1,1,1 });
    lightGroup_->SetLightDir(0, { 0,-1,0 });

    // json読み込み&配置
    //lvdPtr_ = LevelData::Load("Resources/untitled.json");
    //DeployObj(lvdPtr_.get());

    //colCameraPtr->radius_ = 50.f;
    //colCameraPtr->theta_ = 0.f;
    //colCameraPtr->phi_ = 0.f;
    testP_->GetCoordinatePtr()->SetAxes(Axis3{ Vector3(0,0,-1),Vector3(-1,0,0),Vector3(0,1,0) });
    testP_->GetCoordinatePtr()->SetIsPriority(true);
}

void DemoScene::Update(void)
{
    lightGroup_->Update();
    testP_->Update();

    static float sRadius4Cam{};
    ImGui::SliderFloat("sRadius4Cam", &sRadius4Cam, 0.f, 200.f);
    static float sTheta4Cam{};
    //ImGui::SliderFloat("sTheta4Cam", &sTheta4Cam, 0.f, 3.14159f);
    ImGui::SliderFloat("sTheta4Cam", &sTheta4Cam, 0.f, 6.28319f);
    static float sPhi4Cam{ 0.f };
    ImGui::SliderFloat("sPhi4Cam", &sPhi4Cam, 0.f, 6.28319f);
    static float sTheta4Cam2{};
    ImGui::SliderFloat("sTheta4Cam2", &sTheta4Cam2, 0.f, 3.14159f);
    static float sPhi4Cam2{ 3.14159f };
    ImGui::SliderFloat("sPhi4Cam2", &sPhi4Cam2, 0.f, 6.28319f);
    // カメラを球面座標系で管理する
    Vector3 ppos = player_->GetCoordinatePtr()->GetPosition();

    player_->Update();
    rabbit1_->Update();
    planet_->Update();

    colCameraPtr->SetPlanetCenter(planet_->GetPosition());


    Matrix4 matWorld{ Math::Mat4::Identity() };
    {
        using namespace Math;

        matWorld *= Mat4::Translate(matWorld, {0,0,sRadius4Cam });

        Matrix4 matRotate{ Mat4::Identity() };
        matRotate = Mat4::RotationX(sTheta4Cam) * Mat4::RotationY(sPhi4Cam);

        matWorld *= matRotate;

        matWorld.m[3][0] += ppos.x;
        matWorld.m[3][1] += ppos.y;
        matWorld.m[3][2] += ppos.z;
    }

    colCameraPtr->GetCoordinatePtr()->SetMatWorld(matWorld);
    testP_->GetCoordinatePtr()->SetMatWorld(matWorld);

    //for (auto& object : objects_) {
    //    object.second->Update();
    //}
    //if(KEYS::IsTrigger(DIK_R)) {
    //    lvdPtr_ = LevelData::Load("Resources/untitled.json");
    //    HotReload(lvdPtr_.get());
    //}

    static bool isCamDebug{};

    GUI::Begin("debug tab maruyama");
    if (GUI::ButtonTrg("switch camera"))
    {
        // デバッグ用のカメラと切替
        if (isCamDebug)
        {
            isCamDebug = false;
            CameraManager::GetInstance()->SetCurrentCamera(colCameraPtr.get());
        }
        else
        {
            isCamDebug = true;
            CameraManager::GetInstance()->SetCurrentCamera(cameraPtr.get());
            cameraPtr->SetIsDebugMode(true);
        }
    }

    GUI::Text(isCamDebug ? "true" : "false");
    //ImGui::SliderFloat("camDist", &sCamdist, 0.f, 100.f);
    //ImGui::InputFloat("sCamFollowSpeed", &sCamFollowSpeed);
    GUI::Space();
    ImGui::Text("dummyp matrix");
    Matrix4 p = testP_->GetCoordinatePtr()->GetMatWorld();
    ImGui::Text("%f, %f, %f, %f", p.m[0][0], p.m[0][1], p.m[0][2], p.m[0][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[1][0], p.m[1][1], p.m[1][2], p.m[1][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[2][0], p.m[2][1], p.m[2][2], p.m[2][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[3][0], p.m[3][1], p.m[3][2], p.m[3][3]);

    GUI::Space();
    ImGui::Text("followCamera matrix");
    Matrix4 c = colCameraPtr->GetCoordinatePtr()->GetMatWorld();
    ImGui::Text("%f, %f, %f, %f", c.m[0][0], c.m[0][1], c.m[0][2], c.m[0][3]);
    ImGui::Text("%f, %f, %f, %f", c.m[1][0], c.m[1][1], c.m[1][2], c.m[1][3]);
    ImGui::Text("%f, %f, %f, %f", c.m[2][0], c.m[2][1], c.m[2][2], c.m[2][3]);
    ImGui::Text("%f, %f, %f, %f", c.m[3][0], c.m[3][1], c.m[3][2], c.m[3][3]);

    GUI::Space();
    ImGui::Text("colCamVec");
    //ImGui::Text("z %f,%f,%f", colCamZ.x, colCamZ.y, colCamZ.z);
    //ImGui::Text("x %f,%f,%f", colCamX.x, colCamX.y, colCamX.z);
    //ImGui::Text("y %f,%f,%f", colCamY.x, colCamY.y, colCamY.z);
    GUI::End();

}

void DemoScene::Draw3d(void)
{
    lightGroup_->Draw();

    player_->Draw();
    rabbit1_->Draw();
    if (debugPlanetDraw_) planet_->Draw();
    //testP_->Draw();

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
    using namespace Math::QuaternionF;

    for (auto& objectData : lvdPtr->objects_) {
        if (objectData.type == "MESH") {
            objects_.emplace(objectData.name, new Object3D{ "Resources/model/cube/cube.obj" });
            objects_[objectData.name]->GetCoordinatePtr()->SetPosition(objectData.trans);
            //objects_[objectData.name]->coordinate_.SetRotation(EulerToQuaternion(objectData.rot));
            objects_[objectData.name]->GetCoordinatePtr()->SetScale(objectData.scale);
            objects_[objectData.name]->SetIsInvisible(objectData.isInvisible);
        }
        if (objectData.type == "LIGHT") {
            if (!objectData.isInvisible) lightGroup_->SetPointLightActive(0, true);
            lightGroup_->SetPointLightColor(0, { 1,1,1 });
            lightGroup_->SetPointLightPos(0, objectData.trans);
            lightGroup_->SetPointLightAtten(0, { 0.3f,0.1f,0.1f });
        }
        //if (objectData.type == "CAMERA") {
        //    cameraPtr->eye_ = objectData.trans;
        //    cameraPtr->rotation_ = objectData.rot;
        //}
    }
}

void DemoScene::HotReload(LevelData* lvdPtr)
{
    using namespace Math::QuaternionF;

    for (auto& objectData : lvdPtr->objects_) {
        if (objectData.type == "MESH") {
            if (objects_.find(objectData.name) == objects_.end()) {
                objects_.emplace(objectData.name, new Object3D{ "Resources/model/cube/cube.obj" });
                objects_[objectData.name]->GetCoordinatePtr()->SetPosition(objectData.trans);
                //objects_[objectData.name]->coordinate_.SetRotation(EulerToQuaternion(objectData.rot));
                objects_[objectData.name]->GetCoordinatePtr()->SetScale(objectData.scale);
                objects_[objectData.name]->SetIsInvisible(objectData.isInvisible);
            }
            objects_[objectData.name]->GetCoordinatePtr()->SetPosition(objectData.trans);
            //objects_[objectData.name]->coordinate_.SetRotation(EulerToQuaternion(objectData.rot));
            objects_[objectData.name]->GetCoordinatePtr()->SetScale(objectData.scale);
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
        //if (objectData.type == "CAMERA") {
        //    cameraPtr->eye_ = objectData.trans;
        //    cameraPtr->rotation_ = objectData.rot;
        //}
    }
}
