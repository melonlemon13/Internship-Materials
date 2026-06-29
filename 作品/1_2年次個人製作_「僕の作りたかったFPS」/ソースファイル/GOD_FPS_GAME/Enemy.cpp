#include "Enemy.h"
#include "Player.h"
#include "GameManager.h"
#include "WallCollider.h"

using namespace DirectX::SimpleMath;

static float ClampF(float v, float a, float b)
{
	return (v < a) ? a : (v > b) ? b : v;
}

Enemy::RenderAssets& Enemy::GetRenderAssets()
{
	static RenderAssets s_assets;

	if (!s_assets.loaded)
	{
		LoadAssets(s_assets);
		s_assets.loaded = true;
	}
	return s_assets;
}

void Enemy::PreloadAssets()
{
	(void)GetRenderAssets();
}

void Enemy::LoadAssets(RenderAssets& a)
{
	StaticMesh staticmesh;

	std::u8string modelFile = u8"assets/model/enemy/Enemy.obj";
	std::string texDirectory = "assets/model/enemy";

	std::string tmp(reinterpret_cast<const char*>(modelFile.c_str()), modelFile.size());
	staticmesh.Load(tmp, texDirectory);

	a.meshRenderer.Init(staticmesh);
	a.shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

	a.subsets = staticmesh.GetSubsets();
	a.textures = staticmesh.GetTextures();
	a.materialSrc = staticmesh.GetMaterials();
}

void Enemy::Init()
{
	//変数の初期化
	m_Hp = 100;
	m_Attack_Power = 10;
	m_Speed = 1.2f;
	m_DetectRange = 100.0f;
	m_AttackRange = 2.0f;
	m_Attack_Cooldown = 60;
	m_Attack_Timer = 0;

	m_DeadTimer = 30;

	m_Position = Vector3(0, 0, 10);
	m_Scale = Vector3(1, 1, 1);

	//モデルによってスケールを調整
	m_Scale = { 0.1f, 0.1f, 0.1f };

	//状態をIdleで初期化
	m_State = State::Idle;

	//徘徊方向(ランダム)
	float angle = (float)(rand() % 360) * DirectX::XM_PI / 180.0f;
	m_WanderDir = { sinf(angle), 0.0f, cosf(angle) };
	//2秒ごとに方向を変更
	m_WanderTimer = 120;

	m_HitDone = false;

	m_DamageFlashTimer = 0;

	auto& a = GetRenderAssets();

	m_Materials.clear();
	m_Materials.reserve(a.materialSrc.size());

	for (auto& src : a.materialSrc)
	{
		std::unique_ptr<Material> m = std::make_unique<Material>();
		m->Create(src);
		m_Materials.push_back(std::move(m));
	}
}

void Enemy::Update()
{
	m_Position.y = 0.0f;
	//HPチェック
	if (m_Hp <= 0)
	{
		m_State = State::Dead;
	}

	//死亡処理
	if (m_State == State::Dead)
	{
		if (m_DeadTimer > 0)
		{
			m_DeadTimer--;
			return; // 死亡中は何もしない
		}

		// ここで完全削除
		m_DeadTimer = -1;
		GameManager::GetInstance()->DeleteObject(this);
		return;
	}

	//人間は愚か
	//プレイヤーが死んでるorターゲットが設定されていなかったら何もしない
	//自身が死んでいたら何もしない
	//こんなんなんぼ書いてもええですからね
	if (!m_Target) return;
	if (m_Target->GetHp() <= 0) return;

	//Attackクールダウン減少
	if (m_Attack_Timer > 0) m_Attack_Timer--;

	//ダメージ点滅タイマー減少
	if (m_DamageFlashTimer > 0) --m_DamageFlashTimer;

	//プレイヤーとの距離
	Vector3 to = m_Target->GetPosition() - m_Position;
	to.y = 0.0f;

	//距離正規化
	float dist = to.Length();

	//状態遷移

	if (m_State != State::Attack)
	{
		m_HitDone = false;
	}

	if (m_State == State::Attack && dist > m_AttackRange)
	{
		m_State = State::Chase;
	}

	if (dist <= m_AttackRange)
	{
		m_State = State::Attack;
	}
	else if (dist <= m_DetectRange)
	{
		m_State = State::Chase;
	}
	else
	{
		m_State = State::Idle;
	}

	//追いかけているor攻撃の時はプレイヤーの方を向く
	if (m_State == State::Chase || m_State == State::Attack)
	{
		LookAtPlayer();
	}

	if (m_IsHitStop)
	{
		if (m_HitStopTimer > 0)
		{
			m_HitStopTimer--;
			return; // このフレームは何もさせない
		}
		else
		{
			m_IsHitStop = false;
		}
	}

	move();
	Attack();
}

void Enemy::Draw(Camera* cam)
{
	if (m_State == State::Dead) return;

	auto& a = GetRenderAssets();
	if (!a.loaded) return;

	//SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	a.shader.SetGPU();
	a.meshRenderer.BeforeDraw();

	for (int i = 0; i < (int)a.subsets.size(); ++i)
	{
		int matIdx = a.subsets[i].MaterialIdx;
		if (matIdx < 0 || matIdx >= (int)m_Materials.size()) continue;

		Material* mat = m_Materials[matIdx].get();
		bool flashing = (m_DamageFlashTimer > 0);

		DirectX::XMFLOAT4 baseD{}, baseE{};

		if (flashing)
		{
			baseD = mat->GetDiffuse();
			baseE = mat->GetEmission();
			mat->SetDiffuse({ 1,0.3f,0.3f,1 });
			mat->SetEmission({ 0.2f,0,0,1 });
			mat->Update();
		}

		mat->SetGPU();

		if (mat->isTextureEnable())
		{
			if (matIdx < (int)a.textures.size() && a.textures[matIdx])
				a.textures[matIdx]->SetGPU();
		}

		a.meshRenderer.DrawSubset(
			a.subsets[i].IndexNum,
			a.subsets[i].IndexBase,
			a.subsets[i].VertexBase
		);

		if (flashing)
		{
			mat->SetDiffuse(baseD);
			mat->SetEmission(baseE);
			mat->Update();
		}
	}
}

