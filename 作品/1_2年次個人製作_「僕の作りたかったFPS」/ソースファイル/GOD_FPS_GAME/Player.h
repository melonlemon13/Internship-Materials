#pragma once
#include"ObjectBase.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include"BlinkSkill.h"
#include"GrenadeSkill.h"
#include "UltimateSkill.h"
#include "UltSword.h"
#include "KillType.h"

class WeaponBase;
class Enemy;
struct WallCollider;

class Player : public ObjectBase
{
private:
	// 壁当たり判定情報へのポインタ
	const std::vector<WallCollider>* m_WallCols = nullptr;
	// 当たり判定用半径
	float m_CollisionRadius = 20.0f;

	// 敵リスト参照
	std::vector<Enemy*>* m_Enemies = nullptr;
	int m_EnemyNearSE_Cooldown = 0;

	// 頂点データ
	std::vector<VERTEX_3D> m_Vertices;

	//インデックスデータ
	std::vector<unsigned int> m_Indices;

	// 描画の為の情報（メッシュに関わる情報）
	IndexBuffer	 m_IndexBuffer; // インデックスバッファ
	VertexBuffer<VERTEX_3D>	m_VertexBuffer; // 頂点バッファ
	// 描画の為の情報（見た目に関わる部分）
	Shader m_Shader; // シェーダー
	Texture m_Texture;//テクスチャ

	//ダメージ間隔タイマー
	float m_Damage_Interval = 60.0f;
	float m_Now_Damage_Interval = 0.0f;

	//体力
	int m_Hp = 100;

	//移動速度,ダメージなどで変更予定なのでconstは付けない
	//スピードに重みを付ける W>A=S=D
	//今のところA,S,Dそれぞれに変数を用意しているけど変更しない||同時に変更するなら統一した方がいい
	float m_Speed_W = 2.0f;
	float m_Speed_A = 1.8f;
	float m_Speed_S = 1.8f;
	float m_Speed_D = 1.8f;

	//視点移動用変数,設定で変更できるようにするためにconstは付けない
	//マウス感度,!sens
	float m_Sens = 0.0007f;
	//感度の最小値,最大値,変更幅
	const float m_MinSens = 0.0001f;
	const float m_MaxSens = 0.0030f;
	const float m_SensStep = 0.00005f;
	//視点の上下入力限界,変更しないのでconst
	//XM_PIDIV2はDX側の値で意味としてはπ/2,つまり90度
	//首が折れないように90度より少しだけ小さい値を上限に設定
	//constexpr コンパイル時定数,constの上位版みたいな感じ
	static constexpr float m_PITCH_LIMIT = DirectX::XM_PIDIV2 - 0.1f;
	//プレイヤーの頭の位置
	//const float m_EyeHeight = 1.7f;
	//速度
	//ジャンプと落下に使う
	DirectX::SimpleMath::Vector3 m_Velocity = { 0,0,0 };
	//ジャンプできるか(地面と接しているか)
	bool m_On_Ground = true;

	//ジャンプ初速
	float m_Jump_Speed = 20.0f;
	//重力
	float m_Gravity = -80.0f;
	//地面の座標
	float m_GroundY = 0.0f;

	//SE用フラグ
	bool low_voice_flg = true;

	//どの武器を持っているか
	int m_Now_Weapon;
	int m_Old_Weapon;
	//武器を3つまで持てる
	static const int WEAPON_MAX = 3;
	WeaponBase* m_pWeapon[WEAPON_MAX] = { nullptr, nullptr, nullptr };
	// ナイフ固定
	static const int m_PISTROL_SLOT = 0;
	static const int m_ASSAULT_SLOT = 1;
	static const int m_KNIFE_SLOT = 2;

	//スキル用
	int m_Blink_Timer = 0;
	DirectX::SimpleMath::Vector3 m_BlinkDir = { 0,0,0 };
	// スペースキー離すまでブリンク禁止フラグ
	bool m_BlockBlinkUntilSpaceRelease = false;
	// 空中ブリンク（追加移動）の状態
	bool  m_AirBlinkActive = false;
	int   m_AirBlinkFrame = 0;
	int   m_AirBlinkTotalFrames = 0;

	float m_AirBlinkTotalDist = 0.0f;   // 合計距離（cm）
	float m_AirBlinkPrevEase = 0.0f;    // 前フレームのEase進捗（0〜1）

	//空中ブリンク移動更新
	void UpdateBlinkMove();
	//イージング関数(EaseOut)
	float EaseOut(float t) const;

