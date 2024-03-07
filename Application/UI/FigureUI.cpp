#include "FigureUI.h"
#include "MathUtil.h"

const float FigureUI::kDefault_pictureLength_ = 32.f;          // デフォルトの数字1つ分の長さ（正方形前提）
const float FigureUI::kDistance_symbolDecimal_ = 12.f;         // 小数点の座標が基準からどの程度離れているかの距離
const float FigureUI::kDefaultSpace_symbolDecimal_ = 4.f;      // 小数点の為に必要な空間の長さ
const float FigureUI::kDistance_symbolColon_ = 61.f;           // コロンの座標が基準からどの程度離れているかの距離
const float FigureUI::kDefaultSpace_symbolColon_ = 16.f;       // コロンの為に必要な空間の長さ

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
            // 表示形式が"TIMER"である。
            if (figure.second.format == Format::TIMER)
            {
                // 強制的に表示範囲をALLにする。※"Format::TIMER"なら"Scope::MAX"は使用不可
                figure.second.scope = Scope::ALL;
            }

            //#kDefault_intDigit_の分変更
            for (int32_t i = 0; i < figure.second.num_intDigit; i++) { AdaptSettings_Number(figure.first, i, figure.second); }
            //#kDefault_floatDigit_の分変更
            for (int32_t i = 0; i > -figure.second.num_floatDigit; i--) 
            {
                AdaptSettings_Number(figure.first, i - 1, figure.second); }
            //#小数点の分変更
            //  小数点以下の桁数が0より大きく指定されている場合、更新を行う。
            if (figure.second.num_floatDigit > 0)
            {
                FigureSpriteSettings decimalSettings = figure.second;
                decimalSettings.cutStartPoint = { 320.f,0.f };
                AdaptSettings_Symbol(figure.first, "DecimalPoint", kDistance_symbolDecimal_, decimalSettings);
            }
            //#時計用のコロンの分登録
            //  整数部の桁数が2以上の値が指定されている場合、更新を行う。※2の場合百の位->時間に直すとhoursの部分
            if (figure.second.num_intDigit >= 2)
            {
                FigureSpriteSettings colonSettings = figure.second;
                colonSettings.cutStartPoint = { 352.f,0.f };
                float colonPosDiff = kDistance_symbolColon_;
                AdaptSettings_Symbol(figure.first, "Colon", -colonPosDiff, colonSettings);
            }

            // 変更フラグをfalse
            figure.second.isDirty = false;
        }

        // それぞれのspriteの座標や、切り抜き位置などを値に合わせ変更
        float num = *figure.second.num;
        // 今の値が何桁なのかを確認する
        int32_t digitNum = Math::Function::DigitNum((uint32_t)num);
        // 整数部分
        for (int32_t i = 0; i < figure.second.num_intDigit; i++)
        {
            const std::string& key = SynthesisName(figure.first, i);
            // UIクラスから、FigureUIで管理している分の数字UIのspriteのptrを取得する
            auto spPtr = uiPtr_->GetUISpritePtr(key);
            // 桁の値抽出
            const int32_t digitValue = Math::Function::DigitValue(num, i);
            // 切り抜きの始点
            Vector2 cutStartPoint = { kDefault_pictureLength_ * digitValue, 0.f };


            // 表示形式が"Format::TIMER"である
            if (figure.second.format == Format::TIMER)
            {
                // iの値が2未満の時（秒数部分2桁の時）
                if (i < 2)
                {
                    // "num"の値を秒数とした時に、分と秒で分けたときの、残った秒数分
                    int32_t seconds = (int32_t)num % 60;
                    int32_t digitValue_seconds = Math::Function::DigitValue((float)seconds, i);
                    cutStartPoint = { kDefault_pictureLength_ * digitValue_seconds, 0.f };
                }
                // iの値が2~3の時
                else if (i >= 2 && i < 4)
                {
                    // "num"の値を秒数として、分に変換するために60で割る。その後キャストすることで、余りは切り捨てる。
                    int32_t minutes = (int32_t)num / 60;
                    // 60を超えている場合に備え、剰余を使用する
                    minutes = minutes % 60;
                    int32_t digitValue_minutes = Math::Function::DigitValue((float)minutes, i - 2);
                    cutStartPoint = { kDefault_pictureLength_ * digitValue_minutes, 0.f };
                }
                // iの値が4の時
                else
                {
                    // "num"の値を秒数として、分に変換するために60で割る。その後キャストすることで、余りは切り捨てる。
                    int32_t minutes = (int32_t)num / 60;
                    // "minutes"の値を分数として、時間に変換するために60で割る。その後キャストすることで、余りは切り捨てる。
                    int32_t hours = minutes / 60;
                    int32_t digitValue_hours = Math::Function::DigitValue((float)hours, i - 4);
                    cutStartPoint = { kDefault_pictureLength_ * digitValue_hours, 0.f };
                }
            }
            //##設定 ※※[表示する数字を決定しているのはこの部分]※※
            spPtr->SetCutStartPoint(cutStartPoint);


            // 表示範囲が"MAX"である
            if (figure.second.scope == Scope::MAX)
            {
                // 桁数が"figure.second.num_intDigit"桁未満である
                if (digitNum < figure.second.num_intDigit)
                {
                    // iが0以外である && for分のiの値が"digitNum"以上である
                    if (i != 0 && i >= digitNum) // 小数点以下の値の時の為、1桁目の値は0でも消さない。
                    {
                        // 今の値より大きい桁部分の0表記のスプライトの透明度を0を非表示にして描画しない。
                        spPtr->SetAlpha(0.f);
                    }
                }
                else
                {
                    // 今の値より大きい桁部分は存在しないので、スプライトの透明度を設定されている値に戻す。
                    spPtr->SetAlpha(figure.second.alpha);
                }
            }

            uiPtr_->Update(key);
        }

        // 小数点以下部分
        for (int32_t i = 0; i > -figure.second.num_floatDigit; i--)
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
        // 小数点部分
        uiPtr_->Update(figure.first + "DecimalPoint");
        // 時計用のコロン部分
        uiPtr_->Update(figure.first + "Colon");
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
        for (int32_t i = 0; i < figure.second.num_intDigit; i++)
        {
            const std::string& key = SynthesisName(figure.first, i);
            uiPtr_->Draw(key);
        }
        // 小数点以下部分
        for (int32_t i = 0; i > -figure.second.num_floatDigit; i--)
        {
            const std::string& key = SynthesisName(figure.first, i - 1);
            uiPtr_->Draw(key);
        }
        // 小数点部分
        // 小数点以下の桁数が0より大きく指定されている場合、更新を行う。
        if (figure.second.num_floatDigit > 0)
        {
            uiPtr_->Draw(figure.first + "DecimalPoint");
        }
        // 時計用のコロン部分
        if (figure.second.format == Format::TIMER)
        {
            //  整数部の桁数が2以上の値が指定されている場合、更新を行う。※2の場合百の位->時間に直すとhoursの部分
            if (figure.second.num_intDigit >= 2)
            {
                uiPtr_->Draw(figure.first + "Colon");
            }
        }
    }
}

