#include "GameManager.h"

void GameManager::SetAllPtrs(CollisionManager* arg_colMPtr, CameraManager* arg_camMPtr, LightManager* arg_lightMPtr)
{
    colMPtr_ = arg_colMPtr;
    camMPtr_ = arg_camMPtr;
    lightMPtr_ = arg_lightMPtr;
}

void GameManager::Initialize()
{
    //>> オブジェクト
    // カメラの設定
    CamerasSetting();
    // 惑星の初期化
    tutorialPlanet_.Initialize(colMPtr_);
    // プレイヤーの初期化
    player_.Initialize(camMPtr_, colMPtr_, lightMPtr_, &tutorialPlanet_);
    player_.SetupLightCircleShadows();
    // 鶏卵の初期化
    chickenEgg_.Initialize(colMPtr_, lightMPtr_, &tutorialPlanet_);
    // 蛇
    for (auto& snake : snakes_) { snake.Initialize(colMPtr_, lightMPtr_, &tutorialPlanet_, &chickenEgg_); }
    snakes_[1].GetTransformPtr()->position = { 10,60, 20 };
    snakes_[2].GetTransformPtr()->position = { -10,60, 20 };
    // 蛇用ケージ
    for (auto& cage : snakeCages_) { cage.Initialize(colMPtr_, lightMPtr_, &tutorialPlanet_); }
    snakeCages_[0].SetPosition(Vector3{ 20,-60, 30 });
    snakeCages_[1].SetPosition(Vector3{ -40,20, 25 });
    snakeCages_[2].SetPosition(Vector3{ -40,50, -70 });


    // コインリストの初期化とコインの配置
    coinList_.Initialize(colMPtr_);
    coinList_.DeployCoin("Resources/externalText/coinsLayout.txt", &tutorialPlanet_, lightMPtr_);

    //>> イベント
    event_startTutorial_.Initialize(camMPtr_, &player_);
    event_endTutorial_.Initialize(camMPtr_);
    event_tutorialPlanetHole_.Initialize(colMPtr_, camMPtr_, &player_);
    // startTutorialから起動する。 endTutorialは終了時,tutorialPlanetHoleは、更新処理内で起動する
    event_startTutorial_.SetIsExecute(true);
}

void GameManager::Update(void)
{
    //>> オブジェクト
    // 鶏卵付近の蛇数をプレイヤーへ渡す
    HandoverSnakeCount();
    // 鶏卵更新
    chickenEgg_.Update();

    // プレイヤー更新
    player_.Update();
    // 惑星の更新
    tutorialPlanet_.Update();

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
}

void GameManager::Draw3d(void)
{
    //>> オブジェクト
    // 鶏卵描画
    chickenEgg_.Draw();
    // プレイヤー描画
    player_.Draw3d();
    // 惑星の描画
    tutorialPlanet_.Draw();
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
}

void GameManager::Finalize(void)
{
    //>> オブジェクト
    // 惑星の終了処理
    tutorialPlanet_.Finalize();
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
        event_endTutorial_.SetIsExecute(true);
        return true;
    }

    return false;
}
