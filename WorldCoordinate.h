#pragma once
#include "Vector3.h"
#include "Matrix4.h"

class WorldCoordinate
{
public:
    // ä÷êî
    WorldCoordinate(void) {};
    WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& rot);

    void Update(void);

    inline const Matrix4& GetMatWorld(void) { return matWorld_; }
    inline const Vector3& GetPosition(void) { return position_; }
    inline const Vector3& GetRotation(void) { return rotation_; }
    inline const Vector3& GetScale(void) { return scale_; }

    inline void SetMatWorld(const Matrix4& matWorld) { matWorld_ = matWorld; }
    inline void SetPosition(const Vector3& pos) { position_ = pos; }
    inline void SetRotation(const Vector3& rot) { rotation_ = rot; }
    inline void SetScale(const Vector3& scale) { scale_ = scale; }

private:
    // ïœêî
    Vector3 scale_{ 1.f,1.f,1.f };
    Vector3 position_{ 0.f,0.f,0.f };
    Vector3 rotation_{ 0.f,0.f,0.f };

    Matrix4 matWorld_{ Math::Matrix::Identity() };
};
