#include "Util.h"
#include <string>
#include "WndAPI.h"
#include <timeapi.h>
#include <imgui_impl_win32.h>
#pragma comment(lib,"winmm.lib")

const std::wstring WndAPI::kWindowClassName_ = L"maruyamaEngine";

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WndAPI::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    // ImGUi用ウィンドウプロシージャ呼び出し
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return true;

    // メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
        // ウィンドウが破棄された
    case WM_DESTROY:
        // OSに対して、アプリの終了を伝える
        PostQuitMessage(0);
        return 0;
    }

    // 標準のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

WndAPI::WndAPI(void)
{
    // システムタイマーの分解能を上げる	
    timeBeginPeriod(1);

    // ウィンドウクラスの設定
    w_.cbSize = sizeof(WNDCLASSEX);
    w_.lpfnWndProc = static_cast<WNDPROC>(WindowProc);   // ウィンドウプロシージャを設定
    w_.lpszClassName = kWindowClassName_.c_str();        // ウィンドウクラス名
    w_.hInstance = GetModuleHandle(nullptr);             // ウィンドウハンドル
    w_.hCursor = LoadCursor(NULL, IDC_ARROW);            // カーソル指定

    // ウィンドウクラスをOSに登録する
    RegisterClassEx(&w_);

    // ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
    RECT wrc = { 0, 0, kWidth_, kHeight_ };
    // 自動でサイズを補正する
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウオブジェクトの生成
    hwnd_ = CreateWindow(
        w_.lpszClassName,     // クラス名
        kWindowClassName_.c_str(),     // タイトルバーの文字
        WS_OVERLAPPEDWINDOW,  // 標準的なウィンドウスタイル
        CW_USEDEFAULT,        // 表示X座標（OSに任せる）
        CW_USEDEFAULT,        // 表示Y座標（OSに任せる）
        wrc.right - wrc.left, // ウィンドウ横幅
        wrc.bottom - wrc.top, // ウィンドウ縦幅
        nullptr,              // 親ウィンドウハンドル
        nullptr,              // メニューハンドル
        w_.hInstance,         // 呼び出しアプリケーションハンドル
        nullptr               // オプション
    );

    // ウィンドウを表示状態にする
    ShowWindow(hwnd_, SW_SHOW);
}

WndAPI::WndAPI(const char* title)
{
    // システムタイマーの分解能を上げる	
    timeBeginPeriod(1);

    // ウィンドウクラスの設定
    w_.cbSize = sizeof(WNDCLASSEX);
    w_.lpfnWndProc = static_cast<WNDPROC>(WindowProc);   // ウィンドウプロシージャを設定
    w_.lpszClassName = kWindowClassName_.c_str();        // ウィンドウクラス名
    w_.hInstance = GetModuleHandle(nullptr);             // ウィンドウハンドル
    w_.hCursor = LoadCursor(NULL, IDC_ARROW);            // カーソル指定

    // ウィンドウクラスをOSに登録する
    RegisterClassEx(&w_);

    // ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
    RECT wrc = { 0, 0, kWidth_, kHeight_ };
    // 自動でサイズを補正する
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウタイトルをwstring(wchar_t)に変換
    std::wstring wStrTitle{ Util::Convert::ToWString(title) };

    // ウィンドウオブジェクトの生成
    hwnd_ = CreateWindow(
        w_.lpszClassName,     // クラス名
        wStrTitle.c_str(),    // タイトルバーの文字
        WS_OVERLAPPEDWINDOW,  // 標準的なウィンドウスタイル
        CW_USEDEFAULT,        // 表示X座標 (OSに任せる)
        CW_USEDEFAULT,        // 表示Y座標 (OSに任せる)
        wrc.right - wrc.left, // ウィンドウ横幅
        wrc.bottom - wrc.top, // ウィンドウ縦幅
        nullptr,              // 親ウィンドウハンドル
        nullptr,              // メニューハンドル
        w_.hInstance,          // 呼び出しアプリケーションハンドル
        nullptr               // オプション
    );

    // ウィンドウを表示状態にする
    ShowWindow(hwnd_, SW_SHOW);
}

void WndAPI::Delete(void)
{
    UnregisterClass(w_.lpszClassName, w_.hInstance);
}

void WndAPI::SetDisplay(int32_t nCmdShow)
{
    ShowWindow(hwnd_, static_cast<int>(nCmdShow));
}

bool WndAPI::GetKeepWindow()
{
    // メッセージ
    MSG msg{};

    // メッセージがある？
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);     // キー入力メッセージの処理
        DispatchMessage(&msg);      // プロシージャにメッセージを送る
    }

    // Xボタンで終了メッセージが来たらゲームループを抜ける
    return msg.message != WM_QUIT;
}
