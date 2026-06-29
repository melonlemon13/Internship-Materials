#include "UltimateSkill.h"

#include "Player.h"
#include <iostream>

void UltimateSkill::Init(Player* owner)
{
    m_Owner = owner;

    SetTimings
    (
		480, // 8•b
		5400 // 90•b
    );

    // ULT’†‚Í‘S•”ƒuƒƒbƒN 
    SetBlocks(
        true,  // attack
        true,  // reload
        true,  // weaponSwitch
        true,  // otherSkills
        true   // weaponActive
    );

    //ƒQ[ƒ€ŠJn‚©‚çCTŠJn
    ForceStartCooldown(5400);
}

bool UltimateSkill::TryActivate()
{
    if (!m_Owner) return false;
    if (!IsReady()) return false;   // Idle‚¶‚á‚È‚¯‚ê‚Î•s‰Â

    auto r = SkillBase::TryStart();
    return (r != StartResult::Failed);
}

void UltimateSkill::Update()
{
    Tick();
}

void UltimateSkill::OnKill(int kill)
{
    if (kill <= 0) return;

    // Active’†‚ÍCT’Zk‚µ‚È‚¢
    if (IsActive())
        return;

    // Cooldown’†‚¾‚¯’Zk‚·‚é
    if (!IsCoolingDown())
        return;

	// Kill•ªCT’Zk
    ReduceCooldown(kCutPerKill * kill);
}