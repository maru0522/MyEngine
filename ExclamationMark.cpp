#include "ExclamationMark.h"

ExclamationMark::ExclamationMark(void) : Object3D("Resources/model/exclamationMark/exclamationMark.obj")
{
    // 初期姿勢
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };
}
