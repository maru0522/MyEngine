#include "GameManager.h"
#include "SceneManager.h"
#include "Input.h"

void GameManager::SetAllPtrs(CollisionManager* arg_colMPtr, CameraManager* arg_camMPtr, LightManager* arg_lightMPtr, FigureUI* arg_figureUIPtr, UI* arg_uiPtr)
{
    colMPtr_ = arg_colMPtr;
    camMPtr_ = arg_camMPtr;
    lightMPtr_ = arg_lightMPtr;
    figureUIPtr_ = arg_figureUIPtr;
    uiPtr_ = arg_uiPtr;
}

void GameManager::Initialize()
{
    //>> オブジェクト
    // カメラの設定
    CamerasSetting();
    // 惑星の初期化
    gamePlanet_.Initialize(colMPtr_);
    // プレイヤーの初期化
    player_.Initialize(camMPtr_, colMPtr_, lightMPtr_, &gamePlanet_);
    player_.SetupLightCircleShadows();
    // 鶏卵の初期化
    chickenEgg_.Initialize(colMPtr_, lightMPtr_, &gamePlanet_);
    // 蛇
    for (auto& snake : snakes_) { snake.Initialize(colMPtr_, lightMPtr_, &gamePlanet_, &chickenEgg_); }
    snakes_[1].GetTransformPtr()->position = { 10,60, 20 };
    snakes_[2].GetTransformPtr()->position = { -10,60, 20 };
    // 蛇用ケージ
    for (auto& cage : snakeCages_) { cage.Initialize(colMPtr_, lightMPtr_, &gamePlanet_); }
    snakeCages_[0].SetPosition(Vector3{ 20,-60, 30 });
    snakeCages_[1].SetPosition(Vector3{ -40,20, 25 });
    snakeCages_[2].SetPosition(Vector3{ -40,50, -70 });


    // コインリストの初期化とコインの配置
    coinList_.Initialize(colMPtr_);
    coinList_.DeployCoin("Resources/externalText/coinsLayout.txt", &gamePlanet_, lightMPtr_);

    //>> イベント
    event_startTutorial_.Initialize(camMPtr_, &player_);
    event_endTutorial_.Initialize(camMPtr_);
    event_tutorialPlanetHole_.Initialize(colMPtr_, camMPtr_, &player_);
    // startTutorialから起動する。 endTutorialは終了時,tutorialPlanetHoleは、更新処理内で起動する
    event_startTutorial_.SetIsExecute(true);

    // タイマーをUIに登録
    numForInvaseTime_ = kTimer_limit_;
    figureUIPtr_->Register("FigureUI_GameManager_GameTimer");
    figureUIPtr_->GetFigureUISettingsPtr("FigureUI_GameManager_GameTimer")->pos = { 132,25 };
    //figureUIPtr_->GetFigureUISettingsPtr("FigureUI_GameManager_GameTimer")->num = gameTimer_.GetFrameCurrentPtr();
    figureUIPtr_->GetFigureUISettingsPtr("FigureUI_GameManager_GameTimer")->num = &numForInvaseTime_;
    figureUIPtr_->GetFigureUISettingsPtr("FigureUI_GameManager_GameTimer")->format = FigureUI::Format::TIMER;

    // メニュー
    uiPtr_->Register("menu_frame", "Resources/png_gameMenu_frame.png");
    uiPtr_->GetUISpritePtr("menu_frame")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    uiPtr_->GetUISpritePtr("menu_frame")->SetPosition(Vector2{ 640.f,360.f });

    uiPtr_->Register("menu_resume", "Resources/png_gameMenu_resume.png");
    uiPtr_->GetUISpritePtr("menu_resume")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    uiPtr_->GetUISpritePtr("menu_resume")->SetPosition(Vector2{ 630.f,300.f });
    uiPtr_->Register("menu_restart", "Resources/png_gameMenu_restart.png");
    uiPtr_->GetUISpritePtr("menu_restart")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    uiPtr_->GetUISpritePtr("menu_restart")->SetPosition(Vector2{ 625.f,360.f });
    uiPtr_->Register("menu_option", "Resources/png_gameMenu_option.png");
    uiPtr_->GetUISpritePtr("menu_option")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    uiPtr_->GetUISpritePtr("menu_option")->SetPosition(Vector2{ 635.f,420.f });
    uiPtr_->Register("menu_quit", "Resources/png_gameMenu_quit.png");
    uiPtr_->GetUISpritePtr("menu_quit")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    uiPtr_->GetUISpritePtr("menu_quit")->SetPosition(Vector2{ 660.f,480.f });
}

