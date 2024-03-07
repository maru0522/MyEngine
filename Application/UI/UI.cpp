#include "UI.h"

UI* UI::GetInstance(void)
{
    static UI ins;
    return &ins;
}

void UI::Initialize(void)
{
    is_finalized_ = false;
}

void UI::Update(const std::string& arg_key)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }

    // 指定したUIの描画関数を呼ぶ
    umap_sprites_.at(arg_key).Update();
}

void UI::Draw(const std::string& arg_key)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }

    // 指定したUIの描画関数を呼ぶ
    umap_sprites_.at(arg_key).Draw();
}

void UI::Finalize(void)
{
    // umap配列の中身を初期化
    umap_sprites_.clear();

    is_finalized_ = true;
}

void UI::Register(const std::string& arg_key, const std::string& arg_path)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }


    // umap配列に同名画像が既に登録されているか
    bool isContained = umap_sprites_.contains(arg_key);
    // 登録されているならスキップ
    if (isContained) { return; }


    // umap配列に登録する画像
    Sprite sprite(arg_path);
    // 登録
    umap_sprites_.insert({arg_key,sprite});
}

void UI::UnRegister(const std::string& arg_key)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }


    // umap配列に存在するか
    bool isContained = umap_sprites_.contains(arg_key);
    // 存在しないならスキップ
    if (isContained == false) { return; }

    // umap配列から削除
    if (isContained) { umap_sprites_.erase(arg_key); }
}
