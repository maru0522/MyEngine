#include "FigureUI.h"
#include "MathUtil.h"

const float FigureUI::kDefault_pictureLength_ = 32.f; // デフォルトの数字1つ分の長さ（正方形前提）

FigureUI* FigureUI::GetInstance(void)
{
    static FigureUI instance;
    return &instance;
}

void FigureUI::Initialize(void)
{
    is_finalized_ = false;
    uiPtr_ = UI::GetInstance();
}

void FigureUI::Update(void)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }

    // FigureUIで保管している全ての数字UIで実行する
    for (auto& figure : umap_figures_)
    {
        // 数字UIの設定が変更されていたら
        if (figure.second.isDirty)
        {
            //#kDefault_intDigit_の分登録。
            for (int32_t i = 0; i < kDefault_intDigit_; i++) { AdaptSettings(figure.first, i, figure.second); }
            //#kDefault_floatDigit_の分登録
            for (int32_t i = 0; i > -kDefault_floatDigit_; i--) { AdaptSettings(figure.first, i - 1, figure.second); }

            // 変更フラグをfalse
            figure.second.isDirty = false;
        }

        // それぞれのspriteの座標や、切り抜き位置などを値に合わせ変更
        float num = figure.second.num;
        // 整数部分
        for (int32_t i = 0; i < kDefault_intDigit_; i++)
        {
            const std::string& key = SynthesisName(figure.first, i);
            // UIクラスから、FigureUIで管理している分の数字UIのspriteのptrを取得する
            auto spPtr = uiPtr_->GetUISpritePtr(key);

            // 桁の値抽出
            const int32_t digitValue = Math::Function::DigitValue(num, i);
            // 切り抜きの始点
            const Vector2 cutStartPoint = { kDefault_pictureLength_ * digitValue, 0.f };
            // 設定
            spPtr->SetCutStartPoint(cutStartPoint);
            uiPtr_->Update(key);
        }
        // 小数点以下部分
        for (int32_t i = 0; i > -kDefault_floatDigit_; i--)
        {
            const std::string& key = SynthesisName(figure.first, i - 1);
            // UIクラスから、FigureUIで管理している分の数字UIのspriteのptrを取得する
            auto spPtr = uiPtr_->GetUISpritePtr(key);

            // 桁の値抽出
            const int32_t digitValue = Math::Function::DigitValue(num, i - 1);
            // 切り抜きの始点
            const Vector2 cutStartPoint = { kDefault_pictureLength_ * digitValue, 0.f };
            // 設定
            spPtr->SetCutStartPoint(cutStartPoint);
            uiPtr_->Update(key);
        }
    }
}

void FigureUI::Draw(void)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }

    // 登録してある数字全て描画
    for (const auto& figure : umap_figures_)
    {
        // 整数部分
        for (int32_t i = 0; i < kDefault_intDigit_; i++)
        {
            const std::string& key = SynthesisName(figure.first, i);
            uiPtr_->Draw(key);
        }
        // 小数点以下部分
        for (int32_t i = 0; i > -kDefault_floatDigit_; i--)
        {
            const std::string& key = SynthesisName(figure.first, i - 1);
            uiPtr_->Draw(key);
        }
    }
}

void FigureUI::Finalize(void)
{
    // 残ってるkey分全て、uiから抹消
    for (const auto& figure : umap_figures_) {
        // Registerと同じく、[桁数の数]と同じだけ消す
        //#kDefault_intDigit_の分抹消
        for (int32_t i = 0; i < kDefault_intDigit_; i++) { uiPtr_->UnRegister(SynthesisName(figure.first, i)); }
        //#kDefault_floatDigit_の分抹消
        for (int32_t i = 0; i > -kDefault_floatDigit_; i--) { uiPtr_->UnRegister(SynthesisName(figure.first, i - 1)); }
    }
    // umap配列の中身を初期化
    umap_figures_.clear();

    is_finalized_ = true;
}

void FigureUI::Register(const std::string& arg_key)
{
    Register(arg_key, FigureSpriteSettings{});
}

void FigureUI::Register(const std::string& arg_key, const FigureSpriteSettings& arg_settings)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }


    // umap配列に同名画像が既に登録されているか
    bool isContained = umap_figures_.contains(arg_key);
    // 登録されているならスキップ
    if (isContained) { return; }

    // UI側に同名鍵で[桁数の数]だけ登録 ※[桁数の数] = kDefault_intDigit_ + kDefault_floatDigit_
    //#kDefault_intDigit_の分登録。
    for (int32_t i = 0; i < kDefault_intDigit_; i++) { AdaptSettings(arg_key, i, arg_settings); }
    //#kDefault_floatDigit_の分登録
    for (int32_t i = 0; i > -kDefault_floatDigit_; i--) { AdaptSettings(arg_key, i - 1, arg_settings); }
    // umap配列に登録
    umap_figures_.emplace(arg_key, arg_settings);
}

void FigureUI::UnRegister(const std::string& arg_key)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }


    // umap配列に存在するか
    bool isContained = umap_figures_.contains(arg_key);
    // 存在しないならスキップ
    if (isContained == false) { return; }

    // umap配列から削除
    if (isContained) { umap_figures_.erase(arg_key); }
    uiPtr_->UnRegister(arg_key);
}

void FigureUI::AdaptSettings(const std::string& arg_key, int32_t arg_num, const FigureSpriteSettings& arg_settings)
{
    const std::string& key = SynthesisName(arg_key, arg_num);

    uiPtr_->Register(key, kDefault_figurePath_);
    // ここで例外スローしたら、同名で登録している
    auto spPtr = uiPtr_->GetUISpritePtr(key);
    spPtr->SetAlpha(arg_settings.alpha);
    spPtr->SetSize(arg_settings.size);
    spPtr->SetScale(arg_settings.scale);
    spPtr->SetAnchorPoint(arg_settings.anchorPoint);
    spPtr->SetCutStartPoint(arg_settings.cutStartPoint);
    spPtr->SetCutLength(arg_settings.cutLength);

    // 座標（基点.x - 数字の切り抜き幅 * 数字の桁数, 基点.y）※小数点以下の桁の場合、[数字の桁数]がマイナス
    const Vector2 pos = { arg_settings.pos.x - kDefault_pictureLength_ * arg_num, arg_settings.pos.y };
    spPtr->SetPosition(pos);
}

FigureUI::FigureSpriteSettings* FigureUI::GetFigureUISettingsPtr(const std::string& arg_key)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return nullptr; }

    // isDirtyをtrueに
    umap_figures_[arg_key].isDirty = true;
    // ptrを返す。
    return &umap_figures_[arg_key];
}