void Enemy::Uninit()
{

}

void Enemy::move()
{
	if (m_State == State::Attack || m_State == State::Dead)
		return;
	//Stateによって処理を変える
	switch (m_State)
	{
		//深夜徘徊おじさん
	case State::Idle:
	{
		//一定時間で方向変更
		if (m_WanderTimer > 0)
		{
			m_WanderTimer--;
		}
		else
		{
			float angle = (float)(rand() % 360) * DirectX::XM_PI / 180.0f;
			m_WanderDir = { sinf(angle), 0.0f, cosf(angle) };
			m_WanderTimer = 120;
		}

		m_Position += m_WanderDir * m_Speed;
		ResolveWallCollision();
		break;
	}

	//プレイヤー追いかける
	case State::Chase:
	{
		Vector3 to = m_Target->GetPosition() - m_Position;
		to.y = 0.0f;

		if (to.LengthSquared() > 0.0001f)
		{
			to.Normalize();
			m_Position += to * m_Speed;
		}
		ResolveWallCollision();
		break;
	}

	case State::Attack:
	case State::Dead:
		//移動しない
		break;
	}
}

void Enemy::Attack()
{
	if (m_State != State::Attack)
		return;

	if (m_Attack_Timer > 0)
		return;

	// クールダウンリセット
	m_Attack_Timer = m_Attack_Cooldown;

	//TODO: プレイヤーにダメージ
	m_Target->ApplyDamage(m_Attack_Power);
}

void Enemy::LookAtPlayer()
{
	if (!m_Target) return;

	//プレイヤーへの方向
	Vector3 to = m_Target->GetPosition() - m_Position;
	to.y = 0.0f;

	if (to.LengthSquared() < 0.0001f)
		return;

	to.Normalize();

	m_Rotation.y = atan2f(to.x, to.z);
}

void Enemy::TakeDamage(int dmg)
{
	TakeDamage(dmg, KillType::Unknown);
	//m_Hp -= dmg;

	////ヒットストップ開始
	//m_IsHitStop = true;
	//m_HitStopTimer = 5;

	//if (m_Hp <= 0)
	//{
	//	m_Hp = 0;
	//	m_State = State::Dead;
	//	if (m_Player)
	//	{
	//		//m_Player->OnKillEnemy(1);
	//	}

	//	GameManager::GetSound()->Play(SOUND_LABEL_SE_ENEMY_DEAD);
	//	return;
	//}
	////ダメージ点滅タイマーリセット
	//m_DamageFlashTimer = kFlashDuration;
	//GameManager::GetSound()->Play(SOUND_LABEL_SE_ENEMY_HIT);
}

void Enemy::TakeDamage(int dmg, KillType type)
{
	// すでに死んでいるなら無視
	if (m_State == State::Dead) return;

	// 最後に受けた攻撃種別を更新
	m_LastHitType = type;

	m_Hp -= dmg;

	// ヒットストップ開始
	m_IsHitStop = true;
	m_HitStopTimer = 5;

	if (m_Hp <= 0)
	{
		m_Hp = 0;
		m_State = State::Dead;

		// キル計上
		if (!m_KillCounted)
		{
			m_KillCounted = true;
			if (m_Player)
			{
				m_Player->OnKillEnemy(m_LastHitType);
			}
		}

		GameManager::GetSound()->Play(SOUND_LABEL_SE_ENEMY_DEAD);
		return;
	}

	// ダメージ点滅タイマーリセット
	m_DamageFlashTimer = kFlashDuration;
	GameManager::GetSound()->Play(SOUND_LABEL_SE_ENEMY_HIT);
}


void Enemy::ResolveWallCollision()
{
	if (!m_WallCols) return;

	for (const auto& w : *m_WallCols)
	{
		float minX = w.center.x - w.half.x;
		float maxX = w.center.x + w.half.x;
		float minZ = w.center.z - w.half.z;
		float maxZ = w.center.z + w.half.z;

		float cx = ClampF(m_Position.x, minX, maxX);
		float cz = ClampF(m_Position.z, minZ, maxZ);

		float dx = m_Position.x - cx;
		float dz = m_Position.z - cz;

		float d2 = dx * dx + dz * dz;
		float r2 = m_CollisionRadius * m_CollisionRadius;

		if (d2 < r2)
		{
			// 壁の中にいる（d2==0）の場合は軸方向に脱出
			if (d2 < 0.000001f)
			{
				float left = fabsf(m_Position.x - minX);
				float right = fabsf(maxX - m_Position.x);
				float back = fabsf(m_Position.z - minZ);
				float front = fabsf(maxZ - m_Position.z);

				float m = (std::min)((std::min)(left, right), (std::min)(back, front));
				if (m == left)       m_Position.x = minX - m_CollisionRadius;
				else if (m == right) m_Position.x = maxX + m_CollisionRadius;
				else if (m == back)  m_Position.z = minZ - m_CollisionRadius;
				else                 m_Position.z = maxZ + m_CollisionRadius;

				continue;
			}

			float d = sqrtf(d2);
			float nx = dx / d;
			float nz = dz / d;

			float push = m_CollisionRadius - d;
			m_Position.x += nx * push;
			m_Position.z += nz * push;
		}
	}
}