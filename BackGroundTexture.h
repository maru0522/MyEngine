#pragma once
#include "PostEffect.h"
#include <functional>

class BackGroundTexture :
    public PostEffect
{
private:
    //>> singleton
    BackGroundTexture(void) {};
    ~BackGroundTexture(void) {};
    BackGroundTexture(const BackGroundTexture&) = delete;
    BackGroundTexture& operator=(const BackGroundTexture&) = delete;

public:
    //>> 関数
    static BackGroundTexture* GetInstance(void);

    void Initialize(void) override;
    void Update(void);
    void BeginWrite(void);
    void ExecuteWrite(void);
    void EndWrite(void);
    void Draw(void) override;

private:

    //>> 変数
    std::function<void(void)> func_draw_;
    bool is_setFunc_{};

public:
    //>> setter
    void Set(const std::function<void(void)>& arg_funcPtr) { func_draw_ = arg_funcPtr; }
};

