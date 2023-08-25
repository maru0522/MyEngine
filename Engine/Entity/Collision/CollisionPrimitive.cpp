#include "CollisionChecker.h"
#include "CollisionPrimitive.h"

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // ���Ƌ�
    bool isHit = CollisionChecker::SphereToSphere(*this, *arg_Shpere);

    if (isHit)
    {
        colInfo_ = { arg_Shpere->center, arg_Shpere->radius, arg_Shpere->GetID() };
        arg_Shpere->SetColInfo(CollisionInfo{ center,radius,id_ });

        // callback
        //onTrigger_();
        //arg_Shpere->GetOnTrigger()();

        onCollision_();
        arg_Shpere->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // ���ƕ���
    bool isHit = CollisionChecker::SphereToPlane(*this, *arg_Plane);

    if (isHit)
    {
        colInfo_ = { arg_Plane->normal, arg_Plane->distance, arg_Plane->GetID() };
        arg_Plane->SetColInfo(CollisionInfo{ center,radius,id_ });

        // callback
        //onTrigger_();
        //arg_Plane->GetOnTrigger()();

        onCollision_();
        arg_Plane->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // ���Ɠ_
    bool isHit = CollisionChecker::SphereToPoint(*this, *arg_Point);

    if (isHit)
    {
        colInfo_.v = arg_Point->pos;
        colInfo_.id = arg_Point->GetID();
        arg_Point->SetColInfo(CollisionInfo{ center,radius,id_ });

        onCollision_();
        arg_Point->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // ���ƒ�����
    bool isHit = CollisionChecker::SphereToAABB(*this, *arg_AABB);

    if (isHit)
    {
        colInfo_ = { arg_AABB->center, arg_AABB->radius.x, arg_AABB->GetID() };
        colInfo_.f2 = arg_AABB->radius.y;
        colInfo_.f3 = arg_AABB->radius.z;
        arg_AABB->SetColInfo(CollisionInfo{ center,radius,id_ });

        onCollision_();
        arg_AABB->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // ���ʂƕ���
    bool isHit = CollisionChecker::PlaneToPlane(*this, *arg_Plane);

    if (isHit)
    {
        colInfo_ = { arg_Plane->normal, arg_Plane->distance, arg_Plane->GetID() };
        arg_Plane->SetColInfo(CollisionInfo{ normal,distance,id_ });

        // callback
        onCollision_();
        arg_Plane->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // ���ʂƋ�
    bool isHit = CollisionChecker::SphereToPlane(*arg_Shpere, *this);

    if (isHit)
    {
        colInfo_ = { arg_Shpere->center, arg_Shpere->radius, arg_Shpere->GetID() };
        arg_Shpere->SetColInfo(CollisionInfo{ normal,distance,id_ });

        // callback
        onCollision_();
        arg_Shpere->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // ���ʂƓ_
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // ���ʂƒ�����
    if (arg_AABB)
    {

    }
    return false;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // �����̂Ƌ�
    bool isHit = CollisionChecker::SphereToAABB(*arg_Shpere, *this);

    if (isHit)
    {
        colInfo_ = { arg_Shpere->center,arg_Shpere->radius,arg_Shpere->GetID() };
        CollisionInfo colInfo(center, radius.x, id_);
        colInfo.f2 = radius.y;
        colInfo.f3 = radius.z;
        arg_Shpere->SetColInfo(colInfo);

        onCollision_();
        arg_Shpere->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // �����̂ƕ���
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // �����̂Ɠ_
    bool isHit = CollisionChecker::AABBToPoint(*this, *arg_Point);

    if (isHit)
    {
        colInfo_.v = arg_Point->pos;
        colInfo_.id = arg_Point->GetID();
        CollisionInfo colInfo(center, radius.x, id_);
        colInfo.f2 = radius.y;
        colInfo.f3 = radius.z;
        arg_Point->SetColInfo(colInfo);

        onCollision_();
        arg_Point->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // �����̂ƒ�����
    bool isHit = CollisionChecker::AABBToAABB(*this, *arg_AABB);

    if (isHit)
    {
        colInfo_ = { arg_AABB->center, arg_AABB->radius.x, arg_AABB->GetID() };
        colInfo_.f2 = radius.y;
        colInfo_.f3 = radius.z;
        CollisionInfo colInfo(center, radius.x, id_);
        colInfo.f2 = radius.y;
        colInfo.f3 = radius.z;
        arg_AABB->SetColInfo(colInfo);

        onCollision_();
        arg_AABB->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // �_�Ƌ�
    bool isHit = CollisionChecker::SphereToPoint(*arg_Shpere, *this);

    if (isHit)
    {
        colInfo_ = { arg_Shpere->center,arg_Shpere->radius,arg_Shpere->GetID() };
        CollisionInfo colInfo{};
        colInfo.v = pos;
        colInfo.id = id_;
        arg_Shpere->SetColInfo(colInfo);

        onCollision_();
        arg_Shpere->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // �_�ƕ���
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // �_�Ɠ_
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // �_�ƒ�����
    bool isHit = CollisionChecker::AABBToPoint(*arg_AABB, *this);

    if (isHit)
    {
        colInfo_ = { arg_AABB->center, arg_AABB->radius.x, arg_AABB->GetID() };
        colInfo_.f2 = arg_AABB->radius.y;
        colInfo_.f3 = arg_AABB->radius.z;
        CollisionInfo colInfo{};
        colInfo.v = pos;
        colInfo.id = id_;
        arg_AABB->SetColInfo(colInfo);

        onCollision_();
        arg_AABB->GetOnCollision()();
    }

    return isHit;
}
