#include "Screen.h"
#include "WndAPI.h"
#include "MathUtil.h"

Screen::Screen(Matrix4* arg_matView, Matrix4* mat_prj)
    : mat_viewPtr_(arg_matView), mat_prjPtr_(mat_prj)
{
    screen_width_ = WndAPI::kWidth_;
    screen_height_ = WndAPI::kHeight_;
}

Vector2 Screen::WorldToScreenPoint(const Matrix4& arg_matWorld) const
{
    return Math::Function::WorldToScreen(arg_matWorld, (int32_t)screen_width_, (int32_t)screen_height_, *mat_viewPtr_, *mat_prjPtr_);
}

Vector3 Screen::ScreenToWorldPoint(const Vector2& arg_scPos, float arg_fZ) const
{
    return Math::Function::ScreenToWorld(arg_scPos, arg_fZ, (int32_t)screen_width_, (int32_t)screen_height_, *mat_viewPtr_, *mat_prjPtr_);
}
