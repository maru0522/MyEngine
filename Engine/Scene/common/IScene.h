#pragma once
/**
 * @file Scene.h
 * @brief 各シーンの基底となるクラスが宣言されたファイル
 */

class IScene
{
public:
    // 関数
    virtual ~IScene(void) = default;

    virtual void Initialize(void) = 0;
    virtual void Update(void) = 0;
    virtual void Draw3d(void) = 0;
    virtual void Draw2dBack(void) = 0;
    virtual void Draw2dFore(void) = 0;
    virtual void Finalize(void) = 0;
};

