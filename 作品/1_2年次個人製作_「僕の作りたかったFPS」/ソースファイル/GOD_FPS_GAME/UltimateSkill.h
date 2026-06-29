#pragma once
#include "SkillBase.h"

class UltimateSkill : public SkillBase
{
private:
    // 定数
	//発動に必要なキル数、移動速度倍率
    static constexpr int   kCutPerKill = 240;   // 4秒 = 300f
    static constexpr float kMoveMul = 1.5f;

public:
    void Init(Player* owner);

    bool TryActivate();

    // 毎フレーム更新（CT減少 + Active管理）
    void Update();

    // Enemy死亡時に呼ぶ（CT短縮）
    void OnKill(int kills = 1);

    // ULT中だけ速度倍率を返す（Playerの移動計算で使う）
    float GetMoveSpeedMul() const { return IsActive() ? kMoveMul : 1.0f; }
};

