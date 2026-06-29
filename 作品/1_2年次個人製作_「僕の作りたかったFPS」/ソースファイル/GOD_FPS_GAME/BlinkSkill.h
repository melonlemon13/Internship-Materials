#pragma once
#include "SkillBase.h"
#include <SimpleMath.h> // 追加: Vector3型のため
#include <SimpleMath.h>

class Player;

class BlinkSkill : public SkillBase
{
public:
    void Init(Player* owner);

    // 発動したらtrueを返す
    bool TryActivate();

    void Update();

    // デバッグやUI用
    bool IsReady() const { return SkillBase::IsReady(); }
};
