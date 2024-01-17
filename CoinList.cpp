#include "CoinList.h"
const std::string CoinList::kRecognizeNoArgName_ = "nothing";

// ------------------------------------------------------------------------------------------------
void CoinList::Initialize(CollisionManager* arg_colMPtr_)
{
    colMPtr_ = arg_colMPtr_;
}

void CoinList::Update(void)
{
    // 終了処理の実行後ならスキップ
    if (is_finalized_) { return; }


    // コイングループの範囲for文。各要素は参照渡し
    for (auto& coinGroup : flist_coinGroupList_)
    {
        // 所属しているグループの範囲for文。各要素は参照渡し
        for (auto& coin : coinGroup.flist_coinGroup_)
        {
            // 更新処理
            coin->Update();
            // インスタンスを消滅させる条件が、揃っている場合削除。->nullptr化
            if (coin->IsElimination()) { coin.reset(); }
        }
    }
    // 無所属コインの範囲for文。各要素は参照渡し。
    for (auto& coin : flist_coinList_)
    {
        // 更新処理
        coin->Update();
        // インスタンスを消滅させる条件が、揃っている場合削除。->nullptr化
        if (coin->IsElimination()) { coin.reset(); }
    }


    // flist_coinList_のコインがnullptrかどうかチェックするラムダ式
    auto nullChecker = [&](std::unique_ptr<Coin>& arg_uniqueCoin) { return arg_uniqueCoin.get() == nullptr; };
    // 更新処理時のコイングループの範囲for文と全く同じ。
    for (auto& coinGroup : flist_coinGroupList_)
    {
        // nullptrとなっているインスタンを削除。
        coinGroup.flist_coinGroup_.remove_if(nullChecker);
    }
    // nullptrとなっているインスタンを削除。
    flist_coinList_.remove_if(nullChecker);
}

void CoinList::Draw(void)
{
    // 終了処理の実行後ならスキップ
    if (is_finalized_) { return; }

    // コイングループの範囲for文。各要素は参照渡し
    for (auto& coinGroup : flist_coinGroupList_)
    {
        // 所属しているグループの範囲for文。各要素は参照渡し
        for (auto& coin : coinGroup.flist_coinGroup_)
        {
            // 更新処理
            coin->Draw();
        }
    }
    // 範囲for文。各要素はconst参照渡し。
    for (const auto& coin : flist_coinList_)
    {
        // 描画処理
        coin->Draw();
    }
}

void CoinList::Finalize(void)
{
    // 終了処理の実行後ならスキップ
    if (is_finalized_) { return; }

    // 念のため、残っている全てのコインを解放。->nullptr化
    for (auto& coin : flist_coinList_) { coin.reset(); }
    // nullptrのみとなった配列を初期化
    flist_coinList_.clear();

    // 終了処理実行済みフラグをtrue
    is_finalized_ = true;
}

void CoinList::AddGroup(const std::string& arg_id)
{
    // 終了処理の実行後ならスキップ
    if (is_finalized_) { return; }
    // グループ名が未入力かどうか識別するidと一致している場合はスキップ
    if (arg_id == kRecognizeNoArgName_) { return; }

    flist_coinGroupList_.emplace_front(arg_id);
}

Coin* CoinList::AddCoin(const std::string& arg_groupId)
{
    // 終了処理の実行後ならスキップ
    if (is_finalized_) { return nullptr; }


    // グループ名が入力されているかどうか
    const bool is_groupName = arg_groupId != kRecognizeNoArgName_;
    // 生成したコインのインスタンス。
    // colMptrがnullptrの場合、Create()はnullPtrを返す。しかし、上のnullCheckで確認済みのため、coinのnullCheckは必要ない。
    auto coin = coinFactory_.Create();


    if (is_groupName)
    {
        // 該当するCoinGroupのitrを取得
        auto itr = GetCoinGroupItr(arg_groupId);
        if (itr != flist_coinGroupList_.end())
        {
            // そのグループに追加
            itr->flist_coinGroup_.emplace_front(std::move(coin));
            // 追加したコインのptrを返す。（該当するコイングループのitr->コインのフォワードリスト配列.先頭の参照().ユニークポインタが保持するptr()
            return itr->flist_coinGroup_.front().get();
        }
        else
        {
            // ないなら生成する。
            AddGroup(arg_groupId);
            // 生成後そのグループに追加。
            flist_coinGroupList_.front().flist_coinGroup_.emplace_front(std::move(coin));
            // 追加したコインのptrを返す。（該当するコイングループのフォワードリスト配列.先頭の参照().コインのフォワードリスト配列.先頭の参照().ユニークポインタが保持するptr()
            return flist_coinGroupList_.front().flist_coinGroup_.front().get();
        }
    }
    else
    {
        // グループ名が入力されてないなら、flist_coinList_の方へ直接追加する。
        flist_coinList_.emplace_front(std::move(coin));
        // 追加したコインのptrを返す。（無所属コインのフォワードリスト配列.先頭の参照().ユニークポインタが保持するptr()
        return flist_coinList_.front().get();
    }

    // 保険
    return nullptr;
}

