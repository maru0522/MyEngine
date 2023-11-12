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

    ImGui::ShowDemoWindow();


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
    const Vector3 sca= { 1.f,1.f,1.f };
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

    // ドロップダウンの管理と選択中のカメラ名の表示
    if (GUI::DropDownTrg("currentCamera", currentItem, cameraList))
    {
        // ドロップダウン内で別カメラが選択されたとき、使用しているカメラを変更する。
        SetCurrentCamera(currentItem);
    }
    GUI::BlankLine(); // 1行あける



    GUI::Text("Change CunnrentCamera at new DebugCamera");
    GUI::CheckBox("is_switch", &is_switchCameraByCreateDebugCam_);
    GUI::BlankLine(); // 1行あける

    GUI::Text("Use part of CurrentCamera's id when Create new DebugCamera");
    // デバッグカメラ生成用のボタン
    if (GUI::ButtonTrg("Create", { 100,20 }))
    {
        CreateDebugCamera(current_);
    }

    GUI::SearchList("CameraList", cameraList);

    if (ImGui::TreeNode("Context menus"))
    {
        GUI::HelpMaker("\"Context\" functions are simple helpers to associate a Popup to a given Item or Window identifier.");

        // BeginPopupContextItem() is a helper to provide common/simple popup behavior of essentially doing:
        //     if (id == 0)
        //         id = GetItemID(); // Use last item id
        //     if (IsItemHovered() && IsMouseReleased(ImGuiMouseButton_Right))
        //         OpenPopup(id);
        //     return BeginPopup(id);
        // For advanced advanced uses you may want to replicate and customize this code.
        // See more details in BeginPopupContextItem().

        // Example 1
        // When used after an item that has an ID (e.g. Button), we can skip providing an ID to BeginPopupContextItem(),
        // and BeginPopupContextItem() will use the last item ID as the popup ID.
        {
            //const char* names[5] = { "Label1", "Label2", "Label3", "Label4", "Label5" };
            //for (int n = 0; n < 5; n++)
            //{
            //    ImGui::Selectable(names[n]);
                if (ImGui::BeginMenu("Menu inside a regular window"))
                {
                    ImGui::MenuItem("(demo menu)", NULL, false, false);
                    if (ImGui::MenuItem("New")) {}
                    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
                    if (ImGui::BeginMenu("Open Recent"))
                    {
                        ImGui::MenuItem("fish_hat.c");
                        ImGui::MenuItem("fish_hat.inl");
                        ImGui::MenuItem("fish_hat.h");
                        if (ImGui::BeginMenu("More.."))
                        {
                            ImGui::MenuItem("Hello");
                            ImGui::MenuItem("Sailor");
                            ImGui::EndMenu();
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }

                //if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
                //{
                //    ImGui::Text("This a popup for \"%s\"!", names[n]);
                //    if (ImGui::Button("Close"))
                //        ImGui::CloseCurrentPopup();
                //    ImGui::EndPopup();
                //}
                //if (ImGui::IsItemHovered())
                //    ImGui::SetTooltip("Right-click to open popup");
            //}
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