#include "StageScene.h"
#include"input.h"
#include "GameManager.h"
#include "Texture2D.h"
#include "Ground.h"

//クラスのインクルード
#include"WeaponBase.h"
#include"Player.h"
#include"Enemy.h"
#include"Knife.h"
#include"Pistol.h"
#include "Wall.h"
#include "Grenade.h"
#include"LoadingUI.h"
#include "ExplosionEffect.h"
#include"UltSword.h"

using namespace DirectX::SimpleMath;

static float ClampF(float v, float a, float b)
{
	return (v < a) ? a : (v > b) ? b : v;
}

// pos が「壁にめり込む（または近すぎる）」なら true を返す
// enemyRadius は Enemy の m_CollisionRadius と同じ値を渡す
static bool IsOverlappingWallXZ(
	const DirectX::SimpleMath::Vector3& pos,
	float enemyRadius,
	const std::vector<WallCollider>& wallCols)
{
	const float r2 = enemyRadius * enemyRadius;

	for (const auto& w : wallCols)
	{
		const float minX = w.center.x - w.half.x;
		const float maxX = w.center.x + w.half.x;
		const float minZ = w.center.z - w.half.z;
		const float maxZ = w.center.z + w.half.z;

		const float cx = ClampF(pos.x, minX, maxX);
		const float cz = ClampF(pos.z, minZ, maxZ);

		const float dx = pos.x - cx;
		const float dz = pos.z - cz;

		const float d2 = dx * dx + dz * dz;

		// 「触れるのも避けたい」なら <= にしてもOK
		if (d2 < r2)
		{
			return true; // 壁にめり込む（スポーンNG）
		}
	}

	return false; // どの壁とも被ってない（スポーンOK）
}

