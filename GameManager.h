#pragma once
// シングルトン
#include "CollisionManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "FigureUI.h"

// オブジェクト
#include "Planet.h"
#include "Player.h"
#include "ChickenEgg.h"
#include "Snake.h"
#include "SnakeCage.h"

#include "CoinList.h"

// カメラ
#include "BehindCamera.h"
#include "SphericalCamera.h"

// イベント
#include "Event_StartTutorial.h"
#include "Event_EndTutorial.h"
#include "Event_TutorialPlanetHole.h"

class GameManager
{
public:
    //>> 定義
    enum class MenuItem
    {
        RESUME,
        RESTART,
        OPTION,
        QUIT,
    };

    struct GameMenu
    {
        bool is_pause{};
        MenuItem item{};
    };

    const int32_t kCount_lockedCage_ = 3;    // 何体分の檻が収容完了していたらゲームが終了するか。
    const float kTimer_limit_ = 120;         // ゲームの制限時間

    //>> 関数
    GameManager(void) = default;
    ~GameManager(void) = default;

    // 必要なptrを設定
    void SetAllPtrs(CollisionManager* arg_colMPtr, CameraManager* arg_camMPtr, LightManager* arg_lightMPtr, FigureUI* arg_figureUIPtr, UI* arg_uiPtr);
    // 初期化関数
    void Initialize(void);
    void Update(void);
    void Draw3d(void);
    void Draw2d(void);
    void Finalize(void);

private:
    // カメラの設定
    void CamerasSetting(void);
    //
    void HandoverSnakeCount(void);
    // 蛇をケージに閉じ込めるかの条件判定と、収監の実行
    void SnakeIntoCustody(void);
    //
    bool CheckLockedCage(void);
    // playerが蛇に触れているかと、その蛇のptrの取得
    void PlayerCarryableSnake(void);
    // ゲーム用タイマーの管理
    void ManageGameTimer(void);
    // ゲームメニューの管理
    void ControllGameMenu_Update(void);
    void ControllGameMenu_Draw(void);
    bool GetIsOpenMenu(void) { return menu_.is_pause; }

    //>> 変数
    CollisionManager* colMPtr_;
    CameraManager* camMPtr_;
    LightManager* lightMPtr_;
    FigureUI* figureUIPtr_;
    UI* uiPtr_;

    // カメラ
    std::unique_ptr<ICamera> camera_debugPtr_{ std::make_unique<ICamera>() };
    std::unique_ptr<SphericalCamera> camera_colPtr_{ std::make_unique<SphericalCamera>("follow_player") };
    std::unique_ptr<BehindCamera> camera_behind_{ std::make_unique<BehindCamera>("follow_player") };
    std::unique_ptr<SphericalCamera> camera_4Hole_{ std::make_unique<SphericalCamera>("hole_planet") };

    // オブジェクト
    GamePlanet gamePlanet_;
    Player player_;
    ChickenEgg chickenEgg_;
    std::array<Snake, 3> snakes_;
    std::array<SnakeCage, 3> snakeCages_;

    CoinList coinList_;

    // イベント
    Event_StartTutorial event_startTutorial_;
    Event_EndTutorial event_endTutorial_;
    Event_TutorialPlanetHole event_tutorialPlanetHole_;

    // タイマー
    bool is_preStartEventExecute_{};
    float numForInvaseTime_;
    DeltaTimer gameTimer_;

    // メニュー
    GameMenu menu_;
    //Sprite png_menu_frame_;
    //Sprite png_menu_resume_;
    //Sprite png_menu_restart_;
    //Sprite png_menu_option_;
    //Sprite png_menu_quit_;
};
