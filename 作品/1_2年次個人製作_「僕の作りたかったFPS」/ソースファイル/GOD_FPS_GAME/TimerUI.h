#pragma once
#include "ObjectBase.h"
#include "NumberSprite.h"

class TimerUI : public ObjectBase
{
public:
    void Init() override;
    void Update() override;
    void Draw(Camera* cam) override;
    void Uninit() override;

    void SetTimeSec(int sec) { m_TimeSec = sec; }

    void SetBasePos(const DirectX::SimpleMath::Vector3& pos);
    void SetSpacing(float s);
    void SetScale(const DirectX::SimpleMath::Vector3& scl);

private:
    int m_TimeSec = 120;

    NumberSprite m_Number;
};

