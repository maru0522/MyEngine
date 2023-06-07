#pragma once
#include <Windows.h>

class WndAPI
{
public:
    // �ÓI�֐�
    static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    // �ÓI�ϐ�
    // �E�B���h�E�T�C�Y
    static constexpr size_t width_{ 1280 };
    static constexpr size_t height_{ 720 };

private:
    // �E�B���h�E�N���X��
    static const wchar_t windowClassName_[];

public:
    // �֐�
    WndAPI(void);
    WndAPI(const char* title);
    void Delete(void);

    // �E�B���h�E�̕\����Ԃ�ύX����֐�
    // �f�t�H���g:nCmdShow = SW_SHOW
    void SetDisplay(int32_t nCmdShow);

    // �E�B���h�E�Œ�p�̃��b�Z�[�W���[�v����
    // WM_QUIT�Ȃ�FALSE��Ԃ�
    bool GetKeepWindow();

    // �E�B���h�E�n���h���̎擾
    inline HWND GetHwnd() const { return hwnd_; }

    inline HINSTANCE GetHInstance() const { return w_.hInstance; }

private:
    // �ϐ�
    WNDCLASSEX w_{};

    HWND hwnd_{};
};

//void SetWindowSize(int width, int height)
//{
//    // �E�B���h�E�T�C�Y�ւ̑������
//
//    // CameraManager* -> current_ -> UpdateOrthoGraphic();
//    // Sprite::UpdateCBMatOrthoGraphic();
//}

