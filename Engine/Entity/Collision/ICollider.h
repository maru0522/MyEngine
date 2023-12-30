#pragma once
#include <string>
#include "Vector2.h"
#include "Vector3.h"
#include <functional>
#include <forward_list>
/**
 * @file ICollider.h
 * @brief 矩形コライダーの基底となるクラスが宣言されたファイル
 */

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

struct ColliderCallBack
{
public:
    //>> 関数
    ColliderCallBack(void) = default;
    virtual ~ColliderCallBack(void) = default;

    //>> 変数
    std::function<void(void)> callback_onTrigger_;
    std::function<void(void)> callback_onCollision_;
    std::function<void(void)> callback_onRelease_;
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

    // 接触判定時に、互いに接触相手をリセットするための関数
    void InitOther(void) { other_ = nullptr; } // リセットしないと前回時の接触相手で、トリガとリリースの判定を行ってしまう。

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