void StageScene::StartGameplay()
{
	GameManager::GetSound()->Play(SOUND_LABEL_SE_START);

	GameManager::GetInstance()->AddObject<Ground>();

	Now_Time = End_Time;
	Time_Count = 0;

	m_Elapsed_Frames = 0;
	m_Spawn_Timer = 0;
	m_Phase_Index = 0;
	m_pEnemies.clear();

	m_IsEnding = false;

	m_Phases =
	{
		{  0,  4, 120, 1 },
		{ 30,  7,  90, 1 },
		{ 60, 10,  75, 2 },
		{ 90, 14,  60, 2 },
		{120, 20,  45, 3 },
	};

	constexpr float FLOOR_HALF = 600.0f;
	constexpr float PLAYER_RADIUS = 20.0f;
	constexpr float SPAWN_MARGIN = 10.0f;
	constexpr float SAFE_HALF = FLOOR_HALF - PLAYER_RADIUS - SPAWN_MARGIN;

	// Player生成
	m_Player = GameManager::GetInstance()->AddObject<Player>();
	m_Player->SetPosition({ 0.0f, 0.0f, -SAFE_HALF * 0.5f });

	m_Player->SetEnemies(&m_pEnemies);
	m_Player->SetupWeapons(&m_pEnemies);
	m_Player->GetGrenadeSkill().SetTargets(&m_pEnemies);

	m_Player->ResetKillStats();

	// 壁生成
	constexpr float GROUND_Y = -20.0f;
	float size = 1200.0f;
	float half = size * 0.5f;
	float wallHeight = 400.0f;
	float wallCenterY = GROUND_Y + wallHeight * 0.5f;
	float wallThickness = 20.0f;

	AddWallBox({ 0.0f, wallCenterY, +half + wallThickness * 0.5f }, { size, wallHeight, wallThickness });
	AddWallBox({ 0.0f, wallCenterY, -half - wallThickness * 0.5f }, { size, wallHeight, wallThickness });
	AddWallBox({ +half + wallThickness * 0.5f, wallCenterY, 0.0f }, { wallThickness, wallHeight, size });
	AddWallBox({ -half - wallThickness * 0.5f, wallCenterY, 0.0f }, { wallThickness, wallHeight, size });

	// 天井
	float ceilingY = GROUND_Y + 200.0f;
	float thickness = 10.0f;
	float ceilingCenterY = ceilingY - thickness * 0.5f;

	Wall* w = GameManager::GetInstance()->AddObject<Wall>();
	w->SetPosition(Vector3(0.0f, ceilingCenterY, 0.0f));
	w->SetScale(Vector3(size, thickness, size));
	m_Walls.push_back(w);

	// Playerに壁コライダ
	m_Player->SetWallColliders(GetWallColliders());

	{
		// UI レティクル
		Texture2D* reticle = GameManager::GetInstance()->AddUIObject<Texture2D>();
		reticle->SetTexture("assets/texture/reticle.png");
		reticle->SetPosition(0.0f, 0.0f, 0.0f);
		reticle->SetScale(1920.0f, 1080.0f, 0.0f);
		m_pUITextures.emplace_back(reticle);

		//Skill
		//ULT
		Texture2D* ULT = GameManager::GetInstance()->AddUIObject<Texture2D>();
		ULT->SetTexture("assets/texture/ULT.png");
		ULT->SetPosition(-800.0f, -400.0f, 0.0f);
		ULT->SetScale(100.0f, 100.0f, 0.0f);
		m_pUITextures.emplace_back(ULT);
		//Q
		Texture2D* Q = GameManager::GetInstance()->AddUIObject<Texture2D>();
		Q->SetTexture("assets/texture/Q.png");
		Q->SetPosition(-800.0f, -300.0f, 0.0f);
		Q->SetScale(50.0f, 50.0f, 0.0f);
		m_pUITextures.emplace_back(Q);
		//Grenade
		Texture2D* Grenade = GameManager::GetInstance()->AddUIObject<Texture2D>();
		Grenade->SetTexture("assets/texture/Grenade.png");
		Grenade->SetPosition(-650.0f, -400.0f, 0.0f);
		Grenade->SetScale(100.0f, 100.0f, 0.0f);
		m_pUITextures.emplace_back(Grenade);
		//E
		Texture2D* E = GameManager::GetInstance()->AddUIObject<Texture2D>();
		E->SetTexture("assets/texture/E.png");
		E->SetPosition(-650.0f, -300.0f, 0.0f);
		E->SetScale(50.0f, 50.0f, 0.0f);
		m_pUITextures.emplace_back(E);
		//Blink
		Texture2D* Blink = GameManager::GetInstance()->AddUIObject<Texture2D>();
		Blink->SetTexture("assets/texture/Blink.png");
		Blink->SetPosition(-500.0f, -400.0f, 0.0f);
		Blink->SetScale(100.0f, 100.0f, 0.0f);
		m_pUITextures.emplace_back(Blink);
		//SPACE
		Texture2D* SPACE = GameManager::GetInstance()->AddUIObject<Texture2D>();
		SPACE->SetTexture("assets/texture/SPACE.png");
		SPACE->SetPosition(-500.0f, -280.0f, 0.0f);
		SPACE->SetScale(100.0f, 100.0f, 0.0f);
		m_pUITextures.emplace_back(SPACE);
		//Texture2D* m_Grenade_Digits[2] = {};
		//Texture2D* m_Ult_Digits[2] = {};
		//Texture2D* m_Blink_Digits[2] = {};
		//CT
		// ULT
		// 十の位
		m_Ult_Digits[0] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_Ult_Digits[0]->SetTexture("assets/texture/number.png");
		m_Ult_Digits[0]->SetPosition(-810.0f, -460.0f, 0.0f);
		m_Ult_Digits[0]->SetScale(20.0f, 20.0f, 0.0f);
		m_Ult_Digits[0]->SetUV(10, 1, 10, 1);
		m_pUITextures.emplace_back(m_Ult_Digits[0]);

		// 一の位
		m_Ult_Digits[1] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_Ult_Digits[1]->SetTexture("assets/texture/number.png");
		m_Ult_Digits[1]->SetPosition(-790.0f, -460.0f, 0.0f);
		m_Ult_Digits[1]->SetScale(20.0f, 20.0f, 0.0f);
		m_Ult_Digits[1]->SetUV(1, 1, 10, 1);
		m_pUITextures.emplace_back(m_Ult_Digits[1]);

		//Grenade
		// 十の位
		m_Grenade_Digits[0] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_Grenade_Digits[0]->SetTexture("assets/texture/number.png");
		m_Grenade_Digits[0]->SetPosition(-660.0f, -460.0f, 0.0f);
		m_Grenade_Digits[0]->SetScale(20.0f, 20.0f, 0.0f);
		m_Grenade_Digits[0]->SetUV(1, 1, 10, 1);
		m_pUITextures.emplace_back(m_Grenade_Digits[0]);

		// 一の位
		m_Grenade_Digits[1] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_Grenade_Digits[1]->SetTexture("assets/texture/number.png");
		m_Grenade_Digits[1]->SetPosition(-640.0f, -460.0f, 0.0f);
		m_Grenade_Digits[1]->SetScale(20.0f, 20.0f, 0.0f);
		m_Grenade_Digits[1]->SetUV(1, 1, 10, 1);
		m_pUITextures.emplace_back(m_Grenade_Digits[1]);

		//Blink
		// 十の位
		m_Blink_Digits[0] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_Blink_Digits[0]->SetTexture("assets/texture/number.png");
		m_Blink_Digits[0]->SetPosition(-510.0f, -460.0f, 0.0f);
		m_Blink_Digits[0]->SetScale(20.0f, 20.0f, 0.0f);
		m_Blink_Digits[0]->SetUV(1, 1, 10, 1);
		m_pUITextures.emplace_back(m_Blink_Digits[0]);

		// 一の位
		m_Blink_Digits[1] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_Blink_Digits[1]->SetTexture("assets/texture/number.png");
		m_Blink_Digits[1]->SetPosition(-490.0f, -460.0f, 0.0f);
		m_Blink_Digits[1]->SetScale(20.0f, 20.0f, 0.0f);
		m_Blink_Digits[1]->SetUV(1, 1, 10, 1);
		m_pUITextures.emplace_back(m_Blink_Digits[1]);

		//タイマー
		const float baseX = 0.0f;
		const float baseY = 450.0f;
		const float scaleW = 60.0f;
		const float scaleH = 60.0f;
		const float step = 55.0f;

		// 百の位
		m_TimeDigits[0] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_TimeDigits[0]->SetTexture("assets/texture/number.png");
		m_TimeDigits[0]->SetPosition(baseX - step, baseY, 0.0f);
		m_TimeDigits[0]->SetScale(scaleW, scaleH, 0.0f);
		m_TimeDigits[0]->SetUV(2, 1, 10, 1);
		m_pUITextures.emplace_back(m_TimeDigits[0]);

		// 十の位
		m_TimeDigits[1] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_TimeDigits[1]->SetTexture("assets/texture/number.png");
		m_TimeDigits[1]->SetPosition(baseX, baseY, 0.0f);
		m_TimeDigits[1]->SetScale(scaleW, scaleH, 0.0f);
		m_TimeDigits[1]->SetUV(3, 1, 10, 1);
		m_pUITextures.emplace_back(m_TimeDigits[1]);

		// 一の位
		m_TimeDigits[2] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_TimeDigits[2]->SetTexture("assets/texture/number.png");
		m_TimeDigits[2]->SetPosition(baseX + step, baseY, 0.0f);
		m_TimeDigits[2]->SetScale(scaleW, scaleH, 0.0f);
		m_TimeDigits[2]->SetUV(1, 1, 10, 1);
		m_pUITextures.emplace_back(m_TimeDigits[2]);

		//武器、弾数
		//Pistol
		Texture2D* Pistol = GameManager::GetInstance()->AddUIObject<Texture2D>();
		Pistol->SetTexture("assets/texture/Pistol.png");
		Pistol->SetPosition(500.0f, -400.0f, 0.0f);
		Pistol->SetScale(100.0f, 100.0f, 0.0f);
		m_pUITextures.emplace_back(Pistol);

		//1
		Texture2D* No1 = GameManager::GetInstance()->AddUIObject<Texture2D>();
		No1->SetTexture("assets/texture/1.png");
		No1->SetPosition(500.0f, -300.0f, 0.0f);
		No1->SetScale(50.0f, 50.0f, 0.0f);
		m_pUITextures.emplace_back(No1);

		//弾数
		// 十の位
		m_Pistol_Digits[0] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_Pistol_Digits[0]->SetTexture("assets/texture/number.png");
		m_Pistol_Digits[0]->SetPosition(490.0f, -460.0f, 0.0f);
		m_Pistol_Digits[0]->SetScale(20.0f, 20.0f, 0.0f);
		m_Pistol_Digits[0]->SetUV(2, 1, 10, 1);
		m_pUITextures.emplace_back(m_Pistol_Digits[0]);

		// 一の位
		m_Pistol_Digits[1] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_Pistol_Digits[1]->SetTexture("assets/texture/number.png");
		m_Pistol_Digits[1]->SetPosition(510.0f, -460.0f, 0.0f);
		m_Pistol_Digits[1]->SetScale(20.0f, 20.0f, 0.0f);
		m_Pistol_Digits[1]->SetUV(3, 1, 10, 1);
		m_pUITextures.emplace_back(m_Pistol_Digits[1]);

		//AR
		Texture2D* AR = GameManager::GetInstance()->AddUIObject<Texture2D>();
		AR->SetTexture("assets/texture/AR.png");
		AR->SetPosition(650.0f, -400.0f, 0.0f);
		AR->SetScale(100.0f, 100.0f, 0.0f);
		m_pUITextures.emplace_back(AR);

		//2
		Texture2D* No2 = GameManager::GetInstance()->AddUIObject<Texture2D>();
		No2->SetTexture("assets/texture/2.png");
		No2->SetPosition(650.0f, -300.0f, 0.0f);
		No2->SetScale(50.0f, 50.0f, 0.0f);
		m_pUITextures.emplace_back(No2);

		//弾数
		// 十の位
		m_AR_Digits[0] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_AR_Digits[0]->SetTexture("assets/texture/number.png");
		m_AR_Digits[0]->SetPosition(640.0f, -460.0f, 0.0f);
		m_AR_Digits[0]->SetScale(20.0f, 20.0f, 0.0f);
		m_AR_Digits[0]->SetUV(3, 1, 10, 1);
		m_pUITextures.emplace_back(m_AR_Digits[0]);

		// 一の位
		m_AR_Digits[1] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_AR_Digits[1]->SetTexture("assets/texture/number.png");
		m_AR_Digits[1]->SetPosition(660.0f, -460.0f, 0.0f);
		m_AR_Digits[1]->SetScale(20.0f, 20.0f, 0.0f);
		m_AR_Digits[1]->SetUV(6, 1, 10, 1);
		m_pUITextures.emplace_back(m_AR_Digits[1]);

		//Knife
		Texture2D* Knife = GameManager::GetInstance()->AddUIObject<Texture2D>();
		Knife->SetTexture("assets/texture/Knife.png");
		Knife->SetPosition(800.0f, -400.0f, 0.0f);
		Knife->SetScale(100.0f, 100.0f, 0.0f);
		m_pUITextures.emplace_back(Knife);

		//3
		Texture2D* No3 = GameManager::GetInstance()->AddUIObject<Texture2D>();
		No3->SetTexture("assets/texture/3.png");
		No3->SetPosition(800.0f, -300.0f, 0.0f);
		No3->SetScale(50.0f, 50.0f, 0.0f);
		m_pUITextures.emplace_back(No3);

		//弾数
		// 十の位
		m_Knife_Digits[0] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_Knife_Digits[0]->SetTexture("assets/texture/number.png");
		m_Knife_Digits[0]->SetPosition(790.0f, -460.0f, 0.0f);
		m_Knife_Digits[0]->SetScale(20.0f, 20.0f, 0.0f);
		m_Knife_Digits[0]->SetUV(10, 1, 10, 1);
		m_pUITextures.emplace_back(m_Knife_Digits[0]);

		// 一の位
		m_Knife_Digits[1] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_Knife_Digits[1]->SetTexture("assets/texture/number.png");
		m_Knife_Digits[1]->SetPosition(810.0f, -460.0f, 0.0f);
		m_Knife_Digits[1]->SetScale(20.0f, 20.0f, 0.0f);
		m_Knife_Digits[1]->SetUV(10, 1, 10, 1);
		m_pUITextures.emplace_back(m_Knife_Digits[1]);

		const float hpBaseX = 0.0f;
		const float hpBaseY = -450.0f;   // 画面下
		const float hpScaleW = 60.0f;
		const float hpScaleH = 60.0f;
		const float hpStep = 55.0f;

		// 百の位
		m_HP_Digits[0] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_HP_Digits[0]->SetTexture("assets/texture/number.png");
		m_HP_Digits[0]->SetPosition(hpBaseX - hpStep, hpBaseY, 0.0f);
		m_HP_Digits[0]->SetScale(hpScaleW, hpScaleH, 0.0f);
		m_HP_Digits[0]->SetUV(1, 1, 10, 1);
		m_pUITextures.emplace_back(m_HP_Digits[0]);

		// 十の位
		m_HP_Digits[1] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_HP_Digits[1]->SetTexture("assets/texture/number.png");
		m_HP_Digits[1]->SetPosition(hpBaseX, hpBaseY, 0.0f);
		m_HP_Digits[1]->SetScale(hpScaleW, hpScaleH, 0.0f);
		m_HP_Digits[1]->SetUV(1, 1, 10, 1);
		m_pUITextures.emplace_back(m_HP_Digits[1]);

		// 一の位
		m_HP_Digits[2] = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_HP_Digits[2]->SetTexture("assets/texture/number.png");
		m_HP_Digits[2]->SetPosition(hpBaseX + hpStep, hpBaseY, 0.0f);
		m_HP_Digits[2]->SetScale(hpScaleW, hpScaleH, 0.0f);
		m_HP_Digits[2]->SetUV(1, 1, 10, 1);
		m_pUITextures.emplace_back(m_HP_Digits[2]);
	}
}

