#pragma once
#include"ObjectBase.h"

#include <vector>
#include <memory>

#include "MeshRenderer.h"
#include "Material.h"
#include "Texture.h"
#include "StaticMesh.h"
#include "KillType.h"

struct WallCollider;
class Player;

class Enemy : public ObjectBase
{
public:
	//Idle:徘徊,Chase:追いかける,Attack:攻撃,Dead:死亡
	enum class State { Idle, Chase, Attack, Dead };
private:
	struct RenderAssets
	{
		bool loaded = false;

		MeshRenderer meshRenderer;
		Shader shader;

		std::vector<SUBSET> subsets;
		std::vector<std::unique_ptr<Texture>> textures;

		// Materialの元データだけ持つ
		std::vector<MATERIAL> materialSrc;
	};

	static RenderAssets& GetRenderAssets();
	static void LoadAssets(RenderAssets& a);

private:

	const std::vector<WallCollider>* m_WallCols = nullptr;
	float m_CollisionRadius = 5.0f;
	Player* m_Target = nullptr;
	State m_State = State::Idle;

	int m_Hp = 100;
	int m_Attack_Power = 10;
	float m_Speed = 1.2f;
	//プレイヤーを見つける範囲、この範囲内にプレイヤーがいると追いかける
	float m_DetectRange = 100.0f;
	//攻撃範囲、この範囲内プレイヤーがいると攻撃する
	float m_AttackRange = 2.0f;
	int m_Attack_Cooldown = 60;
	int m_Attack_Timer = 0;

	//攻撃判定のフラグ
	bool m_HitDone = false;

	//徘徊(Idle)用
	DirectX::SimpleMath::Vector3 m_WanderDir = { 1.0f, 0.0f, 0.0f };
	int m_WanderTimer = 0;         // 方向変更タイマー（フレーム）

	//死亡時に使うタイマー
	int m_DeadTimer = 30;

	// 描画の為の情報（メッシュに関わる情報）
	MeshRenderer m_MeshRenderer; // 頂点バッファ・インデックスバッファ・インデックス数

	// 描画の為の情報（見た目に関わる部分）
	std::vector<std::unique_ptr<Material>> m_Materials;

	//ダメージを受けたときの点滅用タイマー
	int m_DamageFlashTimer = 0;
	static constexpr int kFlashDuration = 10;

	Player* m_Player = nullptr;

	int  m_HitStopTimer = 0;   // 残りフレーム
	bool m_IsHitStop = false; // ヒットストップ中か

	// 最後に受けたキルタイプ
	KillType m_LastHitType = KillType::Unknown;

	// キルがカウント済みかどうか
	bool m_KillCounted = false;

public:
	static void PreloadAssets();

	Enemy() {};
	~Enemy() {};

	void Init() override;
	void Update()  override;
	void Draw(Camera* cam)  override;
	void Uninit()  override;

	//移動
	void move();
	//攻撃
	void Attack();
	//プレイヤーの方を向く
	void LookAtPlayer();

	void SetWallColliders(const std::vector<WallCollider>* cols) { m_WallCols = cols; }
	void ResolveWallCollision();

	void SetTarget(Player* p) { m_Target = p; }
	void TakeDamage(int dmg);
	void TakeDamage(int dmg, KillType type);
	void SetPlayer(Player* p) { m_Player = p; }

	State GetState() const { return m_State; };
};

