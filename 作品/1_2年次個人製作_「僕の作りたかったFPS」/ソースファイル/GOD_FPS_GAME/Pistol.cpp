#include "Pistol.h"

#include "Player.h"
#include "Enemy.h"
#include "GameManager.h"
#include "Renderer.h"
#include "input.h"
#include <cmath>
#include <algorithm>
using namespace DirectX::SimpleMath;


Pistol::RenderAssets& Pistol::GetRenderAssets()
{
	static RenderAssets s_assets;
	if (!s_assets.loaded)
	{
		LoadAssets(s_assets);
		s_assets.loaded = true;
	}
	return s_assets;
}

void Pistol::PreloadAssets()
{
	// 呼ぶだけでロード済みにできる
	(void)GetRenderAssets();
}

void Pistol::LoadAssets(RenderAssets& a)
{
	// メッシュ読み込み
	StaticMesh staticmesh;

	std::u8string modelFile = u8"assets/model/Gun/Gun2.obj";
	std::string texDirectory = "assets/model/Gun";

	std::string tmpStr1(reinterpret_cast<const char*>(modelFile.c_str()), modelFile.size());
	staticmesh.Load(tmpStr1, texDirectory);

	// VB/IBなど（StaticMeshから作る）
	a.meshRenderer.Init(staticmesh);

	// シェーダ
	a.shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

	// サブセット / テクスチャ / マテリアル
	a.subsets = staticmesh.GetSubsets();

	// ここは move なので「1回だけ」呼ぶ
	a.textures = staticmesh.GetTextures();

	std::vector<MATERIAL> materials = staticmesh.GetMaterials();
	a.materials.clear();
	a.materials.reserve(materials.size());

	for (int i = 0; i < (int)materials.size(); ++i)
	{
		std::unique_ptr<Material> m = std::make_unique<Material>();
		m->Create(materials[i]);
		a.materials.push_back(std::move(m));
	}
}

void Pistol::Init()
{
	(void)GetRenderAssets();

	// 見た目スケール（モデルに合わせて調整）
	m_Scale = { 1.0f, 1.0f, 1.0f };

	m_Active = false;

	//武器ステータス（あとで調整OK）
	m_Attack_Power = 80;

	//マガジン
	m_MagCapacity = 12;
	m_MagAmmo = m_MagCapacity;

	//リロード
	m_Reload_Time = 120;
	m_IsReloading = false;

	// 連射間隔：1発/0.8秒 → 48f
	m_FireCooldown = 48;
	m_Timer = 0;

	// 射程など
	m_Range = 600.0f;      // 600/1200
	m_SpreadDeg = 0.0f;     // リボルバーはほぼ無し
	m_HalfAngleDeg = 2.0f;  // かなり精密

}

void Pistol::Update()
{
	if (!m_Owner) return;
	//装備中でなければ処理をスキップ
	if (!GetActive()) return;

	// 表示されていなければ処理をスキップ
	if (!IsVisible())
	{
		UpdateTimers();
		return;
	}

	UpdateTimers();

	FollowCameraHandPose(m_ForwardDist, m_RightDist, m_DownDist);

	// スキル中はリロード進行を止める
	if (m_IsReloading && IsReloadBlockedBySkill())
	{
		return;
	}

	// リロードモーション
	if (m_IsReloading)
	{
		// 進行度 0～1
		float p = 1.0f - (float)m_Timer / (float)m_Reload_Time;

		// EaseOutをかける
		float ease = EaseOut(p);

		// 下から生えてくる距離（cm）
		const float popDist = 25.0f;

		// カメラの上方向
		Camera* cam = GameManager::GetInstance()->GetCamera();
		Vector3 up = cam->GetUp();

		// 下 → 通常位置 へ
		// p=0  : -up * popDist
		// p=1  :  0
		Vector3 offset = -up * popDist * (1.0f - ease);

		// 表示位置に加算
		m_Position += offset;
	}

	// リロード
	bool nowR = Input::GetKeyPress(VK_R);

	if (nowR && !m_PrevR)
	{
		if (IsReloadBlockedBySkill())
		{
			// 何もしない
		}
		else
		{
			// 弾が満タンでない ＆ リロード中でない
			if (!m_IsReloading && m_MagAmmo < m_MagCapacity)
			{
				m_IsReloading = true;
				m_Timer = m_Reload_Time;
				GameManager::GetSound()->Play(SOUND_LABEL_VOICE_RELOAD);
				GameManager::GetSound()->Play(SOUND_LABEL_SE_PISTOL_RELOAD);
			}
		}
	}
	m_PrevR = nowR;

	// リロード完了
	if (m_IsReloading && m_Timer == 0)
	{
		m_MagAmmo = m_MagCapacity;
		m_IsReloading = false;
	}

	if (m_DidFireThisFrame)
	{
		Camera* cam = GameManager::GetInstance()->GetCamera();
		Vector3 f = cam->GetForward();
		Vector3 u = cam->GetUp();

		// 後ろ + 上 にキック（ピストルは少し強めでもOK）
		m_RecoilOffset += (-f * m_RecoilKickBack) + (u * m_RecoilKickUp);
	}

	m_DidFireThisFrame = false;
}

