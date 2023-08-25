#include "Input.h"
#include "DemoScene.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"

void DemoScene::Initialize(void)
{
    // �J�����̍��W�ݒ�
    cameraPtr->GetTransformPtr()->position = { 0,0,-70 };
    colCameraPtr->GetTransformPtr()->position = { 0,0,-70 };
    // �J�����̃f�o�b�O�J�������[�h��ON
    cameraPtr->SetIsDebugMode(true);
    colCameraPtr->SetIsDebugMode(true);
    // ���W�v�Z�@���N�H�[�^�j�I���D��
    //cameraPtr->GetCoordinatePtr()->SetIsPriority(false);
    // �J�������}�l�[�W���[�ɃZ�b�g
    /*CameraManager::GetInstance()->SetCurrentCamera(cameraPtr.get());*/
    CameraManager::GetInstance()->SetCurrentCamera(colCameraPtr.get());

    //sprite_->SetSize({500,500});
    Object3D::SetLightGroup(lightGroup_.get());

    // �g��
    lightGroup_->SetDirLightActive(0, true);
    lightGroup_->SetDirLightColor(0, { 1,1,1 });
    lightGroup_->SetLightDir(0, { 0,-1,0 });

    // json�ǂݍ���&�z�u
    //lvdPtr_ = LevelData::Load("Resources/untitled.json");
    //DeployObj(lvdPtr_.get());

    //colCameraPtr->radius_ = 50.f;
    //colCameraPtr->theta_ = 0.f;
    //colCameraPtr->phi_ = 0.f;

    rabbit2_->GetTransformPtr()->position = { 10,60, 20 };
    rabbit3_->GetTransformPtr()->position = { -10,60, 20 };

    rock1_->TransformPtr()->position = { 50,0,0 };
    rock2_->TransformPtr()->position = { 47,12,3 };
    rock3_->TransformPtr()->position = { 45,-6,7 };
    rock4_->TransformPtr()->position = { 45,-15,-5 };
    rock5_->TransformPtr()->position = { 46,7,-6 };
}

