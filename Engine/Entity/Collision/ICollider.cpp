#include "ICollider.h"

void ColliderId::RecordIds(void)
{
    detect_idsPre_ = detect_ids_;
    detect_ids_.clear();
}

bool ColliderId::IsExistSameId(const std::string& arg_id)
{
    for (auto& id : detect_ids_)
    {
        // 接触相手の名前が、一覧にあるか検索
        if (id == arg_id) { return true; }
    }

    return false;
}

bool ColliderId::IsExistSameIdPre(const std::string& arg_idPre)
{
    for (auto& id : detect_idsPre_)
    {
        // 接触相手の名前が、一覧（1Frame前）にあるか検索
        if (id == arg_idPre) { return true; }
    }

    return false;
}


bool ICollider::IsTrigger(void)
{
    // 接触相手のid
    const std::string& id = other_->GetID();
    // 接触した瞬間かどうか
    bool isTrigger = IsExistSameId(id) && IsExistSameIdPre(id) == false;

    return isTrigger;
}

bool ICollider::IsRelease(void)
{
    // 接触相手のid
    const std::string& id = other_->GetID();
    // 離した瞬間かどうか
    bool isRelease = IsExistSameId(id) == false && IsExistSameIdPre(id);

    return isRelease;
}

void ICollider::SetOther(ICollider* arg_col)
{
    // 接触相手のptrを保存
    other_ = arg_col;
    // 接触相手のidをリストに追加
    AddDetectId(arg_col->GetID());
}

void ICollider::Check_onTrigger(void)
{
    // 触れた瞬間である
    if (IsTrigger())
    {
        // nullチェックとonTriggerのCallback実行
        if (GetCallback_onTrigger()) { GetCallback_onTrigger()(); }
    }
}

void ICollider::Check_onRelease(void)
{
    // 離した瞬間である
    if (IsRelease())
    {
        // nullチェックとonReleaseのCallback実行
        if(GetCallback_onRelease()) { GetCallback_onRelease()(); }
    }
}
