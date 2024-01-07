#pragma once
#include "Object3D.h"

class ExclamationMark :
    public Object3D
{
public:
    //>> 関数
    ExclamationMark(void);
    ~ExclamationMark(void) = default;

private:
    //>> 変数
    Axis3 axes_;
    Vector3 vec3_newUp_; // 新規上ベクトル
    Transform transform_;

};