void StageScene::Init()
{
	//マウス固定
	Input::SetMouseLock(true);

	// ロードSE再生フラグ初期化
	m_LoadSeStarted = false;

	// ローディングUI生成
	m_LoadUI = GameManager::GetInstance()->AddUIObject<LoadingUI>();
	if (m_LoadUI) m_LoadUI->SetVisible(true);

	//3Dオブジェクトのロード
	// ロードステップ初期化
	m_LoadSteps.clear();
	// ロードステップ登録
	m_LoadSteps.push_back([]() { Pistol::PreloadAssets(); });
	m_LoadSteps.push_back([]() { Assaultrifle::PreloadAssets(); });
	m_LoadSteps.push_back([]() { Knife::PreloadAssets(); });
	m_LoadSteps.push_back([]() { Grenade::PreloadAssets(); });
	m_LoadSteps.push_back([]() { Enemy::PreloadAssets(); });
	m_LoadSteps.push_back([]() { ExplosionEffect::PreloadAssets(); });
	m_LoadSteps.push_back([]() { UltSword::PreloadAssets(); });

	// ローディング状態開始
	m_IsLoading = true;
	// ロードステップ初期化
	m_LoadStep = 0;
	// これで「1フレーム目はLoadingUI表示だけ」にできる
	m_BeginLoadingNextFrame = true;

	// ロードSE再生(ループさせる)
	if (!m_LoadSeStarted) { GameManager::GetSound()->Play(SOUND_LABEL_SE_LOADING); m_LoadSeStarted = true; }

	// ゲームはまだ開始しない
	m_GameStarted = false;
}

