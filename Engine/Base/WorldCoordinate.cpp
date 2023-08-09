#include "WorldCoordinate.h"
#include "Input.h"

using namespace Math;

Coordinate::Coordinate(const Matrix4& world, const Matrix4& sca, const Matrix4& rot, const Matrix4& pos)
    : mat_world(world), mat_sca(sca), mat_rot(rot), mat_pos(pos)
{
}

void Coordinate::CompositionSRT(void)
{
    mat_world *= mat_sca;
    mat_world *= mat_rot;
    mat_world *= mat_pos;
}

void Coordinate::Initialize(void)
{
    Matrix4 identity = Math::Mat4::Identity();

    mat_world = identity;
    mat_sca = identity;
    mat_rot = identity;
    mat_pos = identity;
}
