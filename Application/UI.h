#pragma once
#include <unordered_map>
#include <string>
#include "Sprite.h"
#include <filesystem>

class UI
{
private:
    //>> Singleton
    UI(void) = default;
    ~UI(void) = default;
    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;

    //>> 定義
    using KEY = std::string;

public:
    using fsPath = std::filesystem::path;

    //>> 関数
    static UI* GetInstance(void);

    void Initialize(void);
    void Update(const std::string& arg_key);
    void Draw(const std::string& arg_key);
    void Finalize(void);

    void Register(const std::string& arg_key, const std::string& arg_path);
    void UnRegister(const std::string& arg_key);

    //>> 変数
    std::unordered_map<KEY, Sprite> umap_sprites_;
    bool is_finalized_;

};