void StageScene::Uninit()
{
	////マウス固定解除
	//Input::SetMouseLock(false);

	//if (m_LoadUI)
	//{
	//	GameManager::GetInstance()->DeleteObject(m_LoadUI);
	//	m_LoadUI = nullptr;
	//}

	//for (auto* ui : m_pUITextures)
	//{
	//	if (!ui) continue;
	//	GameManager::GetInstance()->DeleteObject(ui);
	//}
	//m_pUITextures.clear();

	//if (m_Player)
	//{
	//	GameManager::GetInstance()->DeleteObject(m_Player);
	//	m_Player = nullptr;
	//}

	//for (auto* e : m_pEnemies)
	//{
	//	if (!e) continue;
	//	GameManager::GetInstance()->DeleteObject(e);
	//}
	//m_pEnemies.clear();

	//for (auto* w : m_Walls)
	//{
	//	if (!w) continue;
	//	GameManager::GetInstance()->DeleteObject(w);
	//}
	//m_Walls.clear();

	//m_WallCols.clear();

	//m_Phases.clear();

	Input::SetMouseLock(false);

	m_LoadUI = nullptr;

	m_pUITextures.clear();

	m_Player = nullptr;

	m_pEnemies.clear();

	m_Walls.clear();

	m_WallCols.clear();

	m_Phases.clear();
	//deleteオールオブジェクトで勝手に消してくれるので参照を消すだけ
}

