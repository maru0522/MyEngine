#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "xinput.lib")

// ���̂̐���
static Microsoft::WRL::ComPtr<IDirectInput8> directInput_{ nullptr }; // DirectInput����

#pragma region keyboard
Microsoft::WRL::ComPtr<IDirectInputDevice8> Input::Keyboard::sKeyboard_{ nullptr };
std::array<BYTE, 256> Input::Keyboard::sKeysPre_{ 0 };
std::array<BYTE, 256> Input::Keyboard::sKeys_{ 0 };
#pragma endregion

#pragma region diPad
Microsoft::WRL::ComPtr<IDirectInputDevice8> Input::DIPad::sDiPad_{ nullptr };
DIJOYSTATE Input::DIPad::sDiStatePre_{ 0 };
DIJOYSTATE Input::DIPad::sDiState_{ 0 };
bool Input::DIPad::sIsConnect_{ false };
#pragma endregion

#pragma region xPad
Input::XPad::CustomDeadZone Input::XPad::sDeadZone_{ 0 };
XINPUT_STATE Input::XPad::sXStatePre_{ 0 };
XINPUT_STATE Input::XPad::sXState_{ 0 };
bool Input::XPad::sIsConnect_{ false };
#pragma endregion

#pragma region mouse
Microsoft::WRL::ComPtr<IDirectInputDevice8> Input::Mouse::sMouse_{nullptr};
Input::Mouse::MouseState Input::Mouse::sMouseStatePre_{};
Input::Mouse::MouseState Input::Mouse::sMouseState_{};
WndAPI* Input::Mouse::sWndApiPtr_;
#pragma endregion