//Coin* CoinList::AddCoin(const std::string& arg_id, const Axis3& arg_posture, const Transform& arg_transform, bool arg_adaptPosture, bool arg_rePop, const std::string& arg_groupId)
//{
//    // 終了処理の実行後ならスキップ
//    if (is_finalized_) { return nullptr; }
//    //null check
//    if (!colMPtr_) { return nullptr; }
//
//
//    // グループ名が入力されているかどうか
//    const bool is_groupName = arg_groupId != kRecognizeNoArgName_;
//    // 生成したコインのインスタンス。
//    // colMptrがnullptrの場合、Create()はnullPtrを返す。しかし、上のnullCheckで確認済みのため、coinのnullCheckは必要ない。
//    auto coin = coinFactory_.Create(colMPtr_, arg_id, arg_posture, arg_transform, arg_adaptPosture, arg_rePop);
//
//
//    if (is_groupName)
//    {
//        // 該当するCoinGroupのitrを取得
//        auto itr = GetCoinGroupItr(arg_groupId);
//        if (itr != flist_coinGroupList_.end())
//        {
//            // そのグループに追加
//            itr->flist_coinGroup_.emplace_front(std::move(coin));
//            // 追加したコインのptrを返す。（該当するコイングループのitr->コインのフォワードリスト配列.先頭の参照().ユニークポインタが保持するptr()
//            return itr->flist_coinGroup_.front().get();
//        }
//        else
//        {
//            // ないなら生成する。
//            AddGroup(arg_groupId);
//            // 生成後そのグループに追加。
//            flist_coinGroupList_.front().flist_coinGroup_.emplace_front(std::move(coin));
//            // 追加したコインのptrを返す。（該当するコイングループのフォワードリスト配列.先頭の参照().コインのフォワードリスト配列.先頭の参照().ユニークポインタが保持するptr()
//            return flist_coinGroupList_.front().flist_coinGroup_.front().get();
//        }
//    }
//    else
//    {
//        // グループ名が入力されてないなら、flist_coinList_の方へ直接追加する。
//        flist_coinList_.emplace_front(std::move(coin));
//        // 追加したコインのptrを返す。（無所属コインのフォワードリスト配列.先頭の参照().ユニークポインタが保持するptr()
//        return flist_coinList_.front().get();
//    }
//
//    // 保険
//    return nullptr;
//}

