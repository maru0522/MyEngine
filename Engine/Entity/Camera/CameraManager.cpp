#include "CameraManager.h"
#include "Object3D.h"
#include "MathUtil.h"
#include "WndAPI.h"
#include "Sprite.h"
#include "Input.h"
#include <functional>
#include "SimplifyImGui.h"

CameraManager* CameraManager::GetInstance(void)
{
    static CameraManager sCameraM;
    return &sCameraM;
}

void CameraManager::Initialize(void)
{
    is_finalized_ = false;
}

void CameraManager::Update(void)
{
    // 終了処理後ならスキップ
    if (is_finalized_) { return; }

    DebugGui();


    // nullチェック
    if (!current_) // nullptrではない無効なアドレスかどうかを識別する方法を調べる
    {
        //// 破棄されたアドレスを参照していた場合に備え、nullptrを入れる。
        //current_ = nullptr;
        //// ベクタ配列を全検索する
        //for (auto it = vector_cameras_.begin(); it != vector_cameras_.end(); it++)
        //{
        //    // nullptrになっている要素がある場合、削除する
        //    if (*it == nullptr) { vector_cameras_.erase(it); }
        //}

        // 処理をスキップ
        return;
    }

    // 更新処理
    current_->Update();
}

void CameraManager::Finalize(void)
{
    is_finalized_ = true;

    // 現在のカメラを外す
    current_ = nullptr;

    // umap配列を初期化
    vector_cameras_.clear();
    vector_debugCameras_.clear();
}

void CameraManager::Register(ICamera* arg_cameraPtr)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }


    // 名前の末尾につける用の数字
    int32_t num{};
    // 名前に数字を追加。
    std::string id(arg_cameraPtr->GetId() + std::to_string(num)); // <== "id"0 に改名してから登録される
    // ベクタ配列に同名のカメラがあるか検索し、ある場合は末尾の番号を1つずらす。
    RecursiveSearchId_Camera(id, num); // "id"0 ~ ...


    // 登録されるカメラの名前を、末尾に番号がつけられたものへ変更する
    arg_cameraPtr->SetId(id);
    // 登録
    vector_cameras_.push_back(arg_cameraPtr);
}

void CameraManager::UnRegister(ICamera* arg_cameraPtr)
{
    UnRegister(arg_cameraPtr->GetId());
}

void CameraManager::UnRegister(const std::string& arg_id)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }

    // カメラの名前が、arg_id と一致しているか判別する bool型関数
    std::function<bool(ICamera*)> conditional = [&](ICamera* arg_icp) { return arg_icp->GetId() == arg_id; };
    // カメラの名前が、arg_id と一致している場合、ベクタ配列から抹消
    std::erase_if(vector_cameras_, conditional);
}

void CameraManager::CreateDebugCamera(ICamera* arg_cameraPtr)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }

    // 名前の末尾につける用の数字
    int32_t num{};
    // 名前に数字を追加。
    std::string id = arg_cameraPtr->GetId();
    // 接頭辞を除いた名前を抽出
    if (id.starts_with("ICamera_")) { id.erase(0, 8); }
    if (id.starts_with("SphericalCamera_")) { id.erase(0, 16); }
    // ベクタ配列に同名のカメラがあるか検索し、ある場合は末尾の番号を1つずらす。
    RecursiveSearchId_DebugCamera(id, num); // "id"0 ~ ...


    // 登録
    vector_debugCameras_.emplace_back(std::make_unique<DebugCamera>(id));

    const Vector3 pos = { arg_cameraPtr->GetTransformMatrix().mat_pos.m[3][0], arg_cameraPtr->GetTransformMatrix().mat_pos.m[3][1], arg_cameraPtr->GetTransformMatrix().mat_pos.m[3][2] };
    const Vector3 rot = { 0.f,0.f,0.f };
    const Vector3 sca = { 1.f,1.f,1.f };
    const Transform copy = { pos,rot,sca };

    // 座標系を、基になったカメラと揃える
    vector_debugCameras_.back()->SetTransform(copy);
    // 姿勢を、基になったカメラと揃える
    vector_debugCameras_.back()->SetAxis3(arg_cameraPtr->GetAxis3());
    // 投影始端を、基になったカメラと揃える
    vector_debugCameras_.back()->SetNearZ(arg_cameraPtr->GetNearZ());
    // 投影終端を、基になったカメラと揃える
    vector_debugCameras_.back()->SetFarZ(arg_cameraPtr->GetFarZ());
    // ワールド行列を、基になったカメラと揃える
    vector_debugCameras_.back()->SetTransformMatrix(arg_cameraPtr->GetTransformMatrix());
    // アフィン変換の計算方法を、基になったカメラと揃える
    vector_debugCameras_.back()->SetIsAffinUseAxes(arg_cameraPtr->GetIsAffinUseAxes());

    // デバッグカメラ生成時、使用するカメラも、そちらに切り替えるか
    if (is_switchCameraByCreateDebugCam_) { SetCurrentCamera(vector_debugCameras_.back()->GetId()); }
}

