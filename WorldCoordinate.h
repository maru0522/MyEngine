#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include "Quaternion.h"

class WorldCoordinate
{
public:
    // 定義
    struct Axis3
    {
        Quaternion forward;
        Quaternion right;
        Quaternion up;

        Axis3(void)
        {
            forward = Math::QuaternionF::Identity();
            right = Math::QuaternionF::Identity();
            up = Math::QuaternionF::Identity();
        }
    };

    // 関数
    WorldCoordinate(void);
    WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& rot); // 動作未確認

    void Update(void);
    void Reset(void);
private:
    // 変数
    Vector3 position_;
    Vector3 scale_;
    Vector3 eular_;
    Axis3 axis_;

    Matrix4 matWorld_;

public:
    // setter・getter
    inline void SetMatWorld(const Matrix4& matWorld) { matWorld_ = matWorld; }
    inline void SetRotation(const Vector3& eular) { eular_ = eular; }
    inline void SetPosition(const Vector3& pos) { position_ = pos; }
    inline void SetScale(const Vector3& scale) { scale_ = scale; }

    inline void SetAxisForward(const Quaternion& forward) { axis_.forward = forward; }
    inline void SetAxisRight(const Quaternion& right) { axis_.right = right; }
    inline void SetAxisUp(const Quaternion& up) { axis_.up = up; }
    inline void SetAxis(const Axis3& axis3) { axis_ = axis3; }

    inline const Matrix4& GetMatWorld(void) { return matWorld_; }
    inline const Vector3& GetPosition(void) { return position_; }
    inline const Vector3& GetRotation(void) { return eular_; }
    inline const Vector3& GetScale(void) { return scale_; }

    inline Vector3* GetPosPtr(void) { return &position_; }

    inline Vector3 GetAxisX(void) const { return { matWorld_.m[0][0],matWorld_.m[0][1],matWorld_.m[0][2] }; }
    inline Vector3 GetAxisY(void) const { return { matWorld_.m[1][0],matWorld_.m[1][1],matWorld_.m[1][2] }; }
    inline Vector3 GetAxisZ(void) const { return { matWorld_.m[2][0],matWorld_.m[2][1],matWorld_.m[2][2] }; }

    //inline const Vector3& GetForwardVec(void) { return GetAxisZ(); }
    //inline const Vector3& GetRightVec(void) { return GetAxisX(); }
    //inline const Vector3& GetUpVec(void) { return GetAxisY(); }
};
