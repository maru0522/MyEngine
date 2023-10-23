#include "Input.h"
#include "DemoScene.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"
#include "Timer.h"
#include "SceneManager.h"

void DemoScene::Initialize(void)
{
    // カメラの設定
    CameraSetUp();

    //sprite_->SetSize({500,500});
    Object3D::SetLightGroup(lightGroup_.get());

    // 平行光源
    lightGroup_->SetDirLightActive(0, true);
    lightGroup_->SetDirLightColor(0, { 1,1,1 });
    lightGroup_->SetLightDir(0, { 0,-1,0 });

    // 点光源
    //lightGroup_->SetPointLightActive(0, true);
    //lightGroup_->SetPointLightColor(0, { 1,1,1 });
    //lightGroup_->SetPointLightAtten(0, { 0.3f,0.1f,0.1f });
    //lightGroup_->SetPointLightPos(0, { 0,70,0 });

    // スポットライト
    //lightGroup_->SetSpotLightActive(0, true);
    //lightGroup_->SetSpotLightColor(0, { 1,1,1 });
    //lightGroup_->SetSpotLightDir(0, { 0,-1,0 });
    //lightGroup_->SetSpotLightFactorAngle(0, { 20.f,30.f });
    //lightGroup_->SetSpotLightAtten(0, { 0,0,0 });
    //lightGroup_->SetSpotLightPos(0, { 0,100,0 });

    // json読み込み&配置
    //lvdPtr_ = LevelData::Load("Resources/untitled.json");
    //DeployObj(lvdPtr_.get());

    //colCameraPtr->radius_ = 50.f;
    //colCameraPtr->theta_ = 0.f;
    //colCameraPtr->phi_ = 0.f;

    hole1_->transform_.position = { 0,0,-48 };
    hole2_->transform_.position = { 0,0,48 };

    rabbit2_->GetTransformPtr()->position = { 10,60, 20 };
    rabbit3_->GetTransformPtr()->position = { -10,60, 20 };

    rock1_->TransformPtr()->position = { 48,18,9 };
    rock2_->TransformPtr()->position = { 49,-23,6 };
    rock3_->TransformPtr()->position = { 48,-6,24 };
    rock4_->TransformPtr()->position = { 49,10,-20 };
    rock5_->TransformPtr()->position = { 48,-12,-15 };

    sticky1_->SetRoot({ 0,50.f,0 });

    pipe1_->SetPosition({ 16,42.5f,20.f });
    pipe1_->SetRotation({ 0.497f,0.f,5.962f });
    pipe1_->SetPartnerPtr(pipe2_.get());
    pipe1_->GetColPushbackPtr()->SetID("pipe_enterInside1");
    pipe2_->SetPosition({ -29.855f,-38.f ,-11.024f });
    pipe2_->SetRotation({ 3.465f,0.f,0.621f });
    pipe2_->SetPartnerPtr(pipe1_.get());
    pipe2_->GetColPushbackPtr()->SetID("pipe_enterInside2");

    coin1_->SetPosition({ 20.f,55.f,0.f });
    coin2_->SetPosition({ 25.f,52.f,0.f });
    coin3_->SetPosition({ 30.f,49.f,0.f });

    coin4_->SetPosition({ 0.f,10.f,51.f });
    coin5_->SetPosition({ 7.5f,7.5f,51.f });
    coin6_->SetPosition({ 10.f,0.f,51.f });
    coin7_->SetPosition({ 7.5f,-7.5f,51.f });
    coin8_->SetPosition({ 0.f,-10.f,51.f });
    coin9_->SetPosition({ -7.5f,-7.5f,51.f });
    coin10_->SetPosition({ -10.f,0.f,51.f });
    coin11_->SetPosition({ -7.5f,7.5f,51.f });

    coin12_->SetPosition({ 0.f,10.f,-51.f });
    coin13_->SetPosition({ 7.5f,7.5f,-51.f });
    coin14_->SetPosition({ 10.f,0.f,-51.f });
    coin15_->SetPosition({ 7.5f,-7.5f,-51.f });
    coin16_->SetPosition({ 0.f,-10.f,-51.f });
    coin17_->SetPosition({ -7.5f,-7.5f,-51.f });
    coin18_->SetPosition({ -10.f,0.f,-51.f });
    coin19_->SetPosition({ -7.5f,7.5f,-51.f });

    // 丸影
    size_t num_ = 0;
    float distAtCoin = 1.5f;
    Vector3 pos_coin = coin1_->GetPosition();
    Vector3 vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
#pragma region 丸影
    // コイン1
    num_ = 0;
    pos_coin = coin1_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.06f,0.01f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 6.f,8.f });

    // コイン2
    num_ = 1;
    pos_coin = coin2_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.06f,0.01f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 6.f,8.f });

    // コイン3
    num_ = 2;
    pos_coin = coin3_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.06f,0.01f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 6.f,8.f });

    // コイン4
    num_ = 3;
    pos_coin = coin4_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    aaaaaaaaaaaaaaaaa_ = pos_coin + vec_coinTpPlanet * distAtCoin;
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.06f,0.01f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン5
    num_ = 4;
    pos_coin = coin5_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン6
    num_ = 5;
    pos_coin = coin6_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン7
    num_ = 6;
    pos_coin = coin7_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン8
    num_ = 7;
    pos_coin = coin8_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン9
    num_ = 8;
    pos_coin = coin9_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン10
    num_ = 9;
    pos_coin = coin10_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン11
    num_ = 10;
    pos_coin = coin11_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン12
    num_ = 11;
    pos_coin = coin12_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン13
    num_ = 12;
    pos_coin = coin13_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン14
    num_ = 13;
    pos_coin = coin14_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン15
    num_ = 14;
    pos_coin = coin15_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン16
    num_ = 15;
    pos_coin = coin16_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン17
    num_ = 16;
    pos_coin = coin17_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン18
    num_ = 17;
    pos_coin = coin18_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });

    // コイン19
    num_ = 18;
    pos_coin = coin19_->GetPosition();
    vec_coinTpPlanet = (planet_->GetPosition() - pos_coin).Normalize();
    lightGroup_->SetCircleShadowActive(num_, true);
    lightGroup_->SetCircleShadowDir(num_, vec_coinTpPlanet);
    lightGroup_->SetCircleShadowCasterPos(num_, pos_coin + vec_coinTpPlanet * distAtCoin);
    lightGroup_->SetCircleShadowDistanceCasterLight(num_, 1.f);
    lightGroup_->SetCircleShadowAtten(num_, { 0.02f,0.1f,0.07f });
    lightGroup_->SetCircleShadowFactorAngle(num_, { 15.f,20.f });
