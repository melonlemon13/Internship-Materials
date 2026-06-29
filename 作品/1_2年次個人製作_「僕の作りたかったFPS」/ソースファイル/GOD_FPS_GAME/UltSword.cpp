#include "UltSword.h"

#include <iostream>
#include <cmath>
#include"Enemy.h"
#include"Player.h"
#include "GameManager.h"

using namespace DirectX::SimpleMath;

UltSword::RenderAssets& UltSword::GetRenderAssets()
{
    static RenderAssets s_assets;

    if (!s_assets.loaded)
    {
        LoadAssets(s_assets);
        s_assets.loaded = true;
    }

    return s_assets;
}

void UltSword::PreloadAssets()
{
    (void)GetRenderAssets();
}

void UltSword::LoadAssets(RenderAssets& a)
{
    StaticMesh staticmesh;

    std::u8string modelFile = u8"assets/model/Sword/Sword.obj";
    std::string texDirectory = "assets/model/Sword";

    std::string tmp(reinterpret_cast<const char*>(modelFile.c_str()), modelFile.size());
    staticmesh.Load(tmp, texDirectory);

    a.meshRenderer.Init(staticmesh);

    a.shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

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

void UltSword::ApplyParams(const Params& p)
{
    m_Params = p;

    // 武器性能として反映
    m_Attack_Power = p.damage;

    // 斬撃間隔（TimerはUltSword専用で持つ）
    m_SlashTimer = 0;

    // 動いていい
    m_Ready = true;

    // 見えるのはULT中だけ（Player側でULT中に呼ばれる）
    SetVisible(true);
}

void UltSword::Init()
{
    (void)GetRenderAssets();

    // 見た目スケール（モデルに合わせて調整）
    m_Scale = { 0.3f, 0.3f, 0.3f };

    // 初期は停止＆非表示
    m_Ready = false;
    m_SlashTimer = 0;
    SetVisible(false);

    // Init済み
    m_Inited = true;

    // Init前にSetupされた分があれば、ここで反映
    if (m_HasPending)
    {
        ApplyParams(m_Pending);
        m_HasPending = false;
    }
}

void UltSword::Setup(const Params& p)
{
    m_Pending = p;
    m_HasPending = true;

    // Init前ならInitで反映
    if (!m_Inited) return;

    if (m_Inited)
    {
        ApplyParams(m_Pending);
        m_HasPending = false;
    }
}

void UltSword::End()
{
    m_Ready = false;
    m_SlashTimer = 0;
    SetVisible(false);
}

void UltSword::Update()
{
    if (!GetActive()) return;

    // ULT中だけ動く
    if (!m_Ready)
        return;

    // 表示されていないなら最小更新だけ
    if (!IsVisible())
    {
        if (m_SlashTimer > 0) --m_SlashTimer;
        return;
    }

    // 手元追従
    FollowCameraHandPose(m_Params.forwardDist, m_Params.rightDist, m_Params.downDist);

    if (m_SlashTimer > 0) --m_SlashTimer;

	// 斬撃アニメーション
    if (m_DoSlashAnim && m_SlashAnimTimer > 0)
    {
        // 進行度 0→1
        float t = 1.0f - (float)m_SlashAnimTimer / (float)m_SlashAnimTotal;

        // 3フェーズ：溜め(0-0.25) → 斬り(0.25-0.55) → 余韻(0.55-1.0)
        // 溜め：少し上に引いて少し後ろへ
        float windup = 0.0f;
        if (t < 0.25f)
        {
            float p = t / 0.25f;               // 0→1
            windup = p;                         // 0→1
        }

        // 斬り：一番速い部分
        float strike = 0.0f;
        if (t >= 0.25f && t < 0.55f)
        {
            float p = (t - 0.25f) / 0.30f;     // 0→1
            // EaseOutっぽく：最初速い
            strike = sinf(p * DirectX::XM_PIDIV2); // 0→1
        }
        else if (t >= 0.55f)
        {
            strike = 1.0f; // 斬り終わり到達
        }

        // 少し戻す
        float recover = 0.0f;
        if (t >= 0.55f)
        {
            float p = (t - 0.55f) / 0.45f;     // 0→1
            // 0→1 で少し戻す
            recover = sinf(p * DirectX::XM_PIDIV2); // 0→1
        }

        Camera* cam = GameManager::GetInstance()->GetCamera();
        Vector3 f = cam->GetForward();
        Vector3 r = cam->GetRight();
        Vector3 u = cam->GetUp();

        m_Position += u * (10.0f * windup);
        m_Position -= f * (6.0f * windup);

        // 斬り
        m_Position -= u * (22.0f * strike);
        m_Position += f * (6.0f * strike);

        // 余韻：少しだけ戻す
        m_Position += u * (8.0f * recover);

        // 縦切
        m_Rotation.x += DirectX::XMConvertToRadians(+20.0f) * windup;
        m_Rotation.x += DirectX::XMConvertToRadians(-70.0f) * strike;
        m_Rotation.x += DirectX::XMConvertToRadians(+25.0f) * recover;

        m_Rotation.y += DirectX::XMConvertToRadians(+8.0f) * strike;

        // 手首の返し
        m_Rotation.z += DirectX::XMConvertToRadians(-18.0f) * strike;

        --m_SlashAnimTimer;
        if (m_SlashAnimTimer <= 0)
        {
            m_SlashAnimTimer = 0;
            m_DoSlashAnim = false;
        }
    }
}

void UltSword::Draw(Camera* /*cam*/)
{
    if (!GetActive()) return;
    if (!m_Ready) return;
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

void UltSword::Uninit()
{

}

void UltSword::TrySlash()
{
    if (!GetActive()) return;
    if (!m_Ready) return;
    if (!IsVisible()) return;

    if (!m_Owner) return;
    if (!m_Targets) return;

    // 斬撃間隔
    if (m_SlashTimer > 0) return;
    m_SlashTimer = m_Params.slashIntervalF;

    GameManager::GetSound()->Play(SOUND_LABEL_SE_ULT);

	// 斬撃アニメーション開始
    m_DoSlashAnim = true;
    m_SlashAnimTimer = m_SlashAnimTotal;

    // 判定準備
    const Vector3 origin = m_Owner->GetPosition();
    Vector3 forward = GetOwnerForwardXZ(); // WeaponBaseにある前提（XZ正規化）

    const float dist2Limit = m_Params.distance * m_Params.distance;

    // 扇形：fullAngleの半分を半角として使う
    const float halfAngleDeg = m_Params.fullAngleDeg * 0.5f;
    const float cosLimit = cosf(DirectX::XMConvertToRadians(halfAngleDeg));

    int hits = 0;

    for (Enemy* e : *m_Targets)
    {
        if (!e) continue;
        if (e->GetState() == Enemy::State::Dead) continue;

        Vector3 toEnemy = e->GetPosition() - origin;
        toEnemy.y = 0.0f;

        // 距離外
        if (toEnemy.LengthSquared() > dist2Limit) continue;

        // 角度判定
        if (toEnemy.LengthSquared() < 0.0001f) continue;
        toEnemy.Normalize();

        const float dot = forward.Dot(toEnemy);
        if (dot < cosLimit) continue;

        // 範囲内全員ヒット
        e->TakeDamage(m_Attack_Power, KillType::ULT);
        hits++;
    }
}