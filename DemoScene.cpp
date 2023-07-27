#include "Input.h"
#include "DemoScene.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"

void DemoScene::Initialize(void)
{
    // �J�����̍��W�ݒ�
    cameraPtr->GetCoordinatePtr()->SetPosition({ 0,0,-70 });
    colCameraPtr->GetCoordinatePtr()->SetPosition({ 0,0,-70 });
    // �J�����̃f�o�b�O�J�������[�h��ON
    cameraPtr->SetIsDebugMode(true);
    colCameraPtr->SetIsDebugMode(true);
    colCameraPtr->GetCoordinatePtr()->SetIsPriority(false);
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
}

void DemoScene::Update(void)
{
    lightGroup_->Update();
    testP_->Update();

    // *****
    // �J�����̍��W = �v���C���[�̍��W�i�����蔻��ɂ���ďC�����ꂽ�Â����W[1frame�O]�j
    // 
    // �v���C���[�̃A�b�v�f�[�g()
    //     �ړ�����() (�����蔻��ɂ��C�����󂯂ĂȂ��V�K���W[����frame])
    //     �v���C���[�̍��W���s��ɕϊ�[����frame]
    // 
    // �J�����̍��W���s��ɕϊ� (�����蔻��ɂ���ďC�����󂯂��Â����W��ϊ�[1frame�O]�j
    // 
    // �v���C���[�̍��W���C�� (�����蔻��ɂ�鉟���߂�)
    // *****

    static float sCamdist{ 60.f };
    Vector3 goalPos;
    if (debugCamFollow2_)
    {
        colCameraPtr->GetCoordinatePtr()->SetMatWorld(player_->GetCoordinatePtr()->GetMatWorld());
        Vector3 ppos = player_->GetCoordinatePtr()->GetPosition();
        //Vector3 pf = player_->GetCoordinatePtr()->GetForwardVec().Normalize();
        Vector3 pu = player_->GetCoordinatePtr()->GetUpVec().Normalize();
        //Vector3 pf20 = pf * sCamdist;
        Vector3 pu20 = pu * sCamdist;
        //Vector3 cpos = ppos - pf20;
        Vector3 cpos2 = ppos + pu20;
        //goalPos = cpos;
        Axis3 toPlayer = { -player_->GetCoordinatePtr()->GetUpVec().Normalize(), player_->GetCoordinatePtr()->GetRightVec().Normalize(),player_->GetCoordinatePtr()->GetForwardVec().Normalize() };
        //colCameraPtr->GetCoordinatePtr()->SetAxes(toPlayer);
        colCameraPtr->GetCoordinatePtr()->SetAxisForward(-player_->GetCoordinatePtr()->GetUpVec().Normalize());
        colCameraPtr->GetCoordinatePtr()->SetPosition(cpos2);

        testP_->GetCoordinatePtr()->SetMatWorld(player_->GetCoordinatePtr()->GetMatWorld());
        testP_->GetCoordinatePtr()->SetAxes(toPlayer);
        testP_->GetCoordinatePtr()->SetPosition(cpos2);
    }

    player_->Update();
    planet_->Update();

    colCameraPtr->SetPlanetCenter(planet_->GetPosition());

    static float sCamFollowSpeed{ player_->kMoveSpeed_ };
    if (debugCamFollow_) {
        float a{};
        /*a = (player_->GetSphereCollider().center - cameraPtr->GetCoordinatePtr()->GetPosition()).Length();*/
        a = (player_->GetSphereCollider().center - colCameraPtr->GetCoordinatePtr()->GetPosition()).Length();

        // �v���C���[�ƃJ�����Ƃ̋�����kCamDist�����ɂȂ����ꍇ
        if (a < sCamdist - 1)
        {
            // �J�����̐��ʕ����ɁA�v���C���[�̈ړ����x�����J�����ړ��B
            /*cameraPtr->GetCoordinatePtr()->SetPosition(cameraPtr->GetCoordinatePtr()->GetPosition() - cameraPtr->GetCoordinatePtr()->GetForwardVec().Normalize() * player_->kMoveSpeed_);*/
            /*colCameraPtr->GetCoordinatePtr()->SetPosition(colCameraPtr->GetCoordinatePtr()->GetPosition() - colCameraPtr->GetCoordinatePtr()->GetForwardVec().Normalize() * player_->kMoveSpeed_);*/
            colCameraPtr->GetCoordinatePtr()->SetPosition(colCameraPtr->GetCoordinatePtr()->GetPosition() - (player_->GetCoordinatePtr()->GetPosition() - colCameraPtr->GetCoordinatePtr()->GetPosition()).Normalize() * sCamFollowSpeed);
            testP_->GetCoordinatePtr()->SetPosition(colCameraPtr->GetCoordinatePtr()->GetPosition() - (player_->GetCoordinatePtr()->GetPosition() - colCameraPtr->GetCoordinatePtr()->GetPosition()).Normalize() * sCamFollowSpeed);
        }
        // �v���C���[�ƃJ�����Ƃ̋�����kCamDist���傫���ɂȂ����ꍇ
        else if (a > sCamdist + 1)
        {
            /*cameraPtr->GetCoordinatePtr()->SetPosition(cameraPtr->GetCoordinatePtr()->GetPosition() + cameraPtr->GetCoordinatePtr()->GetForwardVec().Normalize() * player_->kMoveSpeed_);*/
            /*colCameraPtr->GetCoordinatePtr()->SetPosition(colCameraPtr->GetCoordinatePtr()->GetPosition() + colCameraPtr->GetCoordinatePtr()->GetForwardVec().Normalize() * player_->kMoveSpeed_);*/
            colCameraPtr->GetCoordinatePtr()->SetPosition(colCameraPtr->GetCoordinatePtr()->GetPosition() + (player_->GetCoordinatePtr()->GetPosition() - colCameraPtr->GetCoordinatePtr()->GetPosition()).Normalize() * sCamFollowSpeed);
            testP_->GetCoordinatePtr()->SetPosition(colCameraPtr->GetCoordinatePtr()->GetPosition() + (player_->GetCoordinatePtr()->GetPosition() - colCameraPtr->GetCoordinatePtr()->GetPosition()).Normalize() * sCamFollowSpeed);
        }

        //cameraPtr->GetCoordinatePtr()->SetPosition(player_->GetCoordinatePtr()->GetPosition() - player_->GetCoordinatePtr()->GetForwardVec().Normalize() * kCamDist_);
        //cameraPtr->GetCoordinatePtr()->SetAxisForward(player_->GetCoordinatePtr()->GetForwardVec().Normalize());
        //cameraPtr->GetCoordinatePtr()->SetAxisRight(player_->GetCoordinatePtr()->GetRightVec().Normalize());
        //cameraPtr->GetCoordinatePtr()->SetAxisUp(player_->GetCoordinatePtr()->GetUpVec().Normalize());
    }

    Vector3 colCamZ{};
    Vector3 colCamX{};
    Vector3 colCamY{};
    if (debugCamFuncFollow_)
    {
        /*cameraPtr->Follow(player_->GetCoordinatePtr()->GetPosPtr());*/
        //colCameraPtr->Follow(player_->GetCoordinatePtr()->GetPosPtr());

        //{

            // �J������z���� (p.pos - c.pos).normalize()
        colCamZ = (player_->GetCoordinatePtr()->GetPosition() - colCameraPtr->GetCoordinatePtr()->GetPosition()).Normalize();
        colCameraPtr->GetCoordinatePtr()->SetAxisForward(colCamZ);

        Vector3 pUpVec = player_->GetCoordinatePtr()->GetUpVec().Normalize();

        // �v���C���[�̏�x�N�g���ƁA�J�����̐��ʃx�N�g���i�J��������v���C���[�ւ̃x�N�g���j�̓��ς� -1�t�߂̂Ƃ�
        const float diff = 0.05f;
        float pUpDotCForward = pUpVec.Dot(colCameraPtr->GetCoordinatePtr()->GetForwardVec().Normalize());

        // �J������x��
        colCamX = player_->GetCoordinatePtr()->GetUpVec().Cross(colCamZ).Normalize();

        // �J�����̏�x�N�g��
        Vector3 cUpVec{};
        if (-1.f + diff >= pUpDotCForward) // -0.95f <= dot(����) 
        {
            // �J�����̏�x�N�g���Z�o
            cUpVec = colCamX.Cross(pUpVec).Normalize();
        }
        
        // �J�������狅�̒��S�����ւ̃x�N�g��
        Vector3 cam2SphereVec = (planet_->GetPosition() - colCameraPtr->GetCoordinatePtr()->GetPosition()).Normalize();
        // �J�������狅�̒��S�����ւ̃x�N�g���̉E�x�N�g��
        Vector3 sphereVerticalRightVec = cUpVec.Cross(cam2SphereVec).Normalize();
        Vector3 sphereVerticakUpVec = sphereVerticalRightVec.Cross(cam2SphereVec).Normalize();

        // �J�������狅�̒��S�����ւ̃x�N�g���̏�x�N�g���� pUpVec�Ƃ̓���
        float verticalUpDotCam2Sphere = sphereVerticakUpVec.Dot(pUpVec);
        // �l��0�����i�J�������猩�ăv���C���[����ʂ̉������ɍs�����Ƃ��j
        if (verticalUpDotCam2Sphere < 0) 
        {
            // �O�ό��ʂ����x�N�g���̏ꍇ���]���ĉE�x�N�g���ɂ���
            colCamX = -colCamX;
        }
        colCameraPtr->GetCoordinatePtr()->SetAxisRight(colCamX);
        // �J������y��
        colCamY = colCamZ.Cross(colCamX).Normalize();
        colCameraPtr->GetCoordinatePtr()->SetAxisUp(colCamY);

        Axis3 cam = { colCamZ,colCamX,colCamY };
        testP_->GetCoordinatePtr()->SetAxes(cam);
        //}
    }
    else
    {
        /*cameraPtr->UnFollow();*/
        //colCameraPtr->UnFollow();
    }


    //for (auto& object : objects_) {
    //    object.second->Update();
    //}
    //if(KEYS::IsTrigger(DIK_R)) {
    //    lvdPtr_ = LevelData::Load("Resources/untitled.json");
    //    HotReload(lvdPtr_.get());
    //}

    DebudGui();

    static bool isCamDebug{};

    GUI::Begin("debug tab maruyama");
    if (GUI::ButtonTrg("switch camera"))
    {
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

    ImGui::SliderFloat("camDist", &sCamdist, 0.f, 100.f);
    ImGui::InputFloat("sCamFollowSpeed", &sCamFollowSpeed);
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
    ImGui::Text("z %f,%f,%f", colCamZ.x, colCamZ.y, colCamZ.z);
    ImGui::Text("x %f,%f,%f", colCamX.x, colCamX.y, colCamX.z);
    ImGui::Text("y %f,%f,%f", colCamY.x, colCamY.y, colCamY.z);
    GUI::End();

}

void DemoScene::Draw3d(void)
{
    lightGroup_->Draw();

    player_->Draw();
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

void DemoScene::DebudGui(void)
{
    //using namespace Math::Function;
    //GUI::Begin("demoInfo", { 200,400 });
    //GUI::ChildFrameBegin("player", { 400,100 });
    ////const Vector3& pPos = player_->GetCoordinatePtr()->GetPosition();
    ////const Quaternion& pRot = player_->body_->coordinate_.GetRotation();
    ////const Vector3& pSca = player_->GetCoordinatePtr()->GetScale();
    //ImGui::Text("player");
    ////ImGui::Text("pos: (%f,%f,%f)", pPos.x, pPos.y, pPos.z);
    ////ImGui::Text("sca: (%f,%f,%f)", pSca.x, pSca.y, pSca.z);
    //ImGui::Text("%s", player_->GetPartnerId().c_str());
    ////ImGui::Text("isHit:(%d)", player_->GetIsHit());
    ////ImGui::Text("rot(qua): (%f,%f,%f,%f)", pRot.x, pRot.y, pRot.z, pRot.w);
    //GUI::ChildFrameEnd();
    //GUI::ChildFrameBegin("camera", { 400,140 });
    //const Vector3& cPos = colCameraPtr->GetCoordinatePtr()->GetPosition();
    //const Vector3& cRot = colCameraPtr->GetCoordinatePtr()->GetRotation();
    //const Vector3& cUp = colCameraPtr->GetCoordinatePtr()->GetMatAxisY();
    //ImGui::Text("camera");
    //ImGui::Text("pos: (%f,%f,%f)", cPos.x, cPos.y, cPos.z);
    //ImGui::Text("up : (%f,%f,%f)", cUp.x, cUp.y, cUp.z);
    //ImGui::Text("rot(rad): (%f,%f,%f)", cRot.x, cRot.y, cRot.z);
    //ImGui::Text("rot(deg): (%f,%f,%f)", ToDegree(cRot.x), ToDegree(cRot.y), ToDegree(cRot.z));
    ////Vector3 f = cameraPtr->GetCoordinatePtr()->GetForwardVec().ExtractVector3();
    ////Vector3 u = cameraPtr->GetCoordinatePtr()->GetUpVec().ExtractVector3();
    ////ImGui::Text("forwardV: (%f,%f,%f)", f.x, f.y, f.z);
    ////ImGui::Text("upV: (%f,%f,%f)", u.x, u.y, u.z);
    //if (GUI::ButtonTrg("camera"))
    //    debugCamFollow_ ?
    //    debugCamFollow_ = false :
    //    debugCamFollow_ = true;
    //ImGui::Text(debugCamFollow_ ? "debugCamFollow : true" : "debugCanFollow : false");
    //GUI::ChildFrameEnd();

    //GUI::ChildFrameBegin("testP", { 400,100 });
    //GUI::Text("pos: (%f,%f,%f)", testP_->GetCoordinatePtr()->GetPosition().x, testP_->GetCoordinatePtr()->GetPosition().y, testP_->GetCoordinatePtr()->GetPosition().z);
    //GUI::ChildFrameEnd();

    //GUI::ChildFrameBegin("other", { 400,200 });
    //ImGui::Text("other");
    //if (GUI::ButtonTrg("camFollow"))
    //    debugCamFollow_ ?
    //    debugCamFollow_ = false :
    //    debugCamFollow_ = true;
    //ImGui::Text(debugCamFollow_ ? "debugCamFollow : true" : "debugCamFollow : false");

    //if (GUI::ButtonTrg("drawPlanet"))
    //    debugPlanetDraw_ ?
    //    debugPlanetDraw_ = false :
    //    debugPlanetDraw_ = true;
    //ImGui::Text(debugPlanetDraw_ ? "debugPlanetDraw : true" : "debugPlanetDraw : false");

    //if (GUI::ButtonTrg("camFuncFollow"))
    //    debugCamFuncFollow_ ?
    //    debugCamFuncFollow_ = false :
    //    debugCamFuncFollow_ = true;
    //ImGui::Text(debugCamFuncFollow_ ? "debugCamFuncFollow_ : true" : "debugCamFuncFollow_ : false");
    ////GUI::Space();
    ////Vector3 origin(0.f, 0.f, 0.f);
    ////Vector3 point(10.f, 10.f, 10.f);
    ////Vector3 result(origin - point);
    ////float resultf(result.Length());
    ////ImGui::Text("result: (%f)",resultf);
    //GUI::ChildFrameEnd();
    //GUI::End();
}
