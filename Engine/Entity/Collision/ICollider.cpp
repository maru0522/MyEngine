#include "ICollider.h"

void ICollider::ExecuteCollisionCallback(void)
{
    //// 接触した瞬間
    //if (is_preCol_ == false && is_col_)
    //{
    //    // nullチェック
    //    if (onTrigger_) { onTrigger_(); }
    //}

    //// 接触している
    //if (is_col_)
    //{
    //    // nullチェック
    //    if (onCollision_) { onCollision_(); }
    //}

    //// 接触が終わった瞬間
    //if (is_preCol_ && is_col_ == false)
    //{
    //    // nullチェック
    //    if (onRelease_) { onRelease_(); }
    //}

    if (onTrigger_) { onTrigger_(); }
    if (onCollision_) { onCollision_(); }
    if (onRelease_) { onRelease_(); }
}
