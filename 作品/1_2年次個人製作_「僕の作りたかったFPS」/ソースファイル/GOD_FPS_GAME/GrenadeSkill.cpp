#include "GrenadeSkill.h"

#include <SimpleMath.h>
#include"Player.h"
#include"Enemy.h"
#include "GameManager.h"
#include "Grenade.h"

using namespace DirectX::SimpleMath;

void GrenadeSkill::Init(Player* owner)
{
	// 持ち主をセット
	m_Owner = owner;

	SetTimings(
		30,      // activeFrames
		900    // cooldownFrames//15秒(60f * 15)
	);

	// 他のスキル発動以外をブロック
	SetBlocks(
		true,  // attack
		true,  // reload
		true,  // weaponSwitch
		false,   // otherSkills
		true  // weaponActive
	);

	ResetToIdle();
}

void GrenadeSkill::Update()
{
	// CT進行
	Tick();
}

bool GrenadeSkill::TryActivate()
{
	if (!m_Owner) return false;
	// 前方向（XZ固定）が取れなければ失敗
	Vector3 forward = m_Owner->GetForwardXZ();
	if (forward.LengthSquared() <= 0.00001f)
		return false;
	//SkillBaseの発動判定（CT中なら失敗）
	auto r = TryStart();
	if (r == SkillBase::StartResult::Failed)
		return false;

	// 成功した瞬間にグレネード生成
	SpawnGrenade();
	return true;
}

void GrenadeSkill::SpawnGrenade()
{
	// 前方向（XZ固定）
	Vector3 forward = m_Owner->GetForwardXZ();
	if (forward.LengthSquared() <= 0.00001f) return;
	forward.Normalize();

	// 右方向（XZ固定）
	Vector3 right = m_Owner->GetRight();
	right.y = 0.0f;
	if (right.LengthSquared() <= 0.00001f) return;
	right.Normalize();

	// 生成位置：前 + 左 + 上
	const Vector3 base = m_Owner->GetPosition();
	const Vector3 spawnPos =
		base
		+ forward * m_SpawnForwardDist
		- right * m_SpawnRightDist
		+ Vector3(0.0f, m_SpawnUpDist, 0.0f);

	Grenade* g = GameManager::GetInstance()->AddObject<Grenade>();
	if (!g) return;

	GameManager::GetSound()->Play(SOUND_LABEL_VOICE_GRENADE);

	//投擲速度：前方投げ+プレイヤー速度継承
	Vector3 pv = m_Owner->GetMoveVelocity(); // cm/frame（差分速度）
	pv.y = 0.0f;

	// 前方向成分だけ抽出
	const float pvForwardMag = pv.Dot(forward);
	const Vector3 pvForward = forward * pvForwardMag;
	const Vector3 pvSide = pv - pvForward;

	// 継承率
	const float inheritForward = 1.2f; // ★ここを上げると「走り投げ」が強くなる
	const float inheritSide = 0.3f; // 横移動は控えめ

	Vector3 inherit = pvForward * inheritForward + pvSide * inheritSide;

	// 継承分の上限（暴走防止）
	const float inheritMax = 25.0f; // cm/frame
	if (inherit.LengthSquared() > inheritMax * inheritMax)
	{
		inherit.Normalize();
		inherit *= inheritMax;
	}

	const Vector3 throwXZ = forward * m_ThrowXZSpeed; // 基本の投擲速度
	Vector3 xzVel = throwXZ + inherit;
	xzVel.y = 0.0f;

	// Params
	Grenade::Params p{};
	p.spawnPos = spawnPos;
	p.xzVel = xzVel;

	p.yVel = m_ThrowYVel;
	p.gravity = m_Gravity;

	p.explodeFrames = m_ExplodeFrames;
	p.radius = m_Radius;

	p.damage = 50;
	p.targets = m_Targets;

	// 壁
	p.wallCols = m_Owner->GetWallCols();
	p.collisionRadius = 12.0f;

	// 跳ね返り
	p.restitution = 0.75f;  // 跳ね返りの強さ（0〜1）
	p.bounceDamp = 0.98f;  // 跳ね返り後の減衰（0.8〜0.99）

	g->Setup(p);
}