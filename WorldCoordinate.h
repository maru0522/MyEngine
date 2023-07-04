#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include "Quaternion.h"

class WorldCoordinate
{
public:
    // 定義
    struct Axis3Q
    {
        Quaternion forward;
        Quaternion right;
        Quaternion up;

        Axis3Q(void)
        {
            forward = Math::QuaternionF::MakeAxisAngle(Math::Vector::AxisZIdentity(), 0.f);
            right = Math::QuaternionF::MakeAxisAngle(Math::Vector::AxisXIdentity(), 0.f);
            up = Math::QuaternionF::MakeAxisAngle(Math::Vector::AxisYIdentity(), 0.f);
        }
    };

    union Rotate
    {
        Vector3 eular;
        Axis3Q axisQ;

        Rotate(bool isEular);
        Rotate(const Vector3& eular) : eular(eular) {}
        Rotate(const Axis3Q& axis3q) : axisQ(axis3q) {}
    };

    // 関数
    WorldCoordinate(void);
    WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& rot); // 動作未確認

    void Update(void);
    void Reset(void);

private:
    // 変数
    Vector3 scale_;
    Vector3 position_;
    Rotate rotation_;

    Matrix4 matWorld_;

public:
    // setter・getter

    inline void SetMatWorld(const Matrix4& matWorld) { matWorld_ = matWorld; }
    inline void SetRotation(const Vector3& rot) { rotation_.eular = rot; }
    inline void SetRotation(const Axis3Q& rot) { rotation_.axisQ = rot; }
    inline void SetPosition(const Vector3& pos) { position_ = pos; }
    inline void SetScale(const Vector3& scale) { scale_ = scale; }

    inline const Vector3& GetRotation(void) { return rotation_.eular; }
    inline const Matrix4& GetMatWorld(void) { return matWorld_; }
    inline const Vector3& GetPosition(void) { return position_; }
    inline const Vector3& GetScale(void) { return scale_; }

    inline Vector3* GetPosPtr(void) { return &position_; }

    inline Vector3 GetAxisX(void) const { return { matWorld_.m[0][0],matWorld_.m[0][1],matWorld_.m[0][2] }; }
    inline Vector3 GetAxisY(void) const { return { matWorld_.m[1][0],matWorld_.m[1][1],matWorld_.m[1][2] }; }
    inline Vector3 GetAxisZ(void) const { return { matWorld_.m[2][0],matWorld_.m[2][1],matWorld_.m[2][2] }; }

    //inline const Vector3& GetForwardVec(void) { return GetAxisZ(); }
    //inline const Vector3& GetRightVec(void) { return GetAxisX(); }
    //inline const Vector3& GetUpVec(void) { return GetAxisY(); }
};
