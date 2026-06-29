#pragma once

class Player;

class SkillBase
{
public:
	// スキルの状態
    enum class State
    {
        Idle,
        Active,
        Cooldown,
    };

    // TryStart() の結果（activeFrames==0でも安全にOnFinishを呼べるようにする）
    enum class StartResult
    {
        Failed,            // 開始できない（Idle以外）
        Started,           // Activeに入った
        StartedAndEnded,   // activeFrames==0等で、開始した瞬間に終了（Cooldownへ）
    };

    // Tick() のイベント（外側のスキルがOnFinish/OnReadyを確実に呼べる）
    enum class TickEvent
    {
        None,
        ExitedActive, // Active -> Cooldown になった瞬間
        BecameReady,  // Cooldown -> Idle になった瞬間
    };

	// スキルのブロック状態
    struct Blocks
    {
        bool attack = false;
        bool reload = false;
        bool weaponSwitch = false;
        bool otherSkills = false;
        bool weaponActive = false;
    };

protected:
    Player* m_Owner = nullptr;

private:
    State  m_State = State::Idle;

    int    m_ActiveFrames = 0;
    int    m_CooldownFrames = 0;

    int    m_ActiveTimer = 0;
    int    m_CooldownTimer = 0;

    Blocks m_Blocks{};

public:
    SkillBase();

	// 強制的にクールタイムを開始
    void ForceStartCooldown(int cooldownFrames);
    // Cooldown残りを外側から減らす（KillでCT短縮用）
    void ReduceCooldown(int frames);

    // 設定
    void SetTimings(int activeFrames, int cooldownFrames);
    void SetBlocks(bool blockAttack, bool blockReload, bool blockWeaponSwitch, bool blockOtherSkills, bool blockWeaponActive);

    // 状態リセット
    void ResetToIdle();

    // 発動要求
    StartResult TryStart();

    // 毎フレーム更新
    TickEvent Tick();

    //// 状態参照
    bool IsReady() const { return m_State == State::Idle; }
    bool IsActive() const { return m_State == State::Active; }
    bool IsCoolingDown() const { return m_State == State::Cooldown; }
    //State GetState() const { return m_State; }

    // ブロック参照
    Blocks GetBlocks() const { return m_Blocks; }

    int GetCoolTimeSec() const { return m_CooldownTimer; }

private:
    void EnterActive();
    void EnterCooldown();
    void EnterIdle();
};

