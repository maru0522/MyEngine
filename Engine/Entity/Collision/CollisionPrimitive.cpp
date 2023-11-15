#include "CollisionChecker.h"
#include "CollisionPrimitive.h"

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 球と球

    bool isHit = CollisionChecker::SphereToSphere(*this, *arg_Shpere);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Shpere);
        arg_Shpere->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Shpere->GetCallback_onCollision()) { arg_Shpere->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // 球と平面

    bool isHit = CollisionChecker::SphereToPlane(*this, *arg_Plane);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Plane);
        arg_Plane->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Plane->GetCallback_onCollision()) { arg_Plane->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // 球と点
    bool isHit = CollisionChecker::SphereToPoint(*this, *arg_Point);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Point);
        arg_Point->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Point->GetCallback_onCollision()) { arg_Point->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // 球と直方体
    bool isHit = CollisionChecker::SphereToAABB(*this, *arg_AABB, &arg_AABB->inter);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_AABB);
        arg_AABB->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_AABB->GetCallback_onCollision()) { arg_AABB->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // 平面と平面
    bool isHit = CollisionChecker::PlaneToPlane(*this, *arg_Plane);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Plane);
        arg_Plane->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Plane->GetCallback_onCollision()) { arg_Plane->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 平面と球
    bool isHit = CollisionChecker::SphereToPlane(*arg_Shpere, *this);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Shpere);
        arg_Shpere->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Shpere->GetCallback_onCollision()) { arg_Shpere->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // 平面と点
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // 平面と直方体
    if (arg_AABB)
    {

    }
    return false;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 直方体と球
    bool isHit = CollisionChecker::SphereToAABB(*arg_Shpere, *this, &this->inter);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Shpere);
        arg_Shpere->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Shpere->GetCallback_onCollision()) { arg_Shpere->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // 直方体と平面
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // 直方体と点
    bool isHit = CollisionChecker::AABBToPoint(*this, *arg_Point);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Point);
        arg_Point->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Point->GetCallback_onCollision()) { arg_Point->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // 直方体と直方体
    bool isHit = CollisionChecker::AABBToAABB(*this, *arg_AABB);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_AABB);
        arg_AABB->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_AABB->GetCallback_onCollision()) { arg_AABB->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 点と球
    bool isHit = CollisionChecker::SphereToPoint(*arg_Shpere, *this);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Shpere);
        arg_Shpere->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Shpere->GetCallback_onCollision()) { arg_Shpere->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // 点と平面
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // 点と点
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // 点と直方体
    bool isHit = CollisionChecker::AABBToPoint(*arg_AABB, *this);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_AABB);
        arg_AABB->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_AABB->GetCallback_onCollision()) { arg_AABB->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{
    if (arg_Shpere)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{
    if (arg_AABB)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{
    bool isHit = CollisionChecker::SphereToRay(*arg_Shpere, *this);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Shpere);
        arg_Shpere->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Shpere->GetCallback_onCollision()) { arg_Shpere->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{
    if (arg_AABB)
    {

    }
    return false;
}

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}
