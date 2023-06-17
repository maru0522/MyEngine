#pragma once
#include "Vector3.h"
#include "ConstBuffer.h"

class DirectionalLight
{
private:
    // íËã`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    struct CBLight_t
    {
        Vector3 lightv_;
        float pad1;
        Vector3 lightcolor_;
        uint32_t isActive_;
    };

    // ä÷êî
    DirectionalLight(void);
    void Update(void);
    void Draw(void);

private:
    void TransferCB(void);

    // ïœêî
    ConstBuffer<CBLight_t> cbLight_;

    bool isDirty_{};
    Vector3 lightDir_{ 1,0,0 };
    Vector3 lightColor_{ 1,1,1 };

    bool isActive_{};

public:
    // setterÅEgetter
    void SetLightDir(const Vector3& lightDir);
    void SetLightColor(const Vector3& lightColor);
    inline void SetIsActive(bool isActive) { isActive_ = isActive; }

    inline const Vector3& GetLightDir(void) { return lightDir_; }
    inline const Vector3& GetLightColor(void) { return lightColor_; }
    inline bool GetIsActive(void) { return isActive_; }
};

