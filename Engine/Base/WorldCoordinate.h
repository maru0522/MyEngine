#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include "Transform.h"
#include "Quaternion.h"

struct WorldCoordinate2 final
{
    WorldCoordinate2(void) { Initialize(); }
    WorldCoordinate2(const Matrix4& world, const Matrix4& sca, const Matrix4& rot, const Matrix4& pos);

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

//class WorldCoordinate
//{
//private:
//    // 変数
//    Vector3 position_;
//    Vector3 scale_;
//    Vector3 eular_;
//    Axis3 axes_; // axisの複数形
//
//    Matrix4 matWorld_;
//    bool isPriorityEular_;
//
//public:
//    // 関数
//    WorldCoordinate(void) : WorldCoordinate(Vector3{ 0,0,0 }, Vector3{ 1,1,1 }, Vector3{ 0,0,1 }, Vector3{ 1,0,0 }, Vector3{ 0,1,0 }) {}
//    WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& rot);
//    WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& forward, const Vector3& right, const Vector3& up);
//
//    void Update(void);
//    void Reset(void);
//
//    // setter
//    inline void SetIsPriority(bool isPriorityEular) { isPriorityEular_ = isPriorityEular; }
//    inline void SetMatWorld(const Matrix4& matWorld) { matWorld_ = matWorld; }
//    inline void SetRotation(const Vector3& eular) { eular_ = eular; }
//    inline void SetPosition(const Vector3& pos) { position_ = pos; }
//    inline void SetScale(const Vector3& scale) { scale_ = scale; }
//
//    inline void SetAxisForward(const Vector3& forward) { axes_.forward = forward; }
//    inline void SetAxisRight(const Vector3& right) { axes_.right = right; }
//    inline void SetAxisUp(const Vector3& up) { axes_.up = up; }
//    inline void SetAxes(const Axis3& axis3) { axes_ = axis3; }
//
//    // getter
//    inline bool GetIsPriority(void) { return isPriorityEular_; }
//    inline const Matrix4& GetMatWorld(void) { return matWorld_; }
//    inline const Vector3& GetPosition(void) { return position_; }
//    inline const Vector3& GetRotation(void) { return eular_; }
//    inline const Vector3& GetScale(void) { return scale_; }
//
//    inline Vector3* GetPosPtr(void) { return &position_; }
//
//    // 行列から結果的な軸ベクトルを抜き出す。
//    inline Vector3 GetMatAxisX(void) const { return { matWorld_.m[0][0],matWorld_.m[0][1],matWorld_.m[0][2] }; }
//    inline Vector3 GetMatAxisY(void) const { return { matWorld_.m[1][0],matWorld_.m[1][1],matWorld_.m[1][2] }; }
//    inline Vector3 GetMatAxisZ(void) const { return { matWorld_.m[2][0],matWorld_.m[2][1],matWorld_.m[2][2] }; }
//
//    // 自分で設定したベクトルを取得する。
//    inline const Vector3& GetForwardVec(void) { return axes_.forward; }
//    inline const Vector3& GetRightVec(void) { return axes_.right; }
//    inline const Vector3& GetUpVec(void) { return axes_.up; }
//    inline const Axis3& GetAxes(void) { return axes_; }
//};
