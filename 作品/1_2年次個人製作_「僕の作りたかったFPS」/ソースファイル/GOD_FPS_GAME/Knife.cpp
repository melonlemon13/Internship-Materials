#include "Knife.h"

#include"Player.h"
#include"Enemy.h"
#include "GameManager.h"

using namespace DirectX::SimpleMath;

Knife::RenderAssets& Knife::GetRenderAssets()
{
	static RenderAssets s_assets;

	if (!s_assets.loaded)
	{
		LoadAssets(s_assets);
		s_assets.loaded = true;
	}

	return s_assets;
}

void Knife::PreloadAssets()
{
	(void)GetRenderAssets();
}

void Knife::LoadAssets(RenderAssets& a)
{
	StaticMesh staticmesh;

	std::u8string modelFile = u8"assets/model/Knife/Knife2.obj";
	std::string texDirectory = "assets/model/Knife";

	std::string tmp(reinterpret_cast<const char*>(modelFile.c_str()), modelFile.size());
	staticmesh.Load(tmp, texDirectory);

	// VB/IB 作成
	a.meshRenderer.Init(staticmesh);

	// シェーダ
	a.shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

	// サブセット / テクスチャ / マテリアル
	a.subsets = staticmesh.GetSubsets();
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
void Knife::Init()
{
	m_Active = false;

	(void)GetRenderAssets();

	//モデルによってスケールを調整
	m_Scale = { 0.2f, 0.2f, 0.2f };

	//変数値設定
	//ワンパン
	m_Attack_Power = 100;
	//絶対になくならない
	m_MagCapacity = 2147483647;
	//一度の射撃で撃てる量
	//ナイフは一回攻撃したらクールダウンに入って欲しいので
	m_MagAmmo = 1;
	//攻撃間隔,1秒
	m_FireCooldown = 120;
	
	m_Reload_Time = 60;
	m_IsReloading = false;
}

void Knife::Update()
{
	//装備中でなければ処理をスキップ
	if (!GetActive()) return;

	// 表示されていなければ処理をスキップ
	if (!IsVisible())
	{
		UpdateTimers();
		return;
	}

	UpdateTimers();

	if (!m_Owner) return;

	Camera* cam = GameManager::GetInstance()->GetCamera();

	FollowCameraHandPose(forwardDist, rightDist, downDist);

	if(m_Timer == 0)
	{
		m_IsReloading = false;
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
}

void Knife::Draw(Camera* cam)
{
	//装備中でなければ処理をスキップ
	if (!GetActive()) return;

	// 表示されていなければ処理をスキップ
	if (!IsVisible()) return;

	auto& a = GetRenderAssets();
	if (!a.loaded) return;

	// SRT情報作成
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
		const int idx = a.subsets[i].MaterialIdx;
		if (idx < 0 || idx >= (int)a.materials.size()) continue;

		a.materials[idx]->SetGPU();

		if (a.materials[idx]->isTextureEnable())
		{
			if (idx < (int)a.textures.size() && a.textures[idx])
			{
				a.textures[idx]->SetGPU();
			}
		}

		a.meshRenderer.DrawSubset(
			a.subsets[i].IndexNum,
			a.subsets[i].IndexBase,
			a.subsets[i].VertexBase);
	}
}

void Knife::Uninit()
{

}


void Knife::Attack()
{
	//装備中でなければ処理をスキップ
	if (!GetActive()) return;
	//Ownerが未定義なら処理をスキップ
	if (!m_Owner) return;
	//ターゲットが未設定なら処理をスキップ
	if (!m_Targets) return;
	//クールダウン中なら処理をスキップ
	if (m_Timer > 0) return;

	//攻撃が開始したらクールダウンをセット
	m_Timer = m_FireCooldown;

	m_IsReloading = true;

	GameManager::GetSound()->Play(SOUND_LABEL_SE_KNIFE);

	//プレイヤーの位置を取得
	Vector3 playerpos = m_Owner->GetPosition();
	//プレイヤーの向きを取得
	Vector3 playerrot = m_Owner->GetRotation();

	//プレイヤーの正面方向
	Vector3 forward = GetOwnerForwardXZ();

	//距離判定用
	float distance2 = m_Distance * m_Distance;

	//角度判定用
    // dot >= cos(半角)なら扇形の中
	//DirectX::XMConvertToRadians 度 → ラジアン変換
	float coslimit = cosf(DirectX::XMConvertToRadians(m_Range));

	for (Enemy* e : *m_Targets)
	{
		if (!e) continue;
		if (e->GetState() == Enemy::State::Dead) continue;

		Vector3 toEnemy = e->GetPosition() - playerpos;

		float d2 = toEnemy.LengthSquared();

		//Yは使わないので無視
		toEnemy.y = 0.0f;

		//距離外な次の敵へ
		if (toEnemy.LengthSquared() > distance2) continue;

		//角度判定のため正規化
		toEnemy.Normalize();

		//forwardとtoEnemy の内積
		float dot = forward.Dot(toEnemy);

		//扇形の外なら次の敵へ
		if (dot < coslimit) continue;

		//当たったらダメージ
		e->TakeDamage(m_Attack_Power, KillType::Knife);

		////1体に当てたら終了
		//break;
	}
}

