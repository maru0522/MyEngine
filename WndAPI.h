#pragma once
#include <Windows.h>
#include <cstdint>

class WndAPI
{
public:
    // 静的関数
    static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    // 静的変数
    // ウィンドウサイズ
    static constexpr size_t sWidth_{ 1280 };
    static constexpr size_t sHeight_{ 720 };

private:
    // ウィンドウクラス名
    static const wchar_t sWindowClassName_[];

public:
    // 関数
    WndAPI(void);
    WndAPI(const char* title);
    void Delete(void);

    // ウィンドウの表示状態を変更する関数
    // デフォルト:nCmdShow = SW_SHOW
    // https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-showwindow
    void SetDisplay(int32_t nCmdShow);

    // ウィンドウ固定用のメッセージループ処理
    // WM_QUITならFALSEを返す
    bool GetKeepWindow();

    // ウィンドウハンドルの取得
    inline HWND GetHwnd() const { return hwnd_; }

    inline HINSTANCE GetHInstance() const { return w_.hInstance; }

private:
    // 変数
    WNDCLASSEX w_{};

    HWND hwnd_{};
};

//void SetWindowSize(int width, int height)
//{
//    // ウィンドウサイズへの代入処理
//
//    // CameraManager* -> current_ -> UpdateOrthoGraphic();
//    // Sprite::UpdateCBMatOrthoGraphic();
//}