void FigureUI::Finalize(void)
{
    // 残ってるkey分全て、uiから抹消
    for (const auto& figure : umap_figures_) {
        // Registerと同じく、[桁数の数]と同じだけ消す
        //#figure.second.num_intDigitの分抹消
        for (int32_t i = 0; i < figure.second.num_intDigit; i++) { uiPtr_->UnRegister(SynthesisName(figure.first, i)); }
        //#figure.second.num_floatDigitの分抹消
        for (int32_t i = 0; i > -figure.second.num_floatDigit; i--) { uiPtr_->UnRegister(SynthesisName(figure.first, i - 1)); }
        //#小数点の分抹消
        uiPtr_->UnRegister(figure.first + "DecimalPoint");
        //#時計用のコロンの分抹消
        uiPtr_->UnRegister(figure.first + "Colon");
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
    //#arg_settings.num_intDigitの分登録
    for (int32_t i = 0; i < arg_settings.num_intDigit; i++) { AdaptSettings_Number(arg_key, i, arg_settings); }
    //#arg_settings.num_floatDigitの分登録
    for (int32_t i = 0; i > -arg_settings.num_floatDigit; i--) { AdaptSettings_Number(arg_key, i - 1, arg_settings); }
    //#小数点の分登録
    FigureSpriteSettings decimalSettings = arg_settings;
    decimalSettings.cutStartPoint = { 320.f,0.f };
    AdaptSettings_Symbol(arg_key, "DecimalPoint", kDistance_symbolDecimal_, decimalSettings);
    //#時計用のコロンの分登録
    FigureSpriteSettings colonSettings = arg_settings;
    colonSettings.cutStartPoint = { 352.f,0.f };
    float colonPosDiff = kDistance_symbolColon_;
    AdaptSettings_Symbol(arg_key, "Colon", -colonPosDiff, colonSettings);

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

void FigureUI::AdaptSettings_Number(const std::string& arg_key, int32_t arg_num, const FigureSpriteSettings& arg_settings)
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
    Vector2 pos = { arg_settings.pos.x - kDefault_pictureLength_ * arg_num, arg_settings.pos.y };
    // [数字の桁数]がマイナスの場合、さらに小数点のスプライトの分だけ、座標をずらす
    if (arg_num < 0) { pos.x += kDefaultSpace_symbolDecimal_; }
    // 表示形式が"Format::TIMER"である
    if (arg_settings.format == Format::TIMER)
    {
        // 数字の桁数が2以上である
        if (arg_num >= 2)
        {
            // コロン(:)の分だけ、座標をずらす
            pos.x -= kDefaultSpace_symbolColon_;
        }
    }
    spPtr->SetPosition(pos);
}

void FigureUI::AdaptSettings_Symbol(const std::string& arg_key, const std::string& arg_symbolName, float arg_posX, const FigureSpriteSettings& arg_settings)
{
    const std::string& key = arg_key + arg_symbolName;

    uiPtr_->Register(key, kDefault_figurePath_);
    // ここで例外スローしたら、同名で登録している
    auto spPtr = uiPtr_->GetUISpritePtr(key);
    spPtr->SetAlpha(arg_settings.alpha);
    spPtr->SetSize(arg_settings.size);
    spPtr->SetScale(arg_settings.scale);
    spPtr->SetAnchorPoint(arg_settings.anchorPoint);
    spPtr->SetCutStartPoint(arg_settings.cutStartPoint);
    spPtr->SetCutLength(arg_settings.cutLength);

    // 座標（基点.x + 引数で設定した値, 基点.y）
    const Vector2 pos = { arg_settings.pos.x + arg_posX , arg_settings.pos.y };
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