#pragma endregion
}

void DemoScene::Update(void)
{
    static bool isActive = true;
    static Vector3 dl0Dir = { 0.f,-1.f,0.f };
    static Vector3 dl0Color = { 1.f,1.f,1.f };
    GUI::Begin("dirLight0");
    GUI::SliderFloat3(std::string{"dirLight0Dir"}, dl0Dir, 0.f, 1.f);
    GUI::SliderFloat3(std::string{"dirLight0Color  "}, dl0Color, 0, 1);
    GUI::End();

    if (GUI::ButtonTrg("switch dirLight"))
    {
        isActive ? 
            isActive = false :
            isActive = true;
    }

    lightGroup_->SetDirLightActive(0, isActive);
    lightGroup_->SetLightDir(0, dl0Dir);
    lightGroup_->SetDirLightColor(0, dl0Color);

    //static Vector3 pl0Attan = { 0.4f,0.1f,0.05f };
    //static Vector3 pl0Attan = { 0.4f,0.f,0.f };
    //static Vector3 pl0Pos = { 0,100,0 };
    //GUI::Begin("pointLight0");
    //GUI::SliderFloat3(std::string{"pointLight0Attan"}, pl0Attan, 0.f, 1.f);
    //GUI::SliderFloat3(std::string{"pointLight0Pos  "}, pl0Pos, -100.f, 100.f);
    //GUI::End();
    //lightGroup_->SetPointLightAtten(0, pl0Attan);
    //lightGroup_->SetPointLightPos(0, pl0Pos);

    //static Vector2 sl0FA = { 20.f,30.f };
    //static Vector3 sl0Attan = { 0.f,0.f,0.f };
    //static Vector3 sl0Pos = { 0,100,0 };
    //GUI::Begin("spotLight0");
    //GUI::SliderFloat3(std::string{"spotLight0Attan"}, sl0Attan, 0.f, 1.f);
    //GUI::SliderFloat3(std::string{"spotLight0Pos  "}, sl0Pos, -100.f, 100.f);
    //GUI::SliderFloat2(std::string{"spotLight0FactorAngle  "}, sl0FA, 0.f, 180.f);
    //GUI::End();
    //lightGroup_->SetSpotLightAtten(0, sl0Attan);
    //lightGroup_->SetSpotLightPos(0, sl0Pos);
    //lightGroup_->SetSpotLightFactorAngle(0, sl0FA);

    static float cs0DistAtCaster = 1.f;
    static Vector2 cs0FA = { 15.f,20.f };
    static Vector3 cs0Attan = { 0.02f,0.06f,0.01f };
    static Vector3 cs0pos = aaaaaaaaaaaaaaaaa_;
    //static Vector3 cs0dir = { 0,-1,0.f };
    GUI::Begin("circleShadow0");
    GUI::SliderFloat3(std::string{"circleShadow0Pos"}, cs0pos, -50.f, 50.f);
    //GUI::SliderFloat3(std::string{"circleShadow0Dir"}, cs0dir, -1.f, 1.f);
    GUI::SliderFloat3(std::string{"circleShadow0Attan"}, cs0Attan, 0.f, 1.f);
    GUI::SliderFloat2(std::string{"circleShadow0FactorAngle"}, cs0FA, 0.f, 180.f);
    GUI::SliderFloat(std::string{"circleShadow00istanceAtCaster"}, & cs0DistAtCaster, -100.f, 200.f);
    GUI::Text(lightGroup_->GetCircleShadowActive(3) ? "circleshadow[4] is true" : "circleshadow[0] is false");
    GUI::End();
    lightGroup_->SetCircleShadowAtten(3, cs0Attan);
    lightGroup_->SetCircleShadowFactorAngle(3, cs0FA);
    lightGroup_->SetCircleShadowDistanceCasterLight(3, cs0DistAtCaster);
    lightGroup_->SetCircleShadowCasterPos(3, cs0pos);
    //lightGroup_->SetCircleShadowDir(0, cs0dir);

    //Vector3 p2p = (planet_->GetPosition() - player_->GetTransformPtr()->position).Normalize();
    //lightGroup_->SetCircleShadowDir(0, p2p);
    //lightGroup_->SetCircleShadowCasterPos(0, player_->GetTransformPtr()->position + p2p * 20.f);
    //lightGroup_->SetCircleShadowAtten(0, { 0.5f,0.6f,0 });
    //lightGroup_->SetCircleShadowFactorAngle(0, { 0.2f,0.5f });
    //lightGroup_->SetCircleShadowDistanceCasterLight(0, 100.f);

    png_backGround_->Update();

    if (KEYS::IsTrigger(DIK_R)) { SceneManager::GetInstance()->RequestChangeScene(SceneName::TITLE); }
    if (rabbit1_->GetIsCaptured() && rabbit2_->GetIsCaptured() && rabbit3_->GetIsCaptured()) { SceneManager::GetInstance()->RequestChangeScene(SceneName::TITLE); }

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
    // カメラを球面座標系で管理する
    Vector3 ppos = player_->GetTransformPtr()->position;


    // 遠目から惑星を見るカメラに切り替える処理
    const Vector3 kPos_watch_planet = { 0,190,0 };  // 惑星を見るためのカメラの座標（固定）
    static Vector3 pos_playerAtSwitching;           // カメラを切り替えたときの惑星を見るためのカメラの初期座標として切替時のプレイヤーの座標を入れておくもの
    static uint32_t phase_easingHoleCam;            // カメラのイージングの段階
    // イージング用のタイマー
    static FrameTimer phase0_timer_easingHoleCam;   // 初期座標から固定座標へのイージング用タイマー
    static FrameTimer phase1_timer_easingHoleCam;   // 固定座標に滞在する用のタイマー
    static FrameTimer phase2_timer_easingHoleCam;   // 固定座標から、転移先（プレイヤー追従カメラ）の座標へのイージング用タイマー

    phase0_timer_easingHoleCam.Update();
    phase1_timer_easingHoleCam.Update();
    phase2_timer_easingHoleCam.Update();
    float rate{};

    switch (phase_easingHoleCam)
    {
    case 0:
        if (phase0_timer_easingHoleCam.GetIsFinished())
        {
            phase_easingHoleCam++;
            phase1_timer_easingHoleCam.Start(100);
        }
        else
        {
            rate = phase0_timer_easingHoleCam.GetTimeRate();
        }
        break;
    case 1:
        if (phase1_timer_easingHoleCam.GetIsFinished())
        {
            phase_easingHoleCam++;
            phase2_timer_easingHoleCam.Start(50);
        }
        else
        {
            rate = phase1_timer_easingHoleCam.GetTimeRate();
        }
        break;
    case 2:
        if (phase2_timer_easingHoleCam.GetIsFinished())
        {
            phase_easingHoleCam = 3;
        }
        rate = phase2_timer_easingHoleCam.GetTimeRate();
        break;
    }

    if (rate)
    {
        Vector3 pos_camera4Hole;
        if (phase_easingHoleCam == 0)
        {
            pos_camera4Hole.x = Math::Ease::EaseInSine(rate, pos_playerAtSwitching.x, kPos_watch_planet.x);
            pos_camera4Hole.y = Math::Ease::EaseInSine(rate, pos_playerAtSwitching.y, kPos_watch_planet.y);
            pos_camera4Hole.z = Math::Ease::EaseInSine(rate, pos_playerAtSwitching.z, kPos_watch_planet.z);
        }
        else if (phase_easingHoleCam == 1)
        {
            pos_camera4Hole = kPos_watch_planet;
        }
        else if (phase_easingHoleCam == 2)
        {
            pos_camera4Hole.x = Math::Ease::EaseInSine(rate, kPos_watch_planet.x, camera_colPtr_->GetCoordinatePtr()->GetMatPos().x);
            pos_camera4Hole.y = Math::Ease::EaseInSine(rate, kPos_watch_planet.y, camera_colPtr_->GetCoordinatePtr()->GetMatPos().y);
            pos_camera4Hole.z = Math::Ease::EaseInSine(rate, kPos_watch_planet.z, camera_colPtr_->GetCoordinatePtr()->GetMatPos().z);
        }
        camera_4Hole_->GetTransformPtr()->position = pos_camera4Hole;
    }

    // プレイヤーが穴の当たり判定に触れたら
    if (player_->isFallHole1_ || player_->isFallHole2_)
    {
        // イージング用タイマースタート
        phase0_timer_easingHoleCam.Start(50);
        // 穴用カメラの初期座標を、プレイヤー追従カメラの座標で記録
        pos_playerAtSwitching = player_->GetTransformPtr()->position;
        // マネージャーにセット
        CameraManager::GetInstance()->SetCurrentCamera(camera_4Hole_.get());
        phase_easingHoleCam = 0;
    }

    if (phase2_timer_easingHoleCam.GetIsFinished())
    {
        CameraManager::GetInstance()->SetCurrentCamera(camera_colPtr_.get());
    }

    if (KEYS::IsTrigger(DIK_7))
    {
        CameraManager::GetInstance()->SetCurrentCamera(camera_4Hole_.get());
    }
    if (KEYS::IsTrigger(DIK_6))
    {
        CameraManager::GetInstance()->SetCurrentCamera(camera_colPtr_.get());
    }

    static bool isCamDebug{};
    GUI::Begin("debug tab maruyama");
    if (GUI::ButtonTrg("switch camera"))
    {
        // デバッグ用のカメラと切替
        if (isCamDebug)
        {
            isCamDebug = false;
            CameraManager::GetInstance()->SetCurrentCamera(camera_colPtr_.get());
        }
        else
        {
            isCamDebug = true;
            CameraManager::GetInstance()->SetCurrentCamera(camera_debugPtr_.get());
            camera_debugPtr_->SetIsDebugMode(true);
        }
    }

    player_->Update();
    rabbit1_->Update();
    rabbit2_->Update();
    rabbit3_->Update();
    planet_->Update();

    camera_colPtr_->SetPlanetCenter(planet_->GetPosition());
    camera_colPtr_->CalcAxis3(player_->GetTransformPtr()->position, player_->GetAxis3Ptr()->up.Normalize());

    Matrix4 matWorld{ Math::Mat4::Identity() };
    {
        using namespace Math;

        matWorld *= Mat4::Translate(matWorld, { 0,0,-player_->current_rad_ });

        Matrix4 matRotate{ Mat4::Identity() };
        //Matrix4 matRotate = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,-1 };
        matRotate = Mat4::RotationX(player_->theta_) * Mat4::RotationY(player_->phi_);

        matWorld *= matRotate;

        matWorld.m[3][0] += ppos.x;
        matWorld.m[3][1] += ppos.y;
        matWorld.m[3][2] += ppos.z;
    }

    camera_colPtr_->GetCoordinatePtr()->mat_world = matWorld;
    testP_->GetCoordinatePtr()->mat_world = matWorld;

    //for (auto& object : objects_) {
    //    object.second->Update();
    //}
    //if(KEYS::IsTrigger(DIK_R)) {
    //    lvdPtr_ = LevelData::Load("Resources/untitled.json");
    //    HotReload(lvdPtr_.get());
    //}

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
    Matrix4 c = camera_colPtr_->GetCoordinatePtr()->mat_world;
    ImGui::Text("%f, %f, %f, %f", c.m[0][0], c.m[0][1], c.m[0][2], c.m[0][3]);
    ImGui::Text("%f, %f, %f, %f", c.m[1][0], c.m[1][1], c.m[1][2], c.m[1][3]);
    ImGui::Text("%f, %f, %f, %f", c.m[2][0], c.m[2][1], c.m[2][2], c.m[2][3]);
    ImGui::Text("%f, %f, %f, %f", c.m[3][0], c.m[3][1], c.m[3][2], c.m[3][3]);

    GUI::Space();
    ImGui::Text("holeCamera matrix");
    Matrix4 c2 = camera_4Hole_->GetCoordinatePtr()->mat_world;
    ImGui::Text("%f, %f, %f, %f", c2.m[0][0], c2.m[0][1], c2.m[0][2], c2.m[0][3]);
    ImGui::Text("%f, %f, %f, %f", c2.m[1][0], c2.m[1][1], c2.m[1][2], c2.m[1][3]);
    ImGui::Text("%f, %f, %f, %f", c2.m[2][0], c2.m[2][1], c2.m[2][2], c2.m[2][3]);
    ImGui::Text("%f, %f, %f, %f", c2.m[3][0], c2.m[3][1], c2.m[3][2], c2.m[3][3]);

    GUI::Space();
    ImGui::Text("timer_easing");
    GUI::Text("phase:%d", phase_easingHoleCam);
    ImGui::Text("frame0:%f / %d", phase0_timer_easingHoleCam.GetFrameCurrent(), phase0_timer_easingHoleCam.GetFrameMax());
    ImGui::Text("frame1:%f / %d", phase1_timer_easingHoleCam.GetFrameCurrent(), phase1_timer_easingHoleCam.GetFrameMax());
    ImGui::Text("frame2:%f / %d", phase2_timer_easingHoleCam.GetFrameCurrent(), phase2_timer_easingHoleCam.GetFrameMax());
    GUI::Space();

    GUI::End();

    //hole_->GetCoordinatePtr()->mat_world = Math::Function::AffinTrans(Vector3(0,0,0),Vector3(5,52,5),Vector3(1.5708f,0,0));
    hole1_->Update();
    hole2_->Update();
    house_->GetCoordinatePtr()->mat_world = Math::Function::AffinTrans(Vector3(-15, 50, 15), Vector3(5, 5, 5), Vector3(0.31f, 0.f, 0.23f));
    house_->Update();

    rock1_->Update();
    rock2_->Update();
    rock3_->Update();
    rock4_->Update();
    rock5_->Update();

    sticky1_->Update();
    pipe1_->Update();
    pipe2_->Update();

    coin1_->Update();
    coin2_->Update();
    coin3_->Update();
    coin4_->Update();
    coin5_->Update();
    coin6_->Update();
    coin7_->Update();
    coin8_->Update();
    coin9_->Update();
    coin10_->Update();
    coin11_->Update();
    coin12_->Update();
    coin13_->Update();
    coin14_->Update();
    coin15_->Update();
    coin16_->Update();
    coin17_->Update();
    coin18_->Update();
    coin19_->Update();
    if (coin1_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(0, -100); }
    if (coin2_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(1, -100); }
    if (coin3_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(2, -100); }
    if (coin4_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(3, -100); }
    if (coin5_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(4, -100); }
    if (coin6_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(5, -100); }
    if (coin7_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(6, -100); }
    if (coin8_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(7, -100); }
    if (coin9_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(8, -100); }
    if (coin10_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(9, -100); }
    if (coin11_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(10, -100); }
    if (coin12_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(11, -100); }
    if (coin13_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(12, -100); }
    if (coin14_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(13, -100); }
    if (coin15_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(14, -100); }
    if (coin16_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(15, -100); }
    if (coin17_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(16, -100); }
    if (coin18_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(17, -100); }
    if (coin19_->GetIsTaken()) { lightGroup_->SetCircleShadowDistanceCasterLight(18, -100); }

    skydome_->Update();

    GUI::Begin("backGround");
    if (GUI::ButtonTrg("bg_switch"))
    {
        isBG_ ? isBG_ = false :
            isBG_ = true;
    }

    GUI::End();
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

    hole1_->Draw();
    hole2_->Draw();
    house_->Draw();
    rock1_->Draw();
    rock2_->Draw();
    rock3_->Draw();
    rock4_->Draw();
    rock5_->Draw();

    sticky1_->Draw();
    pipe1_->Draw();
    pipe2_->Draw();

    coin1_->Draw();
    coin2_->Draw();
    coin3_->Draw();

    coin4_->Draw();
    //coin5_->Draw();
    //coin6_->Draw();
    //coin7_->Draw();
    //coin8_->Draw();
    //coin9_->Draw();
    //coin10_->Draw();
    //coin11_->Draw();

    //coin12_->Draw();
    //coin13_->Draw();
    //coin14_->Draw();
    //coin15_->Draw();
    //coin16_->Draw();
    //coin17_->Draw();
    //coin18_->Draw();
    //coin19_->Draw();
    skydome_->Draw("Resources/gray1x1.png");

    //for (auto& object : objects_) {
    //    object.second->Draw();
    //}
}


