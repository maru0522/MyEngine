#pragma once
#include "Vector3.h"
#include "Matrix4.h"

class WorldCoordinate
{
public:
    // ä÷êî
    WorldCoordinate(void);
    WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& rot);

    void Update(void);
    void Reset(void);

private:
    // ïœêî
    Vector3 scale_;
    Vector3 position_;
    Vector3 rotation_;

    Matrix4 matWorld_;

public:
    // setterÅEgetter

    inline void SetMatWorld(const Matrix4& matWorld) { matWorld_ = matWorld; }
    inline void SetPosition(const Vector3& pos) { position_ = pos; }
    inline void SetRotation(const Vector3& rot) { rotation_ = rot; }
    inline void SetScale(const Vector3& scale) { scale_ = scale; }

    inline const Matrix4& GetMatWorld(void) { return matWorld_; }
    inline const Vector3& GetPosition(void) { return position_; }
    inline const Vector3& GetRotation(void) { return rotation_; }
    inline const Vector3& GetScale(void) { return scale_; }

    inline Vector3 GetAxisX(void) const { return { matWorld_.m[0][0],matWorld_.m[0][1],matWorld_.m[0][2] }; }
    inline Vector3 GetAxisY(void) const { return { matWorld_.m[1][0],matWorld_.m[1][1],matWorld_.m[1][2] }; }
    inline Vector3 GetAxisZ(void) const { return { matWorld_.m[2][0],matWorld_.m[2][1],matWorld_.m[2][2] }; }

    inline const Vector3& GetForwardVec(void) { return GetAxisZ(); }
    inline const Vector3& GetRightVec(void) { return GetAxisX(); }
    inline const Vector3& GetUpVec(void) { return GetAxisY(); }
};
