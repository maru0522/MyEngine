#pragma once
#include "UI.h"

class FigureUI
{
private:
    //>> Singleton
    FigureUI(void) = default;
    ~FigureUI(void) = default;
    FigureUI(const FigureUI&) = delete;
    FigureUI& operator=(const FigureUI&) = delete;

public:
    //>> 定義
    // 数字の表示部分
    enum class Scope
    {
        ALL, // 00000
        MAX, // ØØ123 ※Øは非表示
    };

    // 数字の表示形式
    enum class Format
    {
        SCORE, // 00000
        TIMER, // 00:00
    };

    struct FigureSpriteSettings
    {
        FigureSpriteSettings(void) = default;
        ~FigureSpriteSettings(void) = default;

        float* num{};
        float alpha{ 1 };

        Vector2 pos{};
        Vector2 size{ 32,32 };
        Vector2 scale{ 1,1 };
        Vector2 anchorPoint{ 0.5,0.5 };
        Vector2 cutStartPoint{ 0,0 };
        Vector2 cutLength{ kDefault_pictureLength_,kDefault_pictureLength_ };

        bool isDirty{};

        Scope scope{};
        Format format{};
    };

    const std::string kDefault_figurePath_ = "Resources/figures2.png";
    const int32_t kDefault_intDigit_ = 5;               // デフォルトの整数部の表示桁数
    const int32_t kDefault_floatDigit_ = 3;             // デフォルトの整数部の表示桁数
    const static float kDefault_pictureLength_;         // デフォルトの数字1つ分の長さ（正方形前提）
    const static float kDistance_symbolDecimal_;        // 小数点の座標が基準からどの程度離れているかの距離
    const static float kDefaultSpace_symbolDecimal_;    // 小数点の為に必要な空間の長さ
    const static float kDistance_symbolColon_;          // コロンの座標が基準からどの程度離れているかの距離
    const static float kDefaultSpace_symbolColon_;      // コロンの為に必要な空間の長さ

    //>> 関数
    static FigureUI* GetInstance(void);

    void Initialize(void);
    void Update(void);
    void Draw(void);
    void Finalize(void);

    // 新しく数字を表示させるため、配列に追加する。"設定した名前_番号"で登録される。
    void Register(const std::string& arg_key);
    void Register(const std::string& arg_key, const FigureSpriteSettings& arg_settings);
    void UnRegister(const std::string& arg_key);

private:
    void AdaptSettings_Number(const std::string& arg_key, int32_t arg_num, const FigureSpriteSettings& arg_settings);
    void AdaptSettings_Symbol(const std::string& arg_key, const std::string& arg_symbolName, float arg_posX, const FigureSpriteSettings& arg_settings);
    std::string SynthesisName(const std::string& arg_key, int32_t arg_num) { return arg_key + std::to_string(arg_num); }

    //>> 変数
    std::unordered_map<std::string, FigureSpriteSettings> umap_figures_;
    UI* uiPtr_{};
    bool is_finalized_{};

public:
    //>> setter
    //void SetNumber(const std::string& arg_key, float arg_num) { umap_figures_[arg_key].num = arg_num; }

    //>> getter
    FigureSpriteSettings* GetFigureUISettingsPtr(const std::string& arg_key);
};

