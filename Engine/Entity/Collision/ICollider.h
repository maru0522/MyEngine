#pragma once
#include <string>
#include "Vector2.h"
#include "Vector3.h"
#include <functional>
#include <forward_list>


// 前方宣言
class CollisionManager;
// 前方宣言2
namespace CollisionPrimitive
{
    struct SphereCollider;
    struct PlaneCollider;
    struct PointCollider;
    struct AABBCollider;
    struct OBBCollider;
    struct RayCollider;
}


class ColliderId
{
public:
    //>> 関数
    ColliderId(void) = default;
    virtual ~ColliderId(void) = default;

    // 接触相手のidをfListに保存
    void RecordIds(void);

protected:
    // 現在のリストに指定した名前があるか
    bool IsExistSameId(const std::string& arg_id);
    // 1Frame前のリストに指定した名前があるか
    bool IsExistSameIdPre(const std::string& arg_idPre);
    // 接触相手のidをリストに追加
    void AddDetectId(const std::string& arg_id) { detect_ids_.push_front(arg_id); }

    //>> 変数
    std::string id_{};
    std::forward_list<std::string> detect_ids_;
    std::forward_list<std::string> detect_idsPre_;

public:
    //>> setter
    void SetID(const std::string& arg_id) { id_ = arg_id; }

    // getter
    const std::string& GetID(void) { return id_; }
};

class ColliderCallBack
{
public:
    //>> 関数
    ColliderCallBack(void) = default;
    virtual ~ColliderCallBack(void) = default;

protected:
    //>> 変数
    std::function<void(void)> callback_onTrigger_;
    std::function<void(void)> callback_onCollision_;
    std::function<void(void)> callback_onRelease_;

public:
    //>> setter
    void SetCallback_onTrigger(const std::function<void(void)>& arg_callback) { callback_onTrigger_ = arg_callback; }
    void SetCallback_onCollision(const std::function<void(void)>& arg_callback) { callback_onCollision_ = arg_callback; }
    void SetCallback_onRelease(const std::function<void(void)>& arg_callback) { callback_onRelease_ = arg_callback; }

    // getter
    const std::function<void(void)>& GetCallback_onTrigger(void) { return callback_onTrigger_; }
    const std::function<void(void)>& GetCallback_onCollision(void) { return callback_onCollision_; }
    const std::function<void(void)>& GetCallback_onRelease(void) { return callback_onRelease_; }
};

class ICollider : public ColliderId, public ColliderCallBack
{
public:
    //>> 関数
    ICollider(void) = default;
    virtual ~ICollider(void) = default;
    virtual bool Dispatch(ICollider* other) = 0;

    virtual bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) = 0;
    virtual bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) = 0;
    virtual bool Col(CollisionPrimitive::PointCollider* arg_Point) = 0;
    virtual bool Col(CollisionPrimitive::AABBCollider* arg_AABB) = 0;
    virtual bool Col(CollisionPrimitive::OBBCollider* arg_OBB) = 0;
    virtual bool Col(CollisionPrimitive::RayCollider* arg_Ray) = 0;

    // 触れた瞬間か確認し、trueならcallbackを実行
    void Check_onTrigger(void);
    // 離した瞬間か確認し、trueならcallbackを実行
    void Check_onRelease(void);

protected:
    bool IsTrigger(void);
    bool IsRelease(void);

    //>> 変数
    ICollider* other_{};

public:
    //>> setter
    void SetOther(ICollider* arg_col);

    //>> getter
    ICollider* GetOther(void) { return other_; }
};