void CoinList::DeployCoin(const std::filesystem::path& arg_path, Planet* arg_planetPtr, LightManager* arg_lightMPtr)
{
    // 指定されたファイルを開く
    std::ifstream ifs_file(arg_path);
    // 失敗なら終了
    if (ifs_file.fail()) { return; }


    std::string str_line;
    // ファイルから1行ずつ読み込む
    while (std::getline(ifs_file, str_line))
    {
        // 行の始まりが、"#NAME:" 以外から始まっているのなら、次へ
        if (str_line.starts_with("#NAME:") == false) { continue; }


        // コイン生成のための要素
        std::string coin_name;
        Axis3 coin_posture;
        Transform coin_transform;
        bool coin_adapt{};
        bool coin_repop{};
        std::string coin_gname;
        // 丸影のための要素
        Vector3 coin_atten{};
        Vector2 coin_wide{};
        float coin_dist{};


        std::istringstream iss_line(str_line);
        std::string str_word;
        // '#'区切りで文字列を読み込む（各接頭辞ごとの要素）
        while (std::getline(iss_line, str_word, '#'))
        {
            // 接頭辞で処理を分ける。

            if (str_word.starts_with("NAME:"))
            {
                // "NAME"を除いた残りを名前にする。
                coin_name = str_word.substr(5);
            }
            else if (str_word.starts_with("POSTURE:"))
            {
                // "POSTURE:"を除いた残りを使用する
                if (str_word.substr(8) == "{}") // 初期化子のみの場合。
                {
                    // 初期化関数を呼んで終了。
                    coin_posture = Axis3::Initialize();
                    continue;
                }


                // "POSTURE:" を除いた残りを使用する。
                std::istringstream iss_word(str_word.substr(8));
                std::string str_inBracket;
                float dir[3][3]{}; // 姿勢の値用変数
                int count_fru{}; // (forward,right,up)
                // '}'区切りで文字列を読み込む（中括弧内に入っている姿勢の各数値）
                while (std::getline(iss_word, str_inBracket, '}'))
                {
                    // 4回以上回っていたらbreak
                    if (count_fru > 2) { break; }


                    // "{"を除いた残りを使用する。"}"は、getlineで消滅した。
                    std::istringstream iss_inBracket(str_inBracket.substr(1));
                    std::string str_num;
                    int count_xyz{}; // (x,y,z)
                    // ','区切りで文字列を読み込む（具体的な姿勢の数値）
                    while (std::getline(iss_inBracket, str_num, ','))
                    {
                        // 4回以上回っていたらbreak
                        if (count_xyz > 2) { break; }


                        // 区切られた文字列を数値に変換
                        dir[count_fru][count_xyz] = std::stof(str_num);
                        // ループ回数を+1
                        count_xyz++;
                    }


                    // ループ回数を+1
                    count_fru++;
                }

                // 取得した値を姿勢として代入
                coin_posture.forward = Math::Vec3::ConvertArray3(dir[0]);
                coin_posture.right = Math::Vec3::ConvertArray3(dir[1]);
                coin_posture.up = Math::Vec3::ConvertArray3(dir[2]);
            }
            else if (str_word.starts_with("TRANS:"))
            {
                // "TRANS:"を除いた残りを使用する
                if (str_word.substr(6) == "{}") // 初期化子のみの場合。
                {
                    // 初期化関数を呼んで終了。
                    coin_transform.Initialize();
                    continue;
                }


                // "TRANS:" を除いた残りを使用する。
                std::istringstream iss_word(str_word.substr(6));
                std::string str_inBracket;
                float trans[3][3]{}; // 姿勢の値用変数
                int count_prs{}; // (position,rotation,scale)
                // '}'区切りで文字列を読み込む（中括弧内に入っている姿勢の各数値）
                while (std::getline(iss_word, str_inBracket, '}'))
                {
                    // 4回以上回っていたらbreak
                    if (count_prs > 2) { break; }


                    // "{"を除いた残りを使用する。"}"は、getlineで消滅した。
                    std::istringstream iss_inBracket(str_inBracket.substr(1));
                    std::string str_num;
                    int count_xyz{}; // (x,y,z)
                    // ','区切りで文字列を読み込む（具体的な姿勢の数値）
                    while (std::getline(iss_inBracket, str_num, ','))
                    {
                        // 4回以上回っていたらbreak
                        if (count_xyz > 2) { break; }


                        // 区切られた文字列を数値に変換
                        trans[count_prs][count_xyz] = std::stof(str_num);
                        // ループ回数を+1
                        count_xyz++;
                    }


                    // ループ回数を+1
                    count_prs++;
                }

                // 取得した値を姿勢として代入
                coin_transform.position = Math::Vec3::ConvertArray3(trans[0]);
                coin_transform.rotation = Math::Vec3::ConvertArray3(trans[1]);
                coin_transform.scale = Math::Vec3::ConvertArray3(trans[2]);
            }
            else if (str_word.starts_with("ADAPT:"))
            {
                str_word.substr(6) == "true" ?
                    coin_adapt = true :
                    coin_adapt = false;
            }
            else if (str_word.starts_with("REPOP:"))
            {
                str_word.substr(6) == "true" ?
                    coin_repop = true :
                    coin_repop = false;
            }
            else if (str_word.starts_with("GNAME:"))
            {
                // "GNAME"を除いた残りを名前にする。
                coin_gname = str_word.substr(6);
            }
            else if (str_word.starts_with("ATTEN:"))
            {
                std::string str_subPrefix = str_word.substr(6);

                // "ATTEN:"を除いた残りを使用する
                if (str_subPrefix == "{}") // 初期化子のみの場合。
                {
                    // 初期化関数を呼んで終了。
                    coin_atten = { 0.02f,0.06f,0.01f };
                    continue;
                }

                // "{}"を除いた残りを使用する。
                str_subPrefix.erase(0, 1);
                str_subPrefix.erase(str_subPrefix.size() - 1, 1);
                std::istringstream iss_inBracket(str_subPrefix);
                std::string str_num;
                float atten[3]{};
                int count_xyz{}; // (x,y,z)
                // ','区切りで文字列を読み込む（具体的な姿勢の数値）
                while (std::getline(iss_inBracket, str_num, ','))
                {
                    // 4回以上回っていたらbreak
                    if (count_xyz > 2) { break; }


                    // 区切られた文字列を数値に変換
                    atten[count_xyz] = std::stof(str_num);
                    // ループ回数を+1
                    count_xyz++;
                }

                // 取得した値をattenとして代入
                coin_atten = Math::Vec3::ConvertArray3(atten);
            }
            else if (str_word.starts_with("WIDE:"))
            {
                std::string str_subPrefix = str_word.substr(5);

                // "ATTEN:"を除いた残りを使用する
                if (str_subPrefix == "{}") // 初期化子のみの場合。
                {
                    // 初期化関数を呼んで終了。
                    coin_wide = { 6.f,8.f };
                    continue;
                }

                // "{}"を除いた残りを使用する。
                str_subPrefix.erase(0, 1);
                str_subPrefix.erase(str_subPrefix.size() - 1, 1);
                std::istringstream iss_inBracket(str_subPrefix);
                std::string str_num;
                float wide[2]{};
                int count_xy{}; // (x,y)
                // ','区切りで文字列を読み込む（具体的な姿勢の数値）
                while (std::getline(iss_inBracket, str_num, ','))
                {
                    // 3回以上回っていたらbreak
                    if (count_xy > 1) { break; }


                    // 区切られた文字列を数値に変換
                    wide[count_xy] = std::stof(str_num);
                    // ループ回数を+1
                    count_xy++;
                }

                // 取得した値をwideとして代入
                coin_wide.x = wide[0];
                coin_wide.y = wide[1];
            }
            else if (str_word.starts_with("DIST:"))
            {
                std::string str_subPrefix = str_word.substr(5);

                // "ATTEN:"を除いた残りを使用する
                if (str_subPrefix == "") // 初期化子のみの場合。
                {
                    // 初期化関数を呼んで終了。
                    coin_dist = 1.f;
                    continue;
                }

                // 取得した値をdistとして代入
                coin_dist = std::stof(str_subPrefix);
            }
        }

        // 生成されたコインのptr
        Coin* generated_coinPtr = nullptr;
        // コインの生成
        coin_gname == "" ? // グループ名が記述されているかによって、引数を変更
            generated_coinPtr = AddCoin() :
            generated_coinPtr = AddCoin(coin_gname);

        generated_coinPtr->SetUp(colMPtr_, coin_name, coin_posture, coin_transform, coin_adapt, coin_repop);
        generated_coinPtr->SetupCircleShadows(arg_planetPtr, arg_lightMPtr, coin_atten, coin_wide, coin_dist);
    }
}

