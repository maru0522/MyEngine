#pragma once
#include "Coin.h"
#include <filesystem>
#include <forward_list>
#include "GamePlanet.h"
#include "LightManager.h"

/**
 * @file CoinList.h
 * @brief コインリストクラスや、コインファクトリークラスの記述
 */

class CoinFactory final
{
public:
    //>. 関数
    CoinFactory(void) = default;
    ~CoinFactory(void) = default;

    // コインを生成
    std::unique_ptr<Coin> Create(void);
    //std::unique_ptr<Coin> Create(CollisionManager* arg_colMPtr_, const std::string& arg_id, const Axis3& arg_posture, const Transform& arg_transform, bool arg_adaptPosture = false, bool arg_rePop = false);
};

class CoinList final
{
private:
    ////>> Singleton
    //CoinList(void) = default;
    //~CoinList(void) = default;
    //CoinList(const CoinList&) = delete;
    //CoinList& operator=(const CoinList&) = delete;

    //>> 定義
    struct CoinGroup
    {
        //>> 関数
        CoinGroup(const std::string& arg_id) : id_(arg_id) {};
        ~CoinGroup(void) = default;

        //>> 変数
        std::string id_;
        std::forward_list<std::unique_ptr<Coin>> flist_coinGroup_; // コインを保持するフォワードリスト配列
    };

    // AddCoin()の一番最後の引数の、グループ名が入っていない事を識別するための、名称。
    static const std::string kRecognizeNoArgName_;

public:
    //>> 関数
    CoinList(void) = default;
    ~CoinList(void) = default;
    // CoinListクラス自体の初期設定
    void Initialize(CollisionManager* arg_colMPtr_);
    // 管理しているコイン全ての更新処理を実行
    void Update(void);
    // 管理しているコイン全ての描画処理を実行
    void Draw(void);
    // CoinListクラス自体の終了処理
    void Finalize(void);

    // コインの配置等が記された外部テキストを読み込む。
    void DeployCoin(const std::filesystem::path& arg_path,Planet* arg_planetPtr,LightManager* arg_lightMPtr);

private:
    // 管理しているコイングループ配列に、新規のグループを追加する。
    void AddGroup(const std::string& arg_id);
    // 管理しているコイン配列に、新規のコインを追加する。
    Coin* AddCoin(const std::string& arg_groupId = kRecognizeNoArgName_);
    //Coin* AddCoin(const std::string& arg_id, const Axis3& arg_posture, const Transform& arg_transform, bool arg_adaptPosture = false, bool arg_rePop = false, const std::string& arg_groupId = kRecognizeNoArgName_);
    std::forward_list<CoinGroup>::iterator GetCoinGroupItr(const std::string& arg_groupId);

    //>> 変数
    bool is_finalized_{};           // 終了処理を呼んだ後か
    CollisionManager* colMPtr_{}; // 当たり判定マネージャー
    CoinFactory coinFactory_{};   // コインファクトリー

    std::forward_list<CoinGroup> flist_coinGroupList_; // コイングループを保持するフォワードリスト配列
    std::forward_list<std::unique_ptr<Coin>> flist_coinList_; // コインを保持するフォワードリスト配列
};

