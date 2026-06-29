#pragma once
#include"ObjectBase.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "StaticMesh.h"
#include "Material.h"

class Player;
class Enemy;
class GameManager;

class WeaponBase :public ObjectBase
{
protected:
	//持ち主
	Player* m_Owner = nullptr;
	//攻撃対象
	std::vector<Enemy*>* m_Targets = nullptr;
	//ダメージ
	int m_Attack_Power;
	//最大装填数
	int m_MagCapacity;
	//現在マガジンに入っている弾薬の数
	int m_MagAmmo;
	//発射レート
	int m_FireCooldown = 0;
	//リロード時間
	int m_Reload_Time;
	//タイマーが0(リロードが終わっている)なら攻撃可能
	int m_Timer;
	//射程
	float m_Range;
	//拡散
	float m_SpreadDeg;
	float m_HalfAngleDeg;
	//距離減衰

	float m_FalloffStart;
	float m_FalloffMinMul;

	//描画や更新のフラグ
	bool m_Active = false;

	//表示・非表示のフラグ
	bool m_Visible = true;

	// 描画の為の情報（メッシュに関わる情報）
	MeshRenderer m_MeshRenderer; // 頂点バッファ・インデックスバッファ・インデックス数

	// スキルでリロードブロックされているか
	bool IsReloadBlockedBySkill() const;

	DirectX::SimpleMath::Vector3 m_RecoilOffset = { 0,0,0 };
	float m_RecoilReturn = 0.80f;   // 戻りの速さ（0.7～0.9で調整）
	float m_RecoilKickBack = 0.35f; // 後ろに下がる量
	float m_RecoilKickUp = 0.08f;   // 上に上がる量

public:
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw(Camera* cam) = 0;
	virtual void Uninit() = 0;

	virtual void Attack() = 0;

	//表示・非表示の設定
	void SetVisible(bool v) { m_Visible = v; }
	bool IsVisible() const { return m_Visible; }

	//タイマー更新
	void UpdateTimers()
	{
		if (m_Timer > 0) m_Timer--;
		//if (m_FireCooldown > 0) m_FireCooldown--;
	}

	//カメラの手の位置に追従させる
	void FollowCameraHandPose(float forwardDist, float rightDist, float downDist);

	//リロードアニメーション用関数
	float EaseOut(float t)
	{
		// Clamp
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;

		// EaseOutQuad
		return 1.0f - (1.0f - t) * (1.0f - t);
	}

	void SetActive(bool a) { m_Active = a; }
	bool GetActive() const { return m_Active; }

	//持ち主をセット
	void SetOwner(Player* owner) { m_Owner = owner; }
	//持ち主を取得
	Player* GetOwner() const { return m_Owner; }
	//StageSceneから敵リスト取得し、セット
	void SetTargets(std::vector<Enemy*>* targets) { m_Targets = targets; }

	//リロードフレーム数を取得
	int GetReloadFrames() { return m_Timer; }

	//オーナーのXZ平面上の前方ベクトルを取得
	DirectX::SimpleMath::Vector3 GetOwnerForwardXZ() const;

	int GetMagAmmo() const{ return m_MagAmmo; }
};

