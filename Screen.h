#pragma once
#include "Matrix4.h"
#include "Vector2.h"

class Screen
{
public:
    //>> 関数
    Screen(Matrix4* arg_matView, Matrix4* mat_prj);
    Screen(void) : Screen(nullptr, nullptr) {}
    ~Screen(void) = default;

    Vector2 WorldToScreenPoint(const Matrix4& arg_matWorld) const;
    Vector3 ScreenToWorldPoint(const Vector2& arg_scPos, float arg_fZ) const;

private:
    //>> 変数
    Matrix4* mat_viewPtr_;
    Matrix4* mat_prjPtr_;
    size_t screen_width_;
    size_t screen_height_;

public:
    //>> setter
    void SetMatViewPtr(Matrix4* arg_matView) { mat_viewPtr_ = arg_matView; }
    void SetMatPrjPtr(Matrix4* arg_matPrj) { mat_prjPtr_ = arg_matPrj; }
};