void StageScene::Update()
{
	// Loading モード
	if (m_IsLoading)
	{
		if (m_LoadUI) m_LoadUI->SetVisible(true);

		// 1フレーム目はUIを確実に出すだけ（ロード開始を遅らせる）
		if (m_BeginLoadingNextFrame)
		{
			m_BeginLoadingNextFrame = false;
			return;
		}

		// 1フレームに1個だけロード
		if (m_LoadStep < (int)m_LoadSteps.size())
		{
			m_LoadSteps[m_LoadStep]();
			m_LoadStep++;
			return;
		}

		// ロード完了
		m_IsLoading = false;
		if (m_LoadUI) m_LoadUI->SetVisible(false);

		// ここでゲーム生成（1回だけ）
		if (!m_GameStarted)
		{
			if (m_LoadSeStarted) { GameManager::GetSound()->Stop(SOUND_LABEL_SE_LOADING);; m_LoadSeStarted = false; }
			StartGameplay();
			m_GameStarted = true;
		}
	}
	if (!m_Player) return;

	//通常の更新処理
	if (Now_Time <= 0)
	{

	}
	else
	{
		Time_Count++;
		if (Time_Count >= 60)
		{
			Now_Time--;
			Time_Count = 0;
			if (Now_Time <= 0)
			{
				Now_Time = 0;
			}

		}
	}
	{
		//UI更新
		int sec = Now_Time;

		int h = sec / 100;        // 百の位
		int t = (sec / 10) % 10;  // 十の位
		int o = sec % 10;         // 一の位

		if (m_TimeDigits[0]) m_TimeDigits[0]->SetUV((float)(h + 1), 1, 10, 1);
		if (m_TimeDigits[1]) m_TimeDigits[1]->SetUV((float)(t + 1), 1, 10, 1);
		if (m_TimeDigits[2]) m_TimeDigits[2]->SetUV((float)(o + 1), 1, 10, 1);

		int ultCTFrames = m_Player->GetUltimateSkill().GetCoolTimeSec();
		int grenadeCTFrames = m_Player->GetGrenadeSkill().GetCoolTimeSec();
		int blinkCTFrames = m_Player->GetBlinkSkill().GetCoolTimeSec();

		if (ultCTFrames < 0) ultCTFrames = 0;
		if (grenadeCTFrames < 0) grenadeCTFrames = 0;
		if (blinkCTFrames < 0) blinkCTFrames = 0;

		// 切り上げ秒：残り1フレームでも 1秒と表示される
		int ultSec = (ultCTFrames + 59) / 60;
		int grenadeSec = (grenadeCTFrames + 59) / 60;
		int blinkSec = (blinkCTFrames + 59) / 60;

		if (ultSec > 99) ultSec = 99;
		if (grenadeSec > 99) grenadeSec = 99;
		if (blinkSec > 99) blinkSec = 99;

		// ULT
		{
			int tens = (ultSec / 10) % 10;
			int ones = ultSec % 10;
			if (m_Ult_Digits[0]) m_Ult_Digits[0]->SetUV((float)(tens + 1), 1, 10, 1);
			if (m_Ult_Digits[1]) m_Ult_Digits[1]->SetUV((float)(ones + 1), 1, 10, 1);
		}

		// Grenade
		{
			int tens = (grenadeSec / 10) % 10;
			int ones = grenadeSec % 10;
			if (m_Grenade_Digits[0]) m_Grenade_Digits[0]->SetUV((float)(tens + 1), 1, 10, 1);
			if (m_Grenade_Digits[1]) m_Grenade_Digits[1]->SetUV((float)(ones + 1), 1, 10, 1);
		}

		// Blink
		{
			int tens = (blinkSec / 10) % 10;
			int ones = blinkSec % 10;
			if (m_Blink_Digits[0]) m_Blink_Digits[0]->SetUV((float)(tens + 1), 1, 10, 1);
			if (m_Blink_Digits[1]) m_Blink_Digits[1]->SetUV((float)(ones + 1), 1, 10, 1);
		}

		WeaponBase* pistol = m_Player->GetWeapon(0); // 0: Pistol
		WeaponBase* ar = m_Player->GetWeapon(1); // 1: AR

		int pistolAmmo = pistol ? pistol->GetMagAmmo() : 0;
		int arAmmo = ar ? ar->GetMagAmmo() : 0;

		// clamp
		if (pistolAmmo < 0) pistolAmmo = 0;
		if (arAmmo < 0)     arAmmo = 0;
		if (pistolAmmo > 99) pistolAmmo = 99;
		if (arAmmo > 99)     arAmmo = 99;

		// Pistol
		{
			int tens = (pistolAmmo / 10) % 10;
			int ones = pistolAmmo % 10;

			if (m_Pistol_Digits[0]) m_Pistol_Digits[0]->SetUV((float)(tens + 1), 1, 10, 1);
			if (m_Pistol_Digits[1]) m_Pistol_Digits[1]->SetUV((float)(ones + 1), 1, 10, 1);
		}

		// AR
		{
			int tens = (arAmmo / 10) % 10;
			int ones = arAmmo % 10;

			if (m_AR_Digits[0]) m_AR_Digits[0]->SetUV((float)(tens + 1), 1, 10, 1);
			if (m_AR_Digits[1]) m_AR_Digits[1]->SetUV((float)(ones + 1), 1, 10, 1);
		}

		{
			//HP
			int hp = m_Player->GetHp();
			if (hp < 0) hp = 0;
			if (hp > 999) hp = 999;

			int h = hp / 100;
			int t = (hp / 10) % 10;
			int o = hp % 10;

			m_HP_Digits[0]->SetUV((float)(h + 1), 1, 10, 1);
			m_HP_Digits[1]->SetUV((float)(t + 1), 1, 10, 1);
			m_HP_Digits[2]->SetUV((float)(o + 1), 1, 10, 1);
		}
	}

	//Playerが存在しないなら処理をスキップ,何もしない
	//Playerをdeleteした後に通ってしまった場合のバグ対策
	if (!m_Player) return;

	//経過時間
	m_Elapsed_Frames++;

	//死んだ敵の片づけ
	CleanupDeadEnemies();

	//Phase更新
	UpdatePhaseByTime();

	const EnemyPhase& phase = m_Phases[m_Phase_Index];

	//同時最大数に達してるならスポーンしない
	if ((int)m_pEnemies.size() < phase.maxEnemies)
	{
		// タイマー進行
		if (m_Spawn_Timer > 0)
		{
			m_Spawn_Timer--;
		}
		else
		{
			// 1回で複数スポーン
			int canSpawn = phase.maxEnemies - (int)m_pEnemies.size();
			int spawnCount = (std::min)(phase.spawnPerTick, canSpawn);

			// 壁判定に使うコライダ一覧
			auto* walls = GetWallColliders();
			if (!walls)
			{
				// 壁情報が無いなら今回は湧かせない
				m_Spawn_Timer = phase.intervalFrames;
				return;
			}

			constexpr float enemyRadius = 5.0f;

			// 1体につき最大何回引き直すか
			constexpr int kMaxTries = 10;

			for (int i = 0; i < spawnCount; ++i)
			{
				const DirectX::SimpleMath::Vector3 p = m_Player->GetPosition();

				bool spawnedOne = false;

				for (int tries = 0; tries < kMaxTries; ++tries)
				{
					// 半径 80〜180 のリングにランダム
					float radius = 80.0f + (rand() % 7000) / 7000.0f * 100.0f;
					float ang = (rand() % 6283) / 1000.0f;

					DirectX::SimpleMath::Vector3 spawnPos;
					spawnPos.x = p.x + cosf(ang) * radius;
					spawnPos.z = p.z + sinf(ang) * radius;
					spawnPos.y = p.y + EyeHeight;

					// 壁にめり込む候補は捨てて引き直し（角偏り防止）
					if (IsOverlappingWallXZ(spawnPos, enemyRadius, *walls))
					{
						continue; // tries の次へ
					}

					// OK
					SpawnEnemy(spawnPos);
					spawnedOne = true;
					break; // tries を抜けて次の i（次の敵）へ
				}

				// 全部失敗したらこの1体は今回は湧かさない（何もしない）
				(void)spawnedOne;
			}

			// 次のスポーンまで待機
			m_Spawn_Timer = phase.intervalFrames;
		}
	}

	// GameOver（プレイヤー死亡）
	if (m_Player->GetHp() <= 0)
	{
		m_IsEnding = true;

		PlayResultData r;
		r.outcome = MatchOutcome::GameOver;
		r.survivalFrames = m_Elapsed_Frames;

		const auto& ks = m_Player->GetKillStats();
		r.totalKills = ks.total;
		r.pistolKills = ks.pistol;
		r.arKills = ks.ar;
		r.knifeKills = ks.knife;
		r.grenadeKills = ks.grenade;
		r.ultKills = ks.ult;

		GameManager::GetInstance()->SetLastResult(r);
		GameManager::GetInstance()->ChangeScene(GAMEOVER);
		return;
	}

	// Clear（時間切れ）
	if (Now_Time <= 0)
	{
		m_IsEnding = true;

		PlayResultData r;
		r.outcome = MatchOutcome::Clear;
		r.survivalFrames = m_Elapsed_Frames;

		const auto& ks = m_Player->GetKillStats();
		r.totalKills = ks.total;
		r.pistolKills = ks.pistol;
		r.arKills = ks.ar;
		r.knifeKills = ks.knife;
		r.grenadeKills = ks.grenade;
		r.ultKills = ks.ult;

		GameManager::GetInstance()->SetLastResult(r);
		GameManager::GetInstance()->ChangeScene(RESULT);
		return;
	}
}

