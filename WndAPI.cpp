#include "WndAPI.h"
#include <string>
#include "Util.h"
#include <timeapi.h>
#pragma comment(lib,"winmm.lib")

const wchar_t WndAPI::windowClassName_[] = L"maruyamaEngine";

LRESULT WndAPI::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    // ���b�Z�[�W�ɉ����ăQ�[���ŗL�̏������s��
    switch (msg) {
        // �E�B���h�E���j�����ꂽ
    case WM_DESTROY:
        // OS�ɑ΂��āA�A�v���̏I����`����
        PostQuitMessage(0);
        return 0;
    }

    // �W���̃��b�Z�[�W�������s��
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

WndAPI::WndAPI(void)
{
    // �V�X�e���^�C�}�[�̕���\���グ��	
    timeBeginPeriod(1);

    // �E�B���h�E�N���X�̐ݒ�
    w_.cbSize = sizeof(WNDCLASSEX);
    w_.lpfnWndProc = static_cast<WNDPROC>(WindowProc);   // �E�B���h�E�v���V�[�W����ݒ�
    w_.lpszClassName = windowClassName_;                  // �E�B���h�E�N���X��
    w_.hInstance = GetModuleHandle(nullptr);             // �E�B���h�E�n���h��
    w_.hCursor = LoadCursor(NULL, IDC_ARROW);            // �J�[�\���w��

    // �E�B���h�E�N���X��OS�ɓo�^����
    RegisterClassEx(&w_);

    // �E�B���h�E�T�C�Y{ X���W Y���W ���� �c�� }
    RECT wrc = { 0, 0, width_, height_ };
    // �����ŃT�C�Y��␳����
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // �E�B���h�E�I�u�W�F�N�g�̐���
    hwnd_ = CreateWindow(
        w_.lpszClassName,     // �N���X��
        windowClassName_,     // �^�C�g���o�[�̕���
        WS_OVERLAPPEDWINDOW,  // �W���I�ȃE�B���h�E�X�^�C��
        CW_USEDEFAULT,        // �\��X���W�iOS�ɔC����j
        CW_USEDEFAULT,        // �\��Y���W�iOS�ɔC����j
        wrc.right - wrc.left, // �E�B���h�E����
        wrc.bottom - wrc.top, // �E�B���h�E�c��
        nullptr,              // �e�E�B���h�E�n���h��
        nullptr,              // ���j���[�n���h��
        w_.hInstance,         // �Ăяo���A�v���P�[�V�����n���h��
        nullptr               // �I�v�V����
    );

    // �E�B���h�E��\����Ԃɂ���
    ShowWindow(hwnd_, SW_SHOW);
}

WndAPI::WndAPI(const char* title)
{
    // �V�X�e���^�C�}�[�̕���\���グ��	
    timeBeginPeriod(1);

    // �E�B���h�E�N���X�̐ݒ�
    w_.cbSize = sizeof(WNDCLASSEX);
    w_.lpfnWndProc = static_cast<WNDPROC>(WindowProc);   // �E�B���h�E�v���V�[�W����ݒ�
    w_.lpszClassName = windowClassName_;                  // �E�B���h�E�N���X��
    w_.hInstance = GetModuleHandle(nullptr);             // �E�B���h�E�n���h��
    w_.hCursor = LoadCursor(NULL, IDC_ARROW);            // �J�[�\���w��

    // �E�B���h�E�N���X��OS�ɓo�^����
    RegisterClassEx(&w_);

    // �E�B���h�E�T�C�Y{ X���W Y���W ���� �c�� }
    RECT wrc = { 0, 0, width_, height_ };
    // �����ŃT�C�Y��␳����
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // �E�B���h�E�^�C�g����wstring(wchar_t)�ɕϊ�
    std::wstring wStrTitle{ Util::Convert::ToWString(title) };

    // �E�B���h�E�I�u�W�F�N�g�̐���
    hwnd_ = CreateWindow(
        w_.lpszClassName,     // �N���X��
        wStrTitle.c_str(),    // �^�C�g���o�[�̕���
        WS_OVERLAPPEDWINDOW,  // �W���I�ȃE�B���h�E�X�^�C��
        CW_USEDEFAULT,        // �\��X���W (OS�ɔC����)
        CW_USEDEFAULT,        // �\��Y���W (OS�ɔC����)
        wrc.right - wrc.left, // �E�B���h�E����
        wrc.bottom - wrc.top, // �E�B���h�E�c��
        nullptr,              // �e�E�B���h�E�n���h��
        nullptr,              // ���j���[�n���h��
        w_.hInstance,          // �Ăяo���A�v���P�[�V�����n���h��
        nullptr               // �I�v�V����
    );

    // �E�B���h�E��\����Ԃɂ���
    ShowWindow(hwnd_, SW_SHOW);
}

void WndAPI::Delete(void)
{
    UnregisterClass(w_.lpszClassName, w_.hInstance);
}

void WndAPI::SetDisplay(int nCmdShow)
{
    ShowWindow(hwnd_, nCmdShow);
}

bool WndAPI::GetKeepWindow()
{
    // ���b�Z�[�W
    MSG msg{};

    // ���b�Z�[�W������H
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);     // �L�[���̓��b�Z�[�W�̏���
        DispatchMessage(&msg);      // �v���V�[�W���Ƀ��b�Z�[�W�𑗂�
    }

    // X�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
    return msg.message != WM_QUIT;
}
