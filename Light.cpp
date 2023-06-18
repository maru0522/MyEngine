#include "Light.h"
#include "InitDirectX.h"

DirectionalLight::DirectionalLight(void)
{
    cbLight_.Create();
    TransferCB();
}

void DirectionalLight::Update(void)
{
    if (isDirty_) {
        TransferCB();
        isDirty_ = false;
    }
}

void DirectionalLight::Draw(void)
{
    InitDirectX::GetInstance()->GetCommandList()->
        SetGraphicsRootConstantBufferView(4, cbLight_.GetBuffer()->GetGPUVirtualAddress());
}

void DirectionalLight::TransferCB(void)
{
    // ライトの定数バッファに代入（転送）
    cbLight_.GetConstBuffMap()->lightv = -lightDir_;
    cbLight_.GetConstBuffMap()->lightcolor = lightColor_;
}

void DirectionalLight::SetLightDir(const Vector3& lightDir)
{
    lightDir_ = lightDir.normalize();
    isDirty_ = true;
}

void DirectionalLight::SetLightColor(const Vector3& lightColor)
{
    lightColor_ = lightColor;
    isDirty_ = true;
}
