#include "SkillBase.h"

SkillBase::SkillBase()
{
    ResetToIdle();
}

void SkillBase::SetTimings(int activeFrames, int cooldownFrames)
{
    // 0以上で扱う（負値は0に丸める）
    m_ActiveFrames = (activeFrames < 0) ? 0 : activeFrames;
    m_CooldownFrames = (cooldownFrames < 0) ? 0 : cooldownFrames;
}

void SkillBase::SetBlocks(bool blockAttack, bool blockReload, bool blockWeaponSwitch, bool blockOtherSkills , bool blockWeaponActive)
{
    m_Blocks.attack = blockAttack;
    m_Blocks.reload = blockReload;
    m_Blocks.weaponSwitch = blockWeaponSwitch;
    m_Blocks.otherSkills = blockOtherSkills;
    m_Blocks.weaponActive = blockWeaponActive;
}

//実質的なInit処理
void SkillBase::ResetToIdle()
{
    m_State = State::Idle;
    m_ActiveTimer = 0;
    m_CooldownTimer = 0;
}

// スキルの発動を試みる
SkillBase::StartResult SkillBase::TryStart()
{
	// Idle状態でなければ失敗
    if (m_State != State::Idle)
        return StartResult::Failed;

    // Activeへ
    EnterActive();

    // activeFrames==0 なら即終了扱いでCooldownへ
    if (m_ActiveFrames == 0)
    {
        EnterCooldown();
        return StartResult::StartedAndEnded;
    }

	// 発動成功
    return StartResult::Started;
}


SkillBase::TickEvent SkillBase::Tick()
{
    switch (m_State)
    {
    case State::Idle:
        return TickEvent::None;

    case State::Active:
        if (m_ActiveTimer > 0)
        {
            --m_ActiveTimer;
        }

        // 0になった瞬間にCooldownへ
        if (m_ActiveTimer <= 0)
        {
            EnterCooldown();
            return TickEvent::ExitedActive;
        }
        return TickEvent::None;

    case State::Cooldown:
        if (m_CooldownTimer > 0)
        {
            --m_CooldownTimer;
        }

        // 0になった瞬間にIdleへ
        if (m_CooldownTimer <= 0)
        {
            EnterIdle();
            return TickEvent::BecameReady;
        }
        return TickEvent::None;
    }

    return TickEvent::None;
}

void SkillBase::EnterActive()
{
    m_State = State::Active;
    m_ActiveTimer = m_ActiveFrames;
    m_CooldownTimer = 0;
}

void SkillBase::EnterCooldown()
{
    m_State = State::Cooldown;
    m_ActiveTimer = 0;
    m_CooldownTimer = m_CooldownFrames;
}

void SkillBase::EnterIdle()
{
    m_State = State::Idle;
    m_ActiveTimer = 0;
    m_CooldownTimer = 0;
}

void SkillBase::ForceStartCooldown(int cooldownFrames)
{
    // 負値は0
    m_CooldownFrames = (cooldownFrames < 0) ? 0 : cooldownFrames;

    // いまActive中なら触らない（事故防止）
    if (m_State == State::Active)
        return;

    // Cooldownに入れてタイマー開始
    m_State = State::Cooldown;
    m_ActiveTimer = 0;
    m_CooldownTimer = m_CooldownFrames;
}

void SkillBase::ReduceCooldown(int frames)
{
    if (frames <= 0) return;

    m_CooldownTimer -= frames;
    if (m_CooldownTimer < 0) m_CooldownTimer = 0;
}