void GameManager::Update(void)
{
    // メニュー
    ControllGameMenu_Update();
    // メニューを開いていたら、スキップ
    if (GetIsOpenMenu()) { return; }

    //>> オブジェクト
    // 鶏卵付近の蛇数をプレイヤーへ渡す
    HandoverSnakeCount();
    // 鶏卵更新
    chickenEgg_.Update();


    PlayerCarryableSnake();
    // プレイヤー更新
    player_.Update();
    // 惑星の更新
    gamePlanet_.Update();

    // 蛇がケージにに触れているかの判定と収監の処理
    SnakeIntoCustody();
    // 蛇の更新
    for (auto& snake : snakes_) { snake.Update(); }
    // ケージの更新
    for (auto& cage : snakeCages_) { cage.Update(); }
    // コインリストの更新
    coinList_.Update();

    // ゲームを終了するか確認
    CheckLockedCage();

    //>> イベント
    event_startTutorial_.Execute();
    event_endTutorial_.Execute();
    event_tutorialPlanetHole_.Execute();

    //>> タイマー
    ManageGameTimer();
}

void GameManager::Draw3d(void)
{
    //>> オブジェクト
    // 鶏卵描画
    chickenEgg_.Draw();
    // プレイヤー描画
    player_.Draw3d();
    // 惑星の描画
    gamePlanet_.Draw();
    // 蛇の描画
    for (auto& snake : snakes_) { snake.Draw(); }
    // ケージの描画
    for (auto& cage : snakeCages_) { cage.Draw(); }
    // コインリストの描画
    coinList_.Draw();

    event_tutorialPlanetHole_.Draw();
}

void GameManager::Draw2d(void)
{
    player_.Draw2dFore();

    event_startTutorial_.Draw();
    event_endTutorial_.Draw();

    ControllGameMenu_Draw();
}

void GameManager::Finalize(void)
{
    //>> オブジェクト
    // 惑星の終了処理
    gamePlanet_.Finalize();
    // プレイヤーの終了処理
    player_.Finalize();
    // 蛇の終了処理
    for (auto& snake : snakes_) { snake.Finalize(); }
    // 蛇檻の終了処理
    for (auto& cage : snakeCages_) { cage.Finalize(); }
    // コインリストの終了処理
    coinList_.Finalize();

    // カメラ
    camMPtr_->UnRegister(camera_debugPtr_.get());
    camMPtr_->UnRegister(camera_colPtr_.get());
    camMPtr_->UnRegister(camera_behind_.get());
    camMPtr_->UnRegister(camera_4Hole_.get());

    //>> イベント
    event_startTutorial_.Finalize();
    event_endTutorial_.Finalize();
    event_tutorialPlanetHole_.Finalize();
}

void GameManager::CamerasSetting(void)
{
    //>> カメラの座標設定
    camMPtr_->Register(camera_debugPtr_.get());
    camera_debugPtr_->SetTransform(Transform{ { 0,0,-70 }, camera_debugPtr_->GetTransform().rotation, camera_debugPtr_->GetTransform().scale });   // デバッグカメラの座標
    camMPtr_->Register(camera_colPtr_.get());
    camera_colPtr_->SetTransform(Transform{ { 3,172,-3 }, camera_colPtr_->GetTransform().rotation, camera_colPtr_->GetTransform().scale });        // プレイヤー用カメラの座標
    camMPtr_->Register(camera_behind_.get());
    camera_colPtr_->SetTransform(Transform{ { 3,172,-3 }, camera_colPtr_->GetTransform().rotation, camera_colPtr_->GetTransform().scale });        // プレイヤー用カメラ2の座標
    camMPtr_->Register(camera_4Hole_.get());
    camera_4Hole_->SetTransform(Transform{ { 0, 190, 0 }, { 1.5725f,-1.2175f,0} , camera_4Hole_->GetTransform().scale });                          // 穴に落ちたとき用カメラの座標と回転
}

void GameManager::HandoverSnakeCount(void)
{
    // 蛇の数を取得
    int32_t snakeCount = chickenEgg_.GetApproachingEggSnakes();
    // プレイヤーへ設定
    player_.SetApproachingEggSnakes(snakeCount);
}