void Input::Keyboard::Initialize(WndAPI* p_wndapi)
{
    HRESULT hr = S_FALSE;

    if (directInput_ == nullptr) {
        // DirectInput�̏�����
        hr = DirectInput8Create(p_wndapi->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
        assert(SUCCEEDED(hr));
    }

    // �L�[�{�[�h�f�o�C�X�̐���
    hr = directInput_->CreateDevice(GUID_SysKeyboard, &sKeyboard_, NULL);
    assert(SUCCEEDED(hr));

    // ���̓f�[�^�`���̃Z�b�g
    hr = sKeyboard_->SetDataFormat(&c_dfDIKeyboard); // �W���`��
    assert(SUCCEEDED(hr));

    // �r�����䃌�x���̃Z�b�g
    hr = sKeyboard_->SetCooperativeLevel(p_wndapi->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
    assert(SUCCEEDED(hr));
}

void Input::Keyboard::Update(void)
{
    // preKeys�֏��ۑ�
    memcpy(sKeysPre_.data(), sKeys_.data(), sizeof(sKeys_));

    // �L�[�{�[�h���̎擾
    sKeyboard_->Acquire();

    // �L�[�̓��͏�Ԃ̏�����
    sKeys_.fill(0);

    // �L�[�S�Ă̓��͏�Ԃ̎擾
    sKeyboard_->GetDeviceState((DWORD)size(sKeys_), sKeys_.data());
}

//void Input::DIPad::Initialize(WndAPI* p_wndapi)
//{
//    HRESULT hr = S_FALSE;
//
//    if (directInput_ == nullptr) {
//        // DirectInput�̏�����
//        hr = DirectInput8Create(p_wndapi->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
//        assert(SUCCEEDED(hr));
//    }
//
//    // �f�o�C�X�̗�
//    hr = directInput_->EnumDevices(DI8DEVTYPE_GAMEPAD, DeviceFindCallBack, nullptr, DIEDFL_ATTACHEDONLY);
//    assert(SUCCEEDED(hr));
//}

//void Input::DIPad::Update(void)
//{
//    if (!isConnect_) {
//        return;
//    }
//
//    // preKeys�֏��ۑ�
//    memcpy(&diStatePre_, &diState_, sizeof(diState_));
//
//    diPad_->Acquire();
//
//    std::fill(std::begin(diState_.rgbButtons), std::end(diState_.rgbButtons), 0);
//
//    //DIJOYSTATE pad;
//
//    //joyPad_->GetDeviceState(sizeof(pad), &pad);
//    diPad_->GetDeviceState(sizeof(diState_), &diState_);
//
//    //std::copy(std::begin(pad.rgbButtons), std::end(pad.rgbButtons), pad_.rgbButtons);
//}

//BOOL __stdcall Input::DIPad::DeviceFindCallBack(const DIDEVICEINSTANCE* pdidInstance, void* pContext)
//{
//    directInput_->CreateDevice(pdidInstance->guidInstance, &diPad_, NULL);
//
//    diPad_->SetDataFormat(&c_dfDIJoystick);
//
//    return DIENUM_STOP;
//}

void Input::XPad::Initialize(void)
{
    XInputGetState(0, &sXState_);
}

void Input::XPad::Update(void)
{
    sXStatePre_ = sXState_;

    ZeroMemory(&sXState_, sizeof(XINPUT_STATE));

    if (XInputGetState(0, &sXState_) == ERROR_SUCCESS) {
        sIsConnect_ = true;
    }
    else {
        sIsConnect_ = false;
    }
}

const Vector2 Input::XPad::GetLStick(void)
{
    Vector2 tmp;

    tmp.x = sXState_.Gamepad.sThumbLX;
    tmp.y = sXState_.Gamepad.sThumbLY;

    if ((sXState_.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && sXState_.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
        (sXState_.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && sXState_.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {
        tmp.x = 0;
        tmp.y = 0;
    }

    return { (float)tmp.x, (float)tmp.y };
}

const Vector2 Input::XPad::GetLStickRaw(void)
{
    return { (float)sXState_.Gamepad.sThumbLX, (float)sXState_.Gamepad.sThumbLY };
}

const Vector2 Input::XPad::GetLStickCustom(void)
{
    Vector2 tmp;

    tmp.x = sXState_.Gamepad.sThumbLX;
    tmp.y = sXState_.Gamepad.sThumbLY;

    if ((sXState_.Gamepad.sThumbLX < sDeadZone_.xLeftValueX && sXState_.Gamepad.sThumbLX > -sDeadZone_.xLeftValueX) &&
        (sXState_.Gamepad.sThumbLY < sDeadZone_.xLeftValueY && sXState_.Gamepad.sThumbLY > -sDeadZone_.xLeftValueY)) {
        tmp.x = 0;
        tmp.y = 0;
    }

    return { (float)tmp.x, (float)tmp.y };
}

const Vector2 Input::XPad::GetRStick(void)
{
    Vector2 tmp;

    tmp.x = sXState_.Gamepad.sThumbRX;
    tmp.y = sXState_.Gamepad.sThumbRY;

    if ((sXState_.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && sXState_.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
        (sXState_.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && sXState_.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)) {
        tmp.x = 0;
        tmp.y = 0;
    }

    return { (float)tmp.x, (float)tmp.y };
}

const Vector2 Input::XPad::GetRStickRaw(void)
{
    return { (float)sXState_.Gamepad.sThumbRX, (float)sXState_.Gamepad.sThumbRY };
}

const Vector2 Input::XPad::GetRStickCustom(void)
{
    Vector2 tmp;

    tmp.x = sXState_.Gamepad.sThumbRX;
    tmp.y = sXState_.Gamepad.sThumbRY;

    if ((sXState_.Gamepad.sThumbRX < sDeadZone_.xRightValueX && sXState_.Gamepad.sThumbRX > -sDeadZone_.xRightValueX) &&
        (sXState_.Gamepad.sThumbRY < sDeadZone_.xRightValueY && sXState_.Gamepad.sThumbRY > -sDeadZone_.xRightValueY)) {
        tmp.x = 0;
        tmp.y = 0;
    }

    return { (float)tmp.x, (float)tmp.y };
}

void Input::XPad::Vibrate(int32_t lPower, int32_t rPower)
{
    XINPUT_VIBRATION v;

    ZeroMemory(&v, sizeof(XINPUT_VIBRATION));

    v.wLeftMotorSpeed = (unsigned short)lPower;
    v.wRightMotorSpeed = (unsigned short)rPower;

    XInputSetState(0, &v);
}

void Input::XPad::SetDeadZone(const Vector2& leftValueXY, const Vector2& rightValueXY)
{
    sDeadZone_.xLeftValueX = (uint16_t)leftValueXY.x;
    sDeadZone_.xLeftValueY = (uint16_t)leftValueXY.y;
    sDeadZone_.xRightValueX = (uint16_t)rightValueXY.x;
    sDeadZone_.xRightValueY = (uint16_t)rightValueXY.y;
}

void Input::InitializeAll(WndAPI* p_wndapi)
{
    KEYS::Initialize(p_wndapi);
    //DPAD::Initialize(p_wndapi);
    XPAD::Initialize(); // dinput.h�Ƃ͊֌W�Ȃ����ߕs�v
    Mouse::Initialize(p_wndapi);
}

void Input::UpdateAll(void)
{
    KEYS::Update();
    //DPAD::Update();
    XPAD::Update();
    Mouse::Update();
}

void Input::Mouse::Initialize(WndAPI* p_wndapi)
{
    HRESULT hr = S_FALSE;

    if (directInput_ == nullptr) {
        // DirectInput�̏�����
        hr = DirectInput8Create(p_wndapi->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
        assert(SUCCEEDED(hr));
    }

    // �}�E�X�f�o�C�X�̐���
    hr = directInput_->CreateDevice(GUID_SysMouse, &sMouse_, NULL);
    assert(SUCCEEDED(hr));

    // ���̓f�[�^�`���̃Z�b�g
    hr = sMouse_->SetDataFormat(&c_dfDIMouse);
    assert(SUCCEEDED(hr));

    // �r�����䃌�x���̃Z�b�g
    hr = sMouse_->SetCooperativeLevel(p_wndapi->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
    assert(SUCCEEDED(hr));

    // �|�C���^�Ⴄ
    sWndApiPtr_ = p_wndapi;
}

void Input::Mouse::Update(void)
{
    // �ŐV�̏����擾
    sMouse_->Acquire();
    sMouse_->Poll();

    // 1F�O�̏��
    sMouseStatePre_ = sMouseState_;

    // ���͏�Ԃ��擾
    sMouse_->GetDeviceState(sizeof(sMouseState_.mState), &sMouseState_.mState);

    // �ʒu���擾
    POINT pos;
    GetCursorPos(&pos);
    ScreenToClient(sWndApiPtr_->GetHwnd(), &pos);

    sMouseState_.cursorPos2d = { (float)pos.x,(float)pos.y };
    sMouseState_.scroll = (float)sMouseState_.mState.lZ;
}
