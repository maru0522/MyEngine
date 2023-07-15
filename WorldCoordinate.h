#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include "Quaternion.h"

struct Axis3
{
    Quaternion forward;
    Quaternion right;
    Quaternion up;

    Axis3(void) : Axis3(Math::QuaternionF::Identity(), Math::QuaternionF::Identity(), Math::QuaternionF::Identity()) {}
    Axis3(const Quaternion& f, const Quaternion& r, const Quaternion& u)
    {
        forward = f;
        right = r;
        up = u;
    }
};

class WorldCoordinate
{
private:
    // 変数
    Vector3 position_;
    Vector3 scale_;
    Vector3 eular_;
    Axis3 axes_; // axisの複数形

    Matrix4 matWorld_;
    bool isPriorityEular_;

public:
    // 関数
    WorldCoordinate(void) : WorldCoordinate(Vector3{ 0,0,0 }, Vector3{ 1,1,1 }, Vector3{ 0,0,1 }, Vector3{ 1,0,0 }, Vector3{ 0,1,0 }) {}
    WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& rot);
    WorldCoordinate(const Vector3& pos, const Vector3& scale, const Quaternion& forward, const Quaternion& right, const Quaternion& up);

    void Update(void);
    void Reset(void);

    // setter
    inline void SetIsPriority(bool isPriorityEular) { isPriorityEular_ = isPriorityEular; }
    inline void SetMatWorld(const Matrix4& matWorld) { matWorld_ = matWorld; }
    inline void SetRotation(const Vector3& eular) { eular_ = eular; }
    inline void SetPosition(const Vector3& pos) { position_ = pos; }
    inline void SetScale(const Vector3& scale) { scale_ = scale; }

    inline void SetAxisForward(const Quaternion& forward) { axes_.forward = forward; }
    inline void SetAxisRight(const Quaternion& right) { axes_.right = right; }
    inline void SetAxisUp(const Quaternion& up) { axes_.up = up; }
    inline void SetAxis(const Axis3& axis3) { axes_ = axis3; }

    // getter
    inline bool GetIsPriority(void) { return isPriorityEular_; }
    inline const Matrix4& GetMatWorld(void) { return matWorld_; }
    inline const Vector3& GetPosition(void) { return position_; }
    inline const Vector3& GetRotation(void) { return eular_; }
    inline const Vector3& GetScale(void) { return scale_; }

    inline Vector3* GetPosPtr(void) { return &position_; }

    // 行列から結果的な軸ベクトルを抜き出す。
    inline Vector3 GetMatAxisX(void) const { return { matWorld_.m[0][0],matWorld_.m[0][1],matWorld_.m[0][2] }; }
    inline Vector3 GetMatAxisY(void) const { return { matWorld_.m[1][0],matWorld_.m[1][1],matWorld_.m[1][2] }; }
    inline Vector3 GetMatAxisZ(void) const { return { matWorld_.m[2][0],matWorld_.m[2][1],matWorld_.m[2][2] }; }

    // 自分で設定したベクトルを抜き出す。
    inline const Quaternion& GetForwardVec(void) { return axes_.forward; }
    inline const Quaternion& GetRightVec(void) { return axes_.right; }
    inline const Quaternion& GetUpVec(void) { return axes_.up; }
};