void GameManager::SnakeIntoCustody(void)
{
    // 蛇が捕獲可能な状況にあるケージがあるか
    bool is_capture{};
    // ケージの座標ptr（蛇が捕まっている時用）
    Vector3* cagePosPtr{};
    // 全てのケージにおいて確認
    for (auto& cage : snakeCages_)
    {
        // getter呼び出し
        is_capture = cage.GetIsCapture();
        // falseの場合スキップ
        if (is_capture == false) { continue; }

        // trueの場合↓
        // この檻は捕獲したため、以降使用不可
        cage.SetIsLock(true);
        // ケージの座標ptrを受け取る。
        cagePosPtr = cage.GetPosPtr();
        // for文を抜ける
        break;
    }
    // 確認後、全てfalseなら終了
    if (is_capture == false) { return; }

    // どの蛇が捕まったのかを確認。
    for (auto& snake : snakes_)
    {
        // getter呼び出して確認
        bool is_touchCaged = snake.GetIsTouchCage();
        // falseの場合、スキップ
        if (is_touchCaged == false) { continue; }

        // trueの場合↓
        // 蛇にケージの座標ptrを渡す。
        snake.Caged(cagePosPtr);
        // 関数終了
        return;
    }

    // この関数は、ケージに触れる蛇が同Fに1体のみであるという前提がある。
    // 同Fに2箇所のケージに同時に触れると、該当ケージに触れた方ではない蛇が収監される可能性がある。
    // 解決策としては、それぞれに番号を振るか、蛇自体のcallbackに収監処理を含めること（結合度高い）
    // もしくは、ワンタイム文字列をもたせるか。
}

bool GameManager::CheckLockedCage(void)
{
    int32_t lockedCageNum = 0;

    // 全てのケージにおいて確認
    for (auto& cage : snakeCages_) { if (cage.GetIsLock()) { lockedCageNum++; } }
    // 収容完了しているケージに数が、"kCount_lockedCage_"以上なら、event_endTutorial_を起動
    if (lockedCageNum >= kCount_lockedCage_)
    {
        event_endTutorial_.SetIsExecute(true, Event_EndGame::DisplayString::CLEAR);
        return true;
    }

    return false;
}

void GameManager::PlayerCarryableSnake(void)
{
    static bool is_carrySnakePre{};
    bool is_carrySnake = player_.GetIsCarrySnake();
    if (is_carrySnake && is_carrySnakePre) { return; }

    if (is_carrySnake == false)
    {
        player_.SetSnakePtr(nullptr);
        is_carrySnakePre = is_carrySnake;
        return;
    }

    for (size_t i = 0; i < snakes_.size(); i++)
    {
        bool is_touchPlayer = snakes_[i].GetIsTouchPlayer();

        if (is_touchPlayer)
        {
            player_.SetSnakePtr(&snakes_[i]);
            break;
        }
    }

    is_carrySnakePre = is_carrySnake;
}

void GameManager::ManageGameTimer(void)
{
    //float rate = gameTimer_.GetTimeRate();
    //if (rate >= 1.f) { event_endTutorial_.SetIsExecute(true, Event_EndGame::DisplayString::TIMEOVER); }

    // タイマーの更新
    numForInvaseTime_ = (std::max)(kTimer_limit_ - gameTimer_.GetFrameCurrent(), 0.f);
    //numForInvaseTime_ = gameTimer_.GetFrameCurrent();
    gameTimer_.Update();

    // "event_startTutorial_"の実行フラグが[true]->[false]に切り替わった瞬間内部タイマーを実行する
    if (is_preStartEventExecute_ && event_startTutorial_.GetIsExecite() == false) { gameTimer_.Start(kTimer_limit_); }

    // 1フレーム前の"event_startTutorial_"の実行フラグとして、代入し続ける。
    is_preStartEventExecute_ = event_startTutorial_.GetIsExecite();
}

