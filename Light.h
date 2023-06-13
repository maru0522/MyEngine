#pragma once
#include "Vector3.h"
#include "ConstBuffer.h"

class Light
{
private:
    // íËã`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    struct CBLight_t
    {
        Vector3 lightv_;
        Vector3 lightcolor_;
    };

    // ä÷êî
    Light(void);
    void Update(void);
    void Draw(void);

private:
    void TransferCB(void);

    // ïœêî
    ConstBuffer<CBLight_t> cbLight_;

    bool isDirty_{};
    Vector3 lightDir_{ 1,0,0 };
    Vector3 lightColor_{ 1,1,1 };

public:
    // setterÅEgetter
    void SetLightDir(const Vector3& lightDir);
    void SetLightColor(const Vector3& lightColor);
};

