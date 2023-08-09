#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include "Transform.h"
#include "Quaternion.h"

struct Coordinate final
{
    Coordinate(void) { Initialize(); }
    Coordinate(const Matrix4& world, const Matrix4& sca, const Matrix4& rot, const Matrix4& pos);

    // 値を初期化する
    void Initialize(void);
    // 行列をscale,rotate,positionの順に合成する
    void CompositionSRT(void);

    Matrix4 mat_world;
    Matrix4 mat_sca;
    Matrix4 mat_rot;
    Matrix4 mat_pos;

    // getter
    // 行列から結果的な軸ベクトルを抜き出す。
    Vector3 GetMatAxisX(void) const { return { mat_world.m[0][0],mat_world.m[0][1],mat_world.m[0][2] }; }
    Vector3 GetMatAxisY(void) const { return { mat_world.m[1][0],mat_world.m[1][1],mat_world.m[1][2] }; }
    Vector3 GetMatAxisZ(void) const { return { mat_world.m[2][0],mat_world.m[2][1],mat_world.m[2][2] }; }
};