void CameraManager::DestroyDebugCamera(const std::string& arg_id)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }

    // デバッグカメラの名前が、arg_id と一致しているか判別する bool型関数
    std::function<bool(const std::unique_ptr<DebugCamera>&)> conditional = [&](const std::unique_ptr<DebugCamera>& arg_up_dc) { return arg_up_dc->GetId() == arg_id; };
    // デバッグカメラの名前が、arg_id と一致している場合、ベクタ配列から抹消
    std::erase_if(vector_debugCameras_, conditional);
}

void CameraManager::DebugGui(void)
{
    // ウィンドウ名
    GUI::Begin("CameraManager");



    // カメラ一覧用ベクタ配列
    std::vector<std::string> cameraList;
    // ベクタ配列を全検索してカメラの名前をコピー（一般カメラ配列）
    for (auto& camera : vector_cameras_)
    {
        cameraList.emplace_back(camera->GetId());
    }
    // ベクタ配列を全検索してカメラの名前をコピー（デバッグカメラ配列）
    for (auto& debugcamera : vector_debugCameras_)
    {
        cameraList.emplace_back(debugcamera->GetId());
    }

    // 現在選択しているカメラの名前
    static std::string currentItem;
    currentItem = current_->GetId();

    GUI::Text("CurrentCamera : %s", currentItem.c_str());
    ImGui::Separator();
    GUI::BlankLine(); // 1行あける
    GUI::BlankLine(); // 1行あける


    GUI::CheckBox("is_switch", &is_switchCameraByCreateDebugCam_);
    ImGui::SameLine();
    GUI::HelpMaker("Automatically switch the viewpoint to the new debug camera when a debug camera is generated");

    ImGui::SameLine();
    GUI::CheckBox("is_popupFront", &is_popupWindowMoveFront_);
    ImGui::SameLine();
    GUI::HelpMaker("When a pop-up window is generated, it comes to the forefront of the screen.");
    GUI::BlankLine(); // 1行あける
    GUI::BlankLine(); // 1行あける
    GUI::BlankLine(); // 1行あける


    // デバッグカメラ生成用のボタン
    if (GUI::ButtonTrg("Create", { 100,20 }))
    {
        CreateDebugCamera(current_);
    }
    ImGui::SameLine();
    GUI::HelpMaker("Use part of CurrentCamera's id when Create new DebugCamera");


    // カメラリストの項目を生成
    if (ImGui::TreeNode("Camera List"))
    {
        // 任意のカメラを検索で絞り込む用のフィルター
        static ImGuiTextFilter cameraFilter;
        // フィルター（検索欄）を設置
        cameraFilter.Draw();
        ImGui::SameLine();
        // ヘルパーの設置（フィルター/検索欄）
        GUI::HelpMaker("The search field allows you to narrow down the cameras to be displayed.");
        GUI::BlankLine();

        // ヘルパーの設置（カメラリスト）
        GUI::HelpMaker("Double Left click for more information");
        int32_t node_clicked = -1;
        // カメラリスト用に、全てのカメラを検索
        for (int32_t i = 0; i < cameraList.size(); i++)
        {
            // 表示するカメラ名を、フィルター（検索欄）を考慮して絞り込みする。
            if (cameraFilter.PassFilter(cameraList[i].c_str())) // 検索欄に何も入力していない場合関係ない
            {
                ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
                bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, cameraList[i].c_str());
                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) { node_clicked = i; }

                if (node_open)
                {
                    if (GUI::ButtonTrg("Set"))
                    {
                        if (cameraList[i] != currentItem)
                        {
                            SetCurrentCamera(cameraList[i]);
                        }
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Information"))
                    {
                        ImGui::OpenPopup("another popup");
                    }

                    if (cameraList[i].starts_with("DebugCamera_"))
                    {
                        ImGui::SameLine();
                        if (GUI::ButtonTrg("Delete"))
                        {
                            DestroyDebugCamera(cameraList[i]);
                        }
                    }

                    bool is_beginPopup{};
                    is_popupWindowMoveFront_ ?
                        is_beginPopup = ImGui::BeginPopupModal("another popup", NULL, ImGuiWindowFlags_AlwaysAutoResize) :
                        is_beginPopup = ImGui::BeginPopup("another popup");

                    if (is_beginPopup)
                    {
                        ICamera* camPtr = GetCamera(cameraList[i]);
                        GUI::Text("Axes");
                        GUI::Text("forward :  %f,%f,%f", camPtr->axes_.forward.x, camPtr->axes_.forward.y, camPtr->axes_.forward.z);
                        GUI::Text("right :    %f,%f,%f", camPtr->axes_.right.x, camPtr->axes_.right.y, camPtr->axes_.right.z);
                        GUI::Text("up :       %f,%f,%f", camPtr->axes_.up.x, camPtr->axes_.up.y, camPtr->axes_.up.z);
                        GUI::BlankLine();
                        GUI::BlankLine();
                        GUI::BlankLine();

                        GUI::Text("Transform");
                        GUI::Text("position : %f,%f,%f", camPtr->transform_.position.x, camPtr->transform_.position.y, camPtr->transform_.position.z);
                        GUI::Text("rotation : %f,%f,%f", camPtr->transform_.rotation.x, camPtr->transform_.rotation.y, camPtr->transform_.rotation.z);
                        GUI::Text("scale :    %f,%f,%f", camPtr->transform_.scale.x, camPtr->transform_.scale.y, camPtr->transform_.scale.z);
                        GUI::BlankLine();
                        GUI::BlankLine();
                        GUI::BlankLine();

                        GUI::Text("TransformMatrix - mat_world");
                        const Matrix4& M = camPtr->transformMatrix_.mat_world;
                        // 可変長引数の関係でラップ関数を呼べない。 // TODO: いつか直す
                        ImGui::Text("%f,%f,%f,%f", M.m[0][0], M.m[0][1], M.m[0][2], M.m[0][3]);
                        ImGui::Text("%f,%f,%f,%f", M.m[1][0], M.m[1][1], M.m[1][2], M.m[1][3]);
                        ImGui::Text("%f,%f,%f,%f", M.m[2][0], M.m[2][1], M.m[2][2], M.m[2][3]);
                        ImGui::Text("%f,%f,%f,%f", M.m[3][0], M.m[3][1], M.m[3][2], M.m[3][3]);
                        GUI::BlankLine();
                        GUI::BlankLine();
                        GUI::BlankLine();

                        static Transform transform;
                        transform = camPtr->GetTransform();
                        GUI::SliderFloat3("rotation", transform.rotation, 0.f, 6.28319f);
                        camPtr->SetTransform(transform);

                        ImGui::Separator();
                        if (ImGui::Button("Close"))
                        {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::TreePop();
                }
            }
        }
        ImGui::TreePop();
    }

    GUI::End();
}

bool CameraManager::IsContainSameId_Camera(const std::string& arg_id)
{
    // 登録されたカメラ群を、全検索
    for (auto& camera : vector_cameras_)
    {
        // 既に登録されたカメラ群に同名のカメラがあれば true
        if (camera->GetId() == arg_id) { return true; }
    }

    // 既に登録されたカメラ群に同名のカメラがなければ false
    return false;
}

bool CameraManager::IsContainSameId_DebugCamera(const std::string& arg_id)
{
    // 登録されたデバッグカメラ群を、全検索
    for (auto& camera : vector_debugCameras_)
    {
        // 既に登録されたデバッグカメラ群に、同名のデバッグカメラ true
        if (camera->GetId() == arg_id) { return true; }
    }

    // 既に登録されたデバッグカメラ群に、同名のデバッグカメラがなければ false
    return false;
}

void CameraManager::RecursiveSearchId_Camera(std::string& arg_id, int32_t& arg_duplicateNum)
{
    // 登録されたベクタ配列に同名のカメラがあるか検索
    bool isContain = IsContainSameId_Camera(arg_id);
    if (isContain)
    {
        // 文字列の最後の数字を取り消す。
        arg_id.pop_back();

        // 文字列の最後に追加する数字を加算
        arg_duplicateNum++;
        // 文字列の最後に数字を追加
        arg_id += std::to_string(arg_duplicateNum);

        // 再帰的呼び出し
        RecursiveSearchId_Camera(arg_id, arg_duplicateNum);
    }

    return;
}

void CameraManager::RecursiveSearchId_DebugCamera(std::string& arg_id, int32_t& arg_duplicateNum)
{
    // カメラの名前が "DebugCamera_" から始まっているか検索
    const bool is_startWith = arg_id.starts_with("DebugCamera_");
    if (is_startWith)
    {
        arg_id.erase(0, 12);
        // 文字列の最後に数字を追加
        arg_id = arg_id + "-" + std::to_string(arg_duplicateNum);
    }

    // 登録されたベクタ配列に同名のカメラがあるかどうか
    const bool is_contain = IsContainSameId_DebugCamera("DebugCamera_" + arg_id);

    // 登録されたベクタ配列に同名のカメラがある場合
    if (is_contain)
    {
        // 文字列の最後の数字を取り消す。
        arg_id.pop_back();

        // 文字列の最後に追加する数字を加算
        arg_duplicateNum++;
        // 文字列の最後に数字を追加
        arg_id += std::to_string(arg_duplicateNum);

        // 再帰的呼び出し
        RecursiveSearchId_DebugCamera(arg_id, arg_duplicateNum);
    }

    return;
}

void CameraManager::SetCurrentCamera(ICamera* arg_cameraPtr)
{
    SetCurrentCamera(arg_cameraPtr->GetId());
}

void CameraManager::SetCurrentCamera(const std::string& arg_id)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }


    // ベクタ配列に存在するか（一般カメラ）
    bool isContain_camera = IsContainSameId_Camera(arg_id);
    // ベクタ配列に存在するか（デバッグカメラ）
    bool isContain_debugCamera = IsContainSameId_DebugCamera(arg_id);
    // カメラクラスのポインタ
    ICamera* cameraPtr = nullptr;


    // どちらにも存在しない場合、スキップ
    if (isContain_camera == false && isContain_debugCamera == false) { return; }
    // どちらか1つのみ、trueになる。（2つのベクタ配列で id が被ることは無い）
    if (isContain_camera)
    {
        for (auto& camera : vector_cameras_)
        {
            // 名前が一致したカメラのptrを受け取る
            if (camera->GetId() == arg_id) { cameraPtr = camera; }
        }
    }
    if (isContain_debugCamera)
    {
        for (auto& debugCamera : vector_debugCameras_)
        {
            // 名前が一致したカメラのptrを受け取る
            if (debugCamera->GetId() == arg_id) { cameraPtr = debugCamera.get(); }
        }
    }


    // 新規カメラをセット
    current_ = cameraPtr;
    current_->Update();
    current_->UpdateOrthoGraphic();
    Sprite::UpdateCBMatOrthoGraphic(this);
    Object3D::UpdateCBMatViewPerse(this);
}

ICamera* CameraManager::GetCamera(const std::string& arg_cameraId)
{
    // ベクタ配列に存在するか（一般カメラ）
    bool isContain_camera = IsContainSameId_Camera(arg_cameraId);
    // ベクタ配列に存在するか（デバッグカメラ）
    bool isContain_debugCamera = IsContainSameId_DebugCamera(arg_cameraId);


    // どちらにも存在しない場合、スキップ
    if (isContain_camera == false && isContain_debugCamera == false) { return nullptr; }
    // どちらか1つのみ、trueになる。（2つのベクタ配列で id が被ることは無い）
    if (isContain_camera)
    {
        for (auto& camera : vector_cameras_)
        {
            // 名前が一致したカメラのptrを受け取る
            if (camera->GetId() == arg_cameraId) { return camera; }
        }
    }
    if (isContain_debugCamera)
    {
        for (auto& debugCamera : vector_debugCameras_)
        {
            // 名前が一致したカメラのptrを受け取る
            if (debugCamera->GetId() == arg_cameraId) { return debugCamera.get(); }
        }
    }

    return nullptr;
}
