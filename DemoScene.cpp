#include "Input.h"
#include "DemoScene.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"

void DemoScene::Initialize(void)
{
    // カメラの座標設定
    /*cameraPtr->GetCoordinatePtr()->SetPosition({ 0,0,-70 });*/
    colCameraPtr->GetCoordinatePtr()->SetPosition({ 0,0,-70 });
    // カメラのデバッグカメラモードをON
    /*cameraPtr->SetIsDebugMode(true);*/
    colCameraPtr->SetIsDebugMode(true);
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
}

void DemoScene::Update(void)
{
    lightGroup_->Update();

    player_->Update();
    planet_->Update();

    colCameraPtr->SetPlanetCenter(planet_->GetPosition());

    if (debugCamFollow_) {
        float a{};
        /*a = (player_->GetSphereCollider().center - cameraPtr->GetCoordinatePtr()->GetPosition()).Length();*/
        a = (player_->GetSphereCollider().center - colCameraPtr->GetCoordinatePtr()->GetPosition()).Length();

        // プレイヤーとカメラとの距離がkCamDist未満になった場合
        if (a < kCamDist_ - 1)
        {
            // カメラの正面方向に、プレイヤーの移動速度だけカメラ移動。
            /*cameraPtr->GetCoordinatePtr()->SetPosition(cameraPtr->GetCoordinatePtr()->GetPosition() - cameraPtr->GetCoordinatePtr()->GetForwardVec().Normalize() * player_->kMoveSpeed_);*/
            /*colCameraPtr->GetCoordinatePtr()->SetPosition(colCameraPtr->GetCoordinatePtr()->GetPosition() - colCameraPtr->GetCoordinatePtr()->GetForwardVec().Normalize() * player_->kMoveSpeed_);*/
            colCameraPtr->GetCoordinatePtr()->SetPosition(colCameraPtr->GetCoordinatePtr()->GetPosition() - (player_->GetCoordinatePtr()->GetPosition() - colCameraPtr->GetCoordinatePtr()->GetPosition()).Normalize() * player_->kMoveSpeed_);
        }
        // プレイヤーとカメラとの距離がkCamDistより大きくになった場合
        else if (a > kCamDist_ + 1)
        {
            /*cameraPtr->GetCoordinatePtr()->SetPosition(cameraPtr->GetCoordinatePtr()->GetPosition() + cameraPtr->GetCoordinatePtr()->GetForwardVec().Normalize() * player_->kMoveSpeed_);*/
            /*colCameraPtr->GetCoordinatePtr()->SetPosition(colCameraPtr->GetCoordinatePtr()->GetPosition() + colCameraPtr->GetCoordinatePtr()->GetForwardVec().Normalize() * player_->kMoveSpeed_);*/
            colCameraPtr->GetCoordinatePtr()->SetPosition(colCameraPtr->GetCoordinatePtr()->GetPosition() + (player_->GetCoordinatePtr()->GetPosition() - colCameraPtr->GetCoordinatePtr()->GetPosition()).Normalize() * player_->kMoveSpeed_);

        }

        //cameraPtr->GetCoordinatePtr()->SetPosition(player_->GetCoordinatePtr()->GetPosition() - player_->GetCoordinatePtr()->GetForwardVec().Normalize() * kCamDist_);
        //cameraPtr->GetCoordinatePtr()->SetAxisForward(player_->GetCoordinatePtr()->GetForwardVec().Normalize());
        //cameraPtr->GetCoordinatePtr()->SetAxisRight(player_->GetCoordinatePtr()->GetRightVec().Normalize());
        //cameraPtr->GetCoordinatePtr()->SetAxisUp(player_->GetCoordinatePtr()->GetUpVec().Normalize());
    }

    if (debugCamFuncFollow_)
    {
        /*cameraPtr->Follow(player_->GetCoordinatePtr()->GetPosPtr());*/
        colCameraPtr->Follow(player_->GetCoordinatePtr()->GetPosPtr());
    }
    else
    {
        /*cameraPtr->UnFollow();*/
        colCameraPtr->UnFollow();
    }

    if (debugCamFollow2_)
    {
        colCameraPtr->GetCoordinatePtr()->SetAxisForward(player_->GetCoordinatePtr()->GetForwardVec().Normalize());
        colCameraPtr->GetCoordinatePtr()->SetAxisRight(player_->GetCoordinatePtr()->GetRightVec().Normalize());
        colCameraPtr->GetCoordinatePtr()->SetAxisUp(player_->GetCoordinatePtr()->GetUpVec().Normalize());

        colCameraPtr->GetCoordinatePtr()->SetPosition(player_->GetCoordinatePtr()->GetPosition() - player_->GetCoordinatePtr()->GetForwardVec().Normalize() * 20.f);
    }

    testP_->Update();
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
    if (debugPlanetDraw_) planet_->Draw();
    testP_->Draw();

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

void DemoScene::DebudGui(void)
{
    using namespace Math::Function;
    GUI::Begin("demoInfo", { 200,400 });
    GUI::ChildFrameBegin("player", { 400,100 });
    //const Vector3& pPos = player_->GetCoordinatePtr()->GetPosition();
    //const Quaternion& pRot = player_->body_->coordinate_.GetRotation();
    //const Vector3& pSca = player_->GetCoordinatePtr()->GetScale();
    ImGui::Text("player");
    //ImGui::Text("pos: (%f,%f,%f)", pPos.x, pPos.y, pPos.z);
    //ImGui::Text("sca: (%f,%f,%f)", pSca.x, pSca.y, pSca.z);
    ImGui::Text("%s", player_->GetPartnerId().c_str());
    //ImGui::Text("isHit:(%d)", player_->GetIsHit());
    //ImGui::Text("rot(qua): (%f,%f,%f,%f)", pRot.x, pRot.y, pRot.z, pRot.w);
    GUI::ChildFrameEnd();
    GUI::ChildFrameBegin("camera", { 400,140 });
    const Vector3& cPos = colCameraPtr->GetCoordinatePtr()->GetPosition();
    const Vector3& cRot = colCameraPtr->GetCoordinatePtr()->GetRotation();
    const Vector3& cUp = colCameraPtr->GetCoordinatePtr()->GetMatAxisY();
    ImGui::Text("camera");
    ImGui::Text("pos: (%f,%f,%f)", cPos.x, cPos.y, cPos.z);
    ImGui::Text("up : (%f,%f,%f)", cUp.x, cUp.y, cUp.z);
    ImGui::Text("rot(rad): (%f,%f,%f)", cRot.x, cRot.y, cRot.z);
    ImGui::Text("rot(deg): (%f,%f,%f)", ToDegree(cRot.x), ToDegree(cRot.y), ToDegree(cRot.z));
    //Vector3 f = cameraPtr->GetCoordinatePtr()->GetForwardVec().ExtractVector3();
    //Vector3 u = cameraPtr->GetCoordinatePtr()->GetUpVec().ExtractVector3();
    //ImGui::Text("forwardV: (%f,%f,%f)", f.x, f.y, f.z);
    //ImGui::Text("upV: (%f,%f,%f)", u.x, u.y, u.z);
    if (GUI::ButtonTrg("camera"))
        debugCamFollow_ ?
        debugCamFollow_ = false :
        debugCamFollow_ = true;
    ImGui::Text(debugCamFollow_ ? "debugCamFollow : true" : "debugCanFollow : false");
    GUI::ChildFrameEnd();

    GUI::ChildFrameBegin("testP", { 400,100 });
    GUI::Text("pos: (%f,%f,%f)", testP_->GetCoordinatePtr()->GetPosition().x, testP_->GetCoordinatePtr()->GetPosition().y, testP_->GetCoordinatePtr()->GetPosition().z);
    GUI::ChildFrameEnd();

    GUI::ChildFrameBegin("other", { 400,200 });
    ImGui::Text("other");
    if (GUI::ButtonTrg("camFollow"))
        debugCamFollow_ ?
        debugCamFollow_ = false :
        debugCamFollow_ = true;
    ImGui::Text(debugCamFollow_ ? "debugCamFollow : true" : "debugCamFollow : false");

    if (GUI::ButtonTrg("drawPlanet"))
        debugPlanetDraw_ ?
        debugPlanetDraw_ = false :
        debugPlanetDraw_ = true;
    ImGui::Text(debugPlanetDraw_ ? "debugPlanetDraw : true" : "debugPlanetDraw : false");

    if (GUI::ButtonTrg("camFuncFollow"))
        debugCamFuncFollow_ ?
        debugCamFuncFollow_ = false :
        debugCamFuncFollow_ = true;
    ImGui::Text(debugCamFuncFollow_ ? "debugCamFuncFollow_ : true" : "debugCamFuncFollow_ : false");
    //GUI::Space();
    //Vector3 origin(0.f, 0.f, 0.f);
    //Vector3 point(10.f, 10.f, 10.f);
    //Vector3 result(origin - point);
    //float resultf(result.Length());
    //ImGui::Text("result: (%f)",resultf);
    GUI::ChildFrameEnd();
    GUI::End();
}
