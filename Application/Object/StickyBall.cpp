#include "StickyBall.h"
#include "SimplifyImGui.h"

StickyBall::StickyBall(CollisionManager* colMPtr) : Object3D("Resources/model/sphere/sphere.obj"), colMPtr_(colMPtr)
{
    // 初期位置
    transform_.position = { 0,0,0 };
    transform_.scale = { kRadius_,kRadius_,kRadius_ };

    // 初期姿勢
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };

    // コライダー設定
    colMPtr->Register(&collision_sphere_); // マネージャーに登録
    collision_sphere_.SetID("sticky_ball"); // tag設定
    collision_sphere_.callback_onCollision_ = std::bind(&StickyBall::OnCollision, this); // 衝突時のcallback用関数の設定
    collision_sphere_.radius = kRadius_; // 球の初期半径 = 規定値
    collision_sphere_.center = transform_.position + axes_.up * kDistance_to_root_; // 球の初期座標 = 根元の座標 + 上向きベクトル * 規定値
}

StickyBall::~StickyBall(void)
{
    colMPtr_->UnRegister(&collision_sphere_);
}

void StickyBall::Update(void)
{
    Object3D::Update();

    // 本来想定されている球の初期座標(根元の座標 + 上向きベクトル * 規定値)
    Vector3 default_stickyPos = transform_.position + axes_.up * kDistance_to_root_;

    // 球の座標が、1フレーム前の全体更新の終盤（衝突更新処理内）で補正されているので、実質1フレーム遅れ(?)
    // 想定される座標からどの程度ずれているか = 球の現在座標 - default_sticky_pos
    distance_to_defPos_ = (collision_sphere_.center - default_stickyPos).Length();
    // distance_to_defPos_が0.fではない場合球を本来想定される位置へ少しずつ補正する。 ※極小値として0.02fまでの誤差は無視される。
    float a = Math::Function::FixEpsilon<float>(distance_to_defPos_, 0.f, 0.02f);
    if (a)
    {
        // 補正速度
        const float correctSpd = 0.05f;

        // 球の現在座標から球の想定初期へのベクトル
        Vector3 vec_curPos2DefPos = (default_stickyPos - collision_sphere_.center).Normalize();

        // 球の現在座標を取得
        Vector3 spherePos = collision_sphere_.center;
        // 取得した変数に対して補正値を加算
        spherePos += vec_curPos2DefPos * correctSpd;
        // 補正済の座標を反映
        collision_sphere_.center = spherePos;
    }

    // 見た目弄る用
    GUI::Begin("sticky");
    GUI::Text("other");
    GUI::Text("def_sticPos: %f,%f,%f", default_stickyPos.x, default_stickyPos.y, default_stickyPos.z);
    GUI::Text("dist_2_defPos: %f", distance_to_defPos_);
    GUI::Text("a: %f", a);
    GUI::BlankLine();
    GUI::Text("info at root");
    GUI::Text("pos:%f,%f,%f", transform_.position.x, transform_.position.y, transform_.position.z);
    GUI::Text("info at sphere");
    GUI::Text("pos:%f,%f,%f", collision_sphere_.center.x, collision_sphere_.center.y, collision_sphere_.center.z);
    GUI::Text("sca:%f,%f,%f", transform_.scale.x, transform_.scale.y, transform_.scale.z);
    GUI::Text("rot:%f,%f,%f", transform_.rotation.x, transform_.rotation.y, transform_.rotation.z);
    GUI::BlankLine();
    GUI::Text("scale");
    ImGui::SliderFloat("x", &transform_.scale.x, 1.f, 10.f);
    ImGui::SliderFloat("y", &transform_.scale.y, 1.f, 10.f);
    ImGui::SliderFloat("z", &transform_.scale.z, 1.f, 10.f);
    GUI::End();

    matTrans_.mat_world = Math::Function::AffinTrans(collision_sphere_.center, transform_.scale, axes_);
}

void StickyBall::Draw(void)
{
    Object3D::Draw("Resources/jasperGreen1x1.png");
}

void StickyBall::OnCollision(void)
{
    if (collision_sphere_.GetOther()->GetID() == "player") // tag:"player"と触れている場合
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(collision_sphere_.GetOther());

        // シーンUpdate()後の衝突Update()で計算される
        // 球体を押し出す処理。※ただし、押し出し後の球体座標が、(根元座標 + 規定値) を超えている場合押し出しされない。

        // プレイヤーの座標と自身の球の座標から押し出される方向を算出する
        Vector3 p_to_this = (collision_sphere_.center - other->center).Normalize();

        // めり込み距離を出す (めり込んでいる想定 - 距離）※値はマイナスで返る
        float diff = Vector3(collision_sphere_.center - other->center).Length() - (other->radius + collision_sphere_.radius);

        // 現在座標
        Vector3 currentPos = collision_sphere_.center;

        // 正規化された球からプレイヤーまでのベクトル * めり込み距離
        currentPos += p_to_this * -diff; // ここをマイナス符号で値反転

        // 移動後の座標が、元の座標より規定値以上離れていないなら、座標を補正可能
        //if ((collision_sphere_.center - currentPos).Length() < kDistance_to_defPos_)
        //{
            // 座標を補正
        collision_sphere_.center = currentPos;

        // 補正された値で行列を生成
        matTrans_.mat_world = Math::Function::AffinTrans(collision_sphere_.center, transform_.scale, axes_);
        //}
    }
}

void StickyBall::SetRoot(const Vector3& pos)
{
    transform_.position = pos;
    collision_sphere_.center = pos + axes_.up * kDistance_to_root_;
}