Enemy* StageScene::SpawnEnemy(const DirectX::SimpleMath::Vector3& pos)
{
	Enemy* e = GameManager::GetInstance()->AddObject<Enemy>();
	e->SetWallColliders(GetWallColliders());
	if (!e) return nullptr;

	e->SetTarget(m_Player);
	e->SetPosition(pos);
	e->SetPlayer(m_Player);

	m_pEnemies.push_back(e);
	return e;
}

void StageScene::CleanupDeadEnemies()
{
	m_pEnemies.erase(
		std::remove_if(
			m_pEnemies.begin(),
			m_pEnemies.end(),
			[](Enemy* e)
			{
				return e == nullptr || e->GetState() == Enemy::State::Dead;
			}
		),
		m_pEnemies.end()
	);
}

void StageScene::UpdatePhaseByTime()
{
	//60fなので/60
	int sec = m_Elapsed_Frames / 60;

	for (int i = (int)m_Phases.size() - 1; i >= 0; --i)
	{
		if (sec >= m_Phases[i].startSec)
		{
			m_Phase_Index = i;
			return;
		}
	}
	m_Phase_Index = 0;
}

Wall* StageScene::AddWallBox(const Vector3& pos, const Vector3& scale)
{
	Wall* w = GameManager::GetInstance()->AddObject<Wall>();
	w->SetPosition(pos);
	w->SetScale(scale);

	m_Walls.push_back(w);

	WallCollider col;
	col.center = pos;
	col.half = { scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f };
	m_WallCols.push_back(col);

	return w;
}