void GameManager::ControllGameMenu_Update(void)
{
    // ボタンを押した場合、is_pauseの状態を反転する
    if (KEYS::IsTrigger(DIK_P) || XPAD::IsTrigger(XPAD::Button::START)) { menu_.is_pause = !menu_.is_pause; }
    // メニューを開いていない場合
    if (GetIsOpenMenu() == false)
    {
        // タイマー再開
        gameTimer_.Resume();
        // 終了
        return;
    }

    // タイマーを一時停止
    gameTimer_.Pause();

    // 上下の押したボタン（十字キー）によって、選択項目を変更
    menu_.item = (MenuItem)((int32_t)menu_.item - (int32_t)XPAD::IsTrigger(XPAD::Button::TOP));
    menu_.item = (MenuItem)((int32_t)menu_.item + (int32_t)XPAD::IsTrigger(XPAD::Button::BOTTOM));
    // 選択項目がループするように
    Math::Function::Loop<MenuItem>(menu_.item, MenuItem::RESUME, MenuItem::QUIT);

    // 選択項目の強調表示
    switch (menu_.item)
    {
    case GameManager::MenuItem::RESUME:
        uiPtr_->GetUISpritePtr("menu_resume")->SetScale(Vector2{ 1.2f,1.2f });
        uiPtr_->GetUISpritePtr("menu_resume")->SetColor255(Vector4{ 200.f,200.f,20.f,255.f });
        uiPtr_->GetUISpritePtr("menu_restart")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_restart")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        uiPtr_->GetUISpritePtr("menu_option")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_option")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        uiPtr_->GetUISpritePtr("menu_quit")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_quit")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        break;
    case GameManager::MenuItem::RESTART:
        uiPtr_->GetUISpritePtr("menu_resume")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_resume")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        uiPtr_->GetUISpritePtr("menu_restart")->SetScale(Vector2{ 1.2f,1.2f });
        uiPtr_->GetUISpritePtr("menu_restart")->SetColor255(Vector4{ 200.f,200.f,20.f,255.f });
        uiPtr_->GetUISpritePtr("menu_option")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_option")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        uiPtr_->GetUISpritePtr("menu_quit")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_quit")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        break;
    case GameManager::MenuItem::OPTION:
        uiPtr_->GetUISpritePtr("menu_resume")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_resume")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        uiPtr_->GetUISpritePtr("menu_restart")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_restart")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        uiPtr_->GetUISpritePtr("menu_option")->SetScale(Vector2{ 1.2f,1.2f });
        uiPtr_->GetUISpritePtr("menu_option")->SetColor255(Vector4{ 200.f,200.f,20.f,255.f });
        uiPtr_->GetUISpritePtr("menu_quit")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_quit")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        break;
    case GameManager::MenuItem::QUIT:
        uiPtr_->GetUISpritePtr("menu_resume")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_resume")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        uiPtr_->GetUISpritePtr("menu_restart")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_restart")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        uiPtr_->GetUISpritePtr("menu_option")->SetScale(Vector2{ 1.f,1.f });
        uiPtr_->GetUISpritePtr("menu_option")->SetColor255(Vector4{ 255.f,255.f,255.f,255.f });
        uiPtr_->GetUISpritePtr("menu_quit")->SetScale(Vector2{ 1.2f,1.2f });
        uiPtr_->GetUISpritePtr("menu_quit")->SetColor255(Vector4{ 200.f,200.f,20.f,255.f });
        break;
    default:
        break;
    }

    // Aボタンを押していない場合、終了
    if (XPAD::IsTrigger(XPAD::Button::A))
    {
        switch (menu_.item)
        {
        case GameManager::MenuItem::RESUME:
            // メニューを閉じる
            menu_.is_pause = false;
            break;
        case GameManager::MenuItem::RESTART:
            SceneManager::GetInstance()->RequestChangeScene(SceneName::GAME);
            break;
        case GameManager::MenuItem::OPTION:
            // メニューを閉じる 一旦
            menu_.is_pause = false;
            break;
        case GameManager::MenuItem::QUIT:
            // タイトルに戻る
            SceneManager::GetInstance()->RequestChangeScene(SceneName::GAME);
            break;
        default:
            break;
        }
    }

    uiPtr_->Update("menu_frame");
    uiPtr_->Update("menu_resume");
    uiPtr_->Update("menu_restart");
    uiPtr_->Update("menu_option");
    uiPtr_->Update("menu_quit");
}

void GameManager::ControllGameMenu_Draw(void)
{
    // メニューを開いていない場合
    if (GetIsOpenMenu() == false)
    {
        // 終了
        return;
    }

    uiPtr_->Draw("menu_frame");
    uiPtr_->Draw("menu_resume");
    uiPtr_->Draw("menu_restart");
    uiPtr_->Draw("menu_option");
    uiPtr_->Draw("menu_quit");
}