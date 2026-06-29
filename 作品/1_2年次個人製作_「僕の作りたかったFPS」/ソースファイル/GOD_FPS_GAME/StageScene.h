#pragma once
#include "SceneBase.h"
#include "WallCollider.h"

//前方宣言
//こういうクラスがあるよ！
class Player;
class WeaponBase;
class Enemy;
class Wall;
class Texture2D;
class Grenade;
class LoadingUI;

//段階的に敵の出現数を多くするための構造体
struct EnemyPhase
{
	int startSec;        //このフェーズ開始秒
	int maxEnemies;      //同時最大数
	int intervalFrames;  //スポーン間隔
	int spawnPerTick;    //1回に出す数
};

//struct DroppedWeaponSlot
//{
//	WeaponBase* weapon = nullptr;
//	DirectX::SimpleMath::Vector3 pos = { 0,0,0 };
//	bool active = false;
//};

class StageScene : public SceneBase
{
private:
	const int End_Time = 120;//2分
	int Now_Time = 0;
	int Time_Count = 0;
	// ローディングUI
	LoadingUI* m_LoadUI;
	bool m_IsLoading = true;
	int  m_LoadStep = 0;
	bool m_LoadSeStarted = false;
	std::vector<std::function<void()>> m_LoadSteps;
	//UI
	std::vector<Texture2D*> m_pUITextures;
	//壁
	std::vector<Wall*> m_Walls;
	std::vector<WallCollider> m_WallCols;
	//敵
	std::vector<Enemy*> m_pEnemies;
	//プレイヤー
	Player* m_Player = nullptr;
	Grenade* m_Grenade = nullptr;
	//目の高さ
	const float EyeHeight = 17.0f;

	// 経過時間
	int m_Elapsed_Frames = 0; // 経過フレーム
	int m_Spawn_Timer = 0;

	// フェーズ管理
	std::vector<EnemyPhase> m_Phases;
	int m_Phase_Index = 0;

	//敵をスポーンさせる
	Enemy* SpawnEnemy(const DirectX::SimpleMath::Vector3& pos);
	//死んだ敵を片付ける
	void CleanupDeadEnemies();
	void UpdatePhaseByTime();

	bool m_BeginLoadingNextFrame = true;
	bool m_GameStarted = false;

	// ゲーム終了フラグ
	bool m_IsEnding = false;

	//UI
	Texture2D* m_TimeDigits[3] = {}; // [0]=百の位 [1]=十の位 [2]=一の位
	Texture2D* m_AR_Digits[2] = {};
	Texture2D* m_Pistol_Digits[2] = {};
	Texture2D* m_Knife_Digits[2] = {};
	Texture2D* m_HP_Digits[3] = {};
	Texture2D* m_Grenade_Digits[2] = {};
	Texture2D* m_Ult_Digits[2] = {};
	Texture2D* m_Blink_Digits[2] = {};

public:

	void StartGameplay();

	StageScene() {};
	~StageScene() {};

	void Update() override;
	void Init() override;
	void Uninit() override;

	//壁を追加するための関数
	Wall* AddWallBox(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& scale);
	//壁の当たり判定を返す
	const std::vector<WallCollider>* GetWallColliders() const { return &m_WallCols; }
};