void DemoScene::Update(void)
{
    lightGroup_->Update();
    testP_->Update();

    //static float sRadius4Cam{};
    //ImGui::SliderFloat("sRadius4Cam", &sRadius4Cam, 0.f, 200.f);
    //static float sTheta4Cam{};
    ////ImGui::SliderFloat("sTheta4Cam", &sTheta4Cam, 0.f, 3.14159f);
    //ImGui::SliderFloat("sTheta4Cam", &sTheta4Cam, 0.f, 6.28319f);
    //static float sPhi4Cam{ 0.f };
    //ImGui::SliderFloat("sPhi4Cam", &sPhi4Cam, 0.f, 6.28319f);

    ImGui::SliderFloat("sRadius4Cam", &player_->current_rad_, 0.f, 200.f);
    ImGui::SliderFloat("sTheta4Cam", &player_->theta_, 0.f, 6.28319f);
    ImGui::SliderFloat("sPhi4Cam", &player_->phi_, 0.f, 6.28319f);
    // �J���������ʍ��W�n�ŊǗ�����
    Vector3 ppos = player_->GetTransformPtr()->position;

    player_->Update();
    rabbit1_->Update();
    rabbit2_->Update();
    rabbit3_->Update();
    planet_->Update();

    colCameraPtr->SetPlanetCenter(planet_->GetPosition());
    colCameraPtr->CalcAxis3(player_->GetTransformPtr()->position, player_->GetAxis3Ptr()->up.Normalize());

    Matrix4 matWorld{ Math::Mat4::Identity() };
    {
        using namespace Math;

        matWorld *= Mat4::Translate(matWorld, {0,0,-player_->current_rad_ });

        Matrix4 matRotate{ Mat4::Identity() };
        //Matrix4 matRotate = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,-1 };
        matRotate = Mat4::RotationX(player_->theta_) * Mat4::RotationY(player_->phi_);

        matWorld *= matRotate;

        matWorld.m[3][0] += ppos.x;
        matWorld.m[3][1] += ppos.y;
        matWorld.m[3][2] += ppos.z;
    }

    colCameraPtr->GetCoordinatePtr()->mat_world = matWorld;
    testP_->GetCoordinatePtr()->mat_world = matWorld;

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
        // �f�o�b�O�p�̃J�����Ɛؑ�
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
    Matrix4 p = testP_->GetCoordinatePtr()->mat_world;
    ImGui::Text("%f, %f, %f, %f", p.m[0][0], p.m[0][1], p.m[0][2], p.m[0][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[1][0], p.m[1][1], p.m[1][2], p.m[1][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[2][0], p.m[2][1], p.m[2][2], p.m[2][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[3][0], p.m[3][1], p.m[3][2], p.m[3][3]);

    GUI::Space();
    ImGui::Text("followCamera matrix");
    Matrix4 c = colCameraPtr->GetCoordinatePtr()->mat_world;
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

    hole_->GetCoordinatePtr()->mat_world = Math::Function::AffinTrans(Vector3(0,0,0),Vector3(5,52,5),Vector3(1.5708f,0,0));
    hole_->Update();
    house_->GetCoordinatePtr()->mat_world = Math::Function::AffinTrans(Vector3(-15, 50, 15), Vector3(5, 5, 5), Vector3(0.31f, 0.f, 0.23f));
    house_->Update();

    rock1_->Update();
    rock2_->Update();
    rock3_->Update();
    rock4_->Update();
    rock5_->Update();
}

void DemoScene::Draw3d(void)
{
    lightGroup_->Draw();

    player_->Draw();
    rabbit1_->Draw();
    rabbit2_->Draw();
    rabbit3_->Draw();
    if (debugPlanetDraw_) planet_->Draw();
    //testP_->Draw();

    hole_->Draw();
    house_->Draw();
    rock1_->Draw();
    rock2_->Draw();
    rock3_->Draw();
    rock4_->Draw();
    rock5_->Draw();

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

//void DemoScene::DeployObj(LevelData* lvdPtr)
//{
//    using namespace Math::QuaternionF;

    //for (auto& objectData : lvdPtr->objects_) {
        //if (objectData.type == "MESH") {
        //    objects_.emplace(objectData.name, new Object3D{ "Resources/model/cube/cube.obj" });
        //    objects_[objectData.name]->GetCoordinatePtr()->SetPosition(objectData.trans);
        //    //objects_[objectData.name]->coordinate_.SetRotation(EulerToQuaternion(objectData.rot));
        //    objects_[objectData.name]->GetCoordinatePtr()->SetScale(objectData.scale);
        //    objects_[objectData.name]->SetIsInvisible(objectData.isInvisible);
        //}
        //if (objectData.type == "LIGHT") {
        //    if (!objectData.isInvisible) lightGroup_->SetPointLightActive(0, true);
        //    lightGroup_->SetPointLightColor(0, { 1,1,1 });
        //    lightGroup_->SetPointLightPos(0, objectData.trans);
        //    lightGroup_->SetPointLightAtten(0, { 0.3f,0.1f,0.1f });
        //}
        //if (objectData.type == "CAMERA") {
        //    cameraPtr->eye_ = objectData.trans;
        //    cameraPtr->rotation_ = objectData.rot;
        //}
    //}
//}

//void DemoScene::HotReload(LevelData* lvdPtr)
//{
//    using namespace Math::QuaternionF;

    //for (auto& objectData : lvdPtr->objects_) {
        //if (objectData.type == "MESH") {
        //    if (objects_.find(objectData.name) == objects_.end()) {
        //        objects_.emplace(objectData.name, new Object3D{ "Resources/model/cube/cube.obj" });
        //        objects_[objectData.name]->GetCoordinatePtr()->SetPosition(objectData.trans);
        //        //objects_[objectData.name]->coordinate_.SetRotation(EulerToQuaternion(objectData.rot));
        //        objects_[objectData.name]->GetCoordinatePtr()->SetScale(objectData.scale);
        //        objects_[objectData.name]->SetIsInvisible(objectData.isInvisible);
        //    }
        //    objects_[objectData.name]->GetCoordinatePtr()->SetPosition(objectData.trans);
        //    //objects_[objectData.name]->coordinate_.SetRotation(EulerToQuaternion(objectData.rot));
        //    objects_[objectData.name]->GetCoordinatePtr()->SetScale(objectData.scale);
        //    objects_[objectData.name]->SetIsInvisible(objectData.isInvisible);
        //}
        //if (objectData.type == "LIGHT") {
        //    objectData.isInvisible ?
        //        lightGroup_->SetPointLightActive(0, false) :
        //        lightGroup_->SetPointLightActive(0, true);
        //    lightGroup_->SetPointLightColor(0, { 1,1,1 });
        //    lightGroup_->SetPointLightPos(0, objectData.trans);
        //    lightGroup_->SetPointLightAtten(0, { 0.3f,0.1f,0.1f });
        //}
        //if (objectData.type == "CAMERA") {
        //    cameraPtr->eye_ = objectData.trans;
        //    cameraPtr->rotation_ = objectData.rot;
        //}
    //}
//}
