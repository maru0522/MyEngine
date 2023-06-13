#include "Light.h"
#include "InitDirectX.h"

Light::Light(void)
{
    cbLight_.Create();
    TransferCB();
}

void Light::Update(void)
{
    if (isDirty_) {
        TransferCB();
        isDirty_ = false;
    }
}

void Light::Draw(void)
{
    InitDirectX::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cbLight_.GetBuffer()->GetGPUVirtualAddress());
}

void Light::TransferCB(void)
{
    // ライトの定数バッファに代入（転送）
    cbLight_.GetConstBuffMap()->lightv_ = -lightDir_;
    cbLight_.GetConstBuffMap()->lightcolor_ = lightColor_;
}

void Light::SetLightDir(const Vector3& lightDir)
{
    lightDir_ = lightDir.normalize();
    isDirty_ = true;
}

void Light::SetLightColor(const Vector3& lightColor)
{
    lightColor_ = lightColor;
    isDirty_ = true;
}
