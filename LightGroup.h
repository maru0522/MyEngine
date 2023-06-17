#pragma once
#include <cstdint>
#include "Light.h"
#include "Vector3.h"
#include "ConstBuffer.h"
#include "ImguiController.h"

class LightGroup
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    // 定数
    static constexpr uint32_t kDirLightNum_ = 3;

    struct CBLightGroup_t
    {
        Vector3 ambientColor_; // 環境光の色
        float pad;
        DirectionalLight::CBLight_t dirLights_[kDirLightNum_];
    };

    // 関数
    LightGroup(void);
    void Update(void);
    void Draw(void);

private:
    void TransferCB(void);
    void DefaultLightSetting(void);
    void DebugImGui(void);

    // 変数
    ConstBuffer<CBLightGroup_t> cbLightGroup_;
    Vector3 ambientColor_{ 1,1,1 };
    DirectionalLight dirLights_[kDirLightNum_];
    bool isDirty_{};

public:
    // setter・getter
    void SetAmbientColor(const Vector3& color);
    void SetLightDir(size_t index, const Vector3& lightDir);
    void SetDirLightColor(size_t index, const Vector3& lightColor);
    inline void SetDirLightActive(size_t index, bool isActive) { dirLights_[index].SetIsActive(isActive); }

};