void DemoScene::Draw2dFore(void)
{
}

void DemoScene::Draw2dBack(void)
{
    if (isBG_) png_backGround_->Draw();
}

void DemoScene::Finalize(void)
{
}

void DemoScene::CameraSetUp(void)
{
    //>> カメラの座標設定
    camera_debugPtr_->GetTransformPtr()->position = { 0,0,-70 };         // デバッグカメラの座標
    camera_colPtr_->GetTransformPtr()->position = { 3,172,-3 };          // プレイヤー用カメラの座標
    camera_4Hole_->GetTransformPtr()->position = { 0,190,0 };            // 穴に落ちたとき用カメラの座標
    camera_4Hole_->GetTransformPtr()->rotation = { 1.5725f,-1.2175f,0 }; // 穴に落ちたとき用カメラの回転
    camera_4Hole_->SetIsOldUpdateMethod(true);                           // 穴に落ちたとき用カメラの計算方法を設定


    //>> カメラのデバッグカメラモードをON
    camera_debugPtr_->SetIsDebugMode(true); // デバッグカメラ


    //>> カメラをマネージャーにセット
    /*CameraManager::GetInstance()->SetCurrentCamera(cameraPtr.get());*/
    CameraManager::GetInstance()->SetCurrentCamera(camera_colPtr_.get()); // プレイヤー用カメラを設定
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