void Pistol::Draw(Camera* cam)
{
	//装備中でなければ処理をスキップ
	if (!GetActive()) return;

	// 表示されていなければ処理をスキップ
	if (!IsVisible()) return;

	auto& a = GetRenderAssets();
	if (!a.loaded) return;

	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	Matrix worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx);

	a.shader.SetGPU();
	a.meshRenderer.BeforeDraw();

	for (int i = 0; i < (int)a.subsets.size(); ++i)
	{
		const int midx = (int)a.subsets[i].MaterialIdx;

		if (midx >= 0 && midx < (int)a.materials.size() && a.materials[midx])
		{
			a.materials[midx]->SetGPU();

			// TextureEnable が true で、対応textureがあればGPUへ
			if (a.materials[midx]->isTextureEnable())
			{
				if (midx >= 0 && midx < (int)a.textures.size() && a.textures[midx])
				{
					a.textures[midx]->SetGPU();
				}
			}
		}
		a.meshRenderer.DrawSubset(
			a.subsets[i].IndexNum,
			a.subsets[i].IndexBase,
			a.subsets[i].VertexBase);
	}
}

void Pistol::Uninit()
{

}

void Pistol::Attack()
{
	// 装備中でなければ何もしない
	if (!GetActive()) return;
	if (!m_Owner) return;
	if (!m_Targets) return;

	// クールダウン or リロード中
	if (m_Timer > 0) return;

	// 弾切れ → リロード開始
	if (m_MagAmmo <= 0)
	{
		m_IsReloading = true;
		m_Timer = m_Reload_Time;
		GameManager::GetSound()->Play(SOUND_LABEL_VOICE_RELOAD);
		GameManager::GetSound()->Play(SOUND_LABEL_SE_PISTOL_RELOAD);
		return;
	}

	// 発射
	m_MagAmmo--;
	m_Timer = m_FireCooldown;
	m_DidFireThisFrame = true;

	GameManager::GetSound()->Play(SOUND_LABEL_SE_PISTOL_FIRING);

	// 発射原点（プレイヤー位置）
	Vector3 origin = m_Owner->GetPosition();

	// 狙い方向（XZ）
	Vector3 forward = GetOwnerForwardXZ();

	// 扇形判定用（半角）
	float cosLimit = cosf(DirectX::XMConvertToRadians(m_HalfAngleDeg));

	Enemy* bestEnemy = nullptr;
	float bestDot = -1.0f;
	float bestDist2 = 0.0f;

	for (Enemy* e : *m_Targets)
	{
		if (!e) continue;
		if (e->GetState() == Enemy::State::Dead) continue;

		Vector3 toEnemy = e->GetPosition() - origin;
		toEnemy.y = 0.0f; // XZのみ

		float dist2 = toEnemy.LengthSquared();

		// 射程外
		if (dist2 > m_Range * m_Range) continue;

		if (dist2 < 0.0001f) continue;

		Vector3 dir = toEnemy;
		dir.Normalize();

		float dot = forward.Dot(dir);

		// 扇形外
		if (dot < cosLimit) continue;

		// より正面の敵を優先（同じなら近い方）
		if (dot > bestDot || (fabsf(dot - bestDot) < 0.0001f && dist2 < bestDist2))
		{
			bestEnemy = e;
			bestDot = dot;
			bestDist2 = dist2;
		}
	}

	if (!bestEnemy) return;

	//距離減衰
	float dist = sqrtf(bestDist2);

	const float falloffStart = 300.0f;   // 3m
	const float minMul = 0.5f;           // 12mで50%

	float damageMul = 1.0f;

	if (dist <= falloffStart)
	{
		damageMul = 1.0f;
	}
	else if (dist >= m_Range)
	{
		damageMul = minMul;
	}
	else
	{
		float t = (dist - falloffStart) / (m_Range - falloffStart); // 0..1
		damageMul = 1.0f + (minMul - 1.0f) * t;
	}

	int finalDamage = (int)std::lroundf((float)m_Attack_Power * damageMul);
	if (finalDamage < 1) finalDamage = 1;

	bestEnemy->TakeDamage(finalDamage, KillType::Pistol);
}