	//1フレームあたりの移動量,疑似速度計算用
	DirectX::SimpleMath::Vector3 m_PrevPosition = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 m_MoveVelocity = DirectX::SimpleMath::Vector3::Zero;
	//スキル
	BlinkSkill m_Blink_Skill;
	GrenadeSkill m_Grenade_Skill;
	// ULT
	UltimateSkill m_Ult_Skill;
	UltSword* m_UltSword = nullptr;

	// ULT開始前に装備してた武器に戻すため
	int m_PreUltWeapon = 0;
	bool m_WasUltActivePrev = false;

	//キル数統計
	struct KillStats
	{
		int total = 0;
		int pistol = 0;
		int ar = 0;
		int knife = 0;
		int grenade = 0;
		int ult = 0;
	};

	KillStats m_KillStats{};

private:
	//スキルによるブロック状態
	SkillBase::Blocks m_SkillBlocks{};

public:
	//武器変更がブロックされているか
	bool IsWeaponSwitchBlocked() const;
	//リロードがブロックされているか
	bool IsReloadBlocked() const;
	//他のスキルがブロックされているか
	bool IsOtherSkillsBlocked() const;

	// スキルによるブロック状態を毎フレ更新する関数
	void UpdateSkillBlocks();
	//スキルのブロック状態を武器に適用する関数
	void ApplySkillBlocksToWeapons();


public:
	Player() {};
	~Player() {};

	void Init() override;
	void Update()  override;
	void Draw(Camera* cam)  override;
	void Uninit()  override;

	//視点移動
	void UpdateLook();
	//移動,WASD,SPACE,SHIFT
	void move();
	//攻撃
	void Attack();
	//ダメージを受ける
	void ApplyDamage(int dmg);

	//スキル用
	void SkillUpdate();

	void StartBlink(const DirectX::SimpleMath::Vector3& dirXZ, float totalDistCm, int frames);

	// プレイヤーが地面に接しているかどうかを返す
	bool IsOnGround() const { return m_On_Ground; }

	//壁の当たり判定を設定
	void SetWallColliders(const std::vector<WallCollider>* cols) { m_WallCols = cols; }
	//壁との当たり判定を解決
	void ResolveWallCollision();

	// 1フレームあたりの移動量,疑似速度を返す
	DirectX::SimpleMath::Vector3 GetMoveVelocity() const { return m_MoveVelocity; }

	//武器の初期設定
	void SetupWeapons(std::vector<Enemy*>* targets);

	//敵接近SE更新
	void UpdateEnemyNearSE();

	//敵リストの設定
	void SetEnemies(std::vector<Enemy*>* enemies) { m_Enemies = enemies; }

	// Enemy死亡通知用
	void OnKillEnemy(int kills = 1);

	//武器ホットキー処理
	void HandleWeaponHotkeys();
	//武器変更
	void ChangeWeapon(int newIndex);

	int GetHp() const
	{
		return m_Hp;
	}

	//向きを取得
	DirectX::SimpleMath::Vector3 GetForward() const;
	DirectX::SimpleMath::Vector3 GetRight() const;

	//速度を取得・設定
	DirectX::SimpleMath::Vector3 GetVelocity() const
	{
		return m_Velocity;
	}

	void SetVelocity(const DirectX::SimpleMath::Vector3& v) { m_Velocity = v; }

	// 壁当たり判定情報へのポインタを取得
	const std::vector<WallCollider>* GetWallCols() const { return m_WallCols; }
	// カメラ基準のForward（XZ）を返す
	DirectX::SimpleMath::Vector3 GetForwardXZ() const;

	// カメラ基準のRight（XZ）を返す
	DirectX::SimpleMath::Vector3 GetRightXZ() const;

	// WASD入力をカメラ基準XZ方向ベクトルで返す
	DirectX::SimpleMath::Vector3 GetMoveInputDirXZ() const;

	//現在の武器を取得
	WeaponBase* GetWeapon(int slot) const
	{
		if (slot < 0 || slot >= 3) return nullptr;
		return m_pWeapon[slot];
	}
	WeaponBase* GetCurrentWeapon() const
	{
		return GetWeapon(m_Now_Weapon);
	}

	//ブリンクスキル取得
	BlinkSkill& GetBlinkSkill() { return m_Blink_Skill; }
	//グレネードスキル取得
	GrenadeSkill& GetGrenadeSkill() { return m_Grenade_Skill; }
	//ウルト取得
	UltimateSkill& GetUltimateSkill() { return m_Ult_Skill; }

	// 武器別キル通知
	void OnKillEnemy(KillType type);

	// 結果画面用に参照できるように
	const KillStats& GetKillStats() const { return m_KillStats; }
	void ResetKillStats() { m_KillStats = {}; }

	int GetHP() const {return m_Hp;}
};