std::forward_list<CoinList::CoinGroup>::iterator CoinList::GetCoinGroupItr(const std::string& arg_groupId)
{
    auto itr = flist_coinGroupList_.begin();
    auto end = flist_coinGroupList_.end();

    while (itr != end)
    {
        // 名前の一致するコイングループを、全検索
        if (itr->id_ == arg_groupId) { return itr; }
        itr++;
    }

    // 末尾の次を返す
    return end;
}

// ------------------------------------------------------------------------------------------------
std::unique_ptr<Coin> CoinFactory::Create(void)
{
    // ユニークポインタで、コインを生成
    std::unique_ptr<Coin> coin = std::make_unique<Coin>();
    // 所有権を譲渡する形で返す。
    return std::move(coin);
}

//std::unique_ptr<Coin> CoinFactory::Create(CollisionManager* arg_colMPtr_, const std::string& arg_id, const Axis3& arg_posture, const Transform& arg_transform, bool arg_adaptPosture, bool arg_rePop)
//{
//    // null check
//    //if (!arg_colMPtr_) { return nullptr; }
//
//    // ユニークポインタで、コインを生成
//    std::unique_ptr<Coin> coin = std::make_unique<Coin>();
//    // コインの設定を引数通りに行う
//    //coin->SetUp(arg_colMPtr_, arg_id, arg_posture, arg_transform, arg_adaptPosture, arg_rePop);
//    // 所有権を譲渡する形で返す。
//    return std::move(coin);
//}