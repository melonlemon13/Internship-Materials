#include "BlinkSkill.h"

#include"Player.h"
#include <cmath>
#include <iostream>
#include "GameManager.h"

using namespace DirectX::SimpleMath;

void BlinkSkill::Init(Player* owner)
{
	// 持ち主をセット
	m_Owner = owner;

    // 仮パラメータ
    SetTimings(
        0,      // activeFrames（即終了）
        600    // cooldownFrames（10秒(60f * 10)）
    );

    // ブリンク中は何もブロックしない
    SetBlocks(
        false,   // attack
        false,   // reload
        false,   // weaponSwitch
        false,   // otherSkills
		false	// weaponActive
        );

    ResetToIdle();
}

bool BlinkSkill::TryActivate()
{
    if (!m_Owner) return false;

    // 地上では使えない
    if(m_Owner->IsOnGround())
		return false; 
	
    // SkillBaseの発動判定（CT中なら失敗）
    auto r = TryStart();
    if (r == SkillBase::StartResult::Failed)
        return false;

    //ダッシュ方向を決める
    // キー入力があれば優先
    // なければカメラForward
    auto dir = m_Owner->GetMoveInputDirXZ();
    if (dir.LengthSquared() <= 0.00001f)
        dir = m_Owner->GetForwardXZ();

    if (dir.LengthSquared() <= 0.00001f)
        return false;

    // Playerにブリンク開始を依頼
    m_Owner->StartBlink(
        dir,
        120.0f, // 合計距離(cm)//Stageは1200f
        18 // フレーム数
    );
    GameManager::GetSound()->Play(SOUND_LABEL_SE_BLINK);
    return true;
}

void BlinkSkill::Update()
{
    // CT進行
    Tick();
}