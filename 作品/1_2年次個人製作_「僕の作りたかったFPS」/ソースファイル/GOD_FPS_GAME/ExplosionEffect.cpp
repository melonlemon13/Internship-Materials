#include "ExplosionEffect.h"


#include "GameManager.h"
#include <algorithm>
#include <iostream>

using namespace DirectX::SimpleMath;

ExplosionEffect::RenderAssets& ExplosionEffect::GetRenderAssets()
{
    static RenderAssets s_assets;

    if (!s_assets.loaded)
    {
        LoadAssets(s_assets);
        s_assets.loaded = true;
    }

    return s_assets;
}

void ExplosionEffect::PreloadAssets()
{
    (void)GetRenderAssets();
}

void ExplosionEffect::LoadAssets(RenderAssets& a)
{
    StaticMesh staticmesh;

    std::u8string modelFile = u8"assets/model/golfball/golf_ball.obj";
    std::string texDirectory = "assets/model/golfball";

    std::string tmp(reinterpret_cast<const char*>(modelFile.c_str()), modelFile.size());
    staticmesh.Load(tmp, texDirectory);

    a.meshRenderer.Init(staticmesh);
    a.shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

    a.subsets = staticmesh.GetSubsets();
    a.textures = staticmesh.GetTextures();

    std::vector<MATERIAL> mats = staticmesh.GetMaterials();
    a.materials.clear();
    a.materials.reserve(mats.size());

    // 黒マテリアル化
    for (auto& m : mats)
    {
        m.Ambient = { 0,0,0,1 };
        m.Diffuse = { 0,0,0,1 };
        m.Specular = { 0,0,0,1 };
        m.Emission = { 0,0,0,1 };
        m.Shiness = 0.0f;

        auto mat = std::make_unique<Material>();
        mat->Create(m);
        a.materials.push_back(std::move(mat));
    }
}
void ExplosionEffect::Setup(const Vector3& pos)
{
    // Pendingに積む
    m_Pending.position = pos;
    m_HasPending = true;

    // Init後にSetupが来た場合は即反映
    if (m_Inited)
    {
        ApplyParams(m_Pending);
        m_HasPending = false;
    }
}

void ExplosionEffect::ApplyParams(const PendingParams& p)
{
    m_Position = p.position;

    // 初期状態
    m_RadiusNow = 0.0f;
    m_Life = 24;
}

void ExplosionEffect::Init()
{
    auto& a = GetRenderAssets();

    // 共有マテリアルをコピー（個体用）
    m_Materials.clear();
    m_Materials.reserve(a.materials.size());

    for (auto& src : a.materials)
    {
        auto m = std::make_unique<Material>();
        *m = *src;
        m_Materials.push_back(std::move(m));
    }

    if (m_HasPending)
    {
        ApplyParams(m_Pending);
        m_HasPending = false;
    }

    m_Inited = true;
}

void ExplosionEffect::Uninit()
{
}

void ExplosionEffect::Update()
{
    if (!m_Inited) return;

    // 寿命と膨張
    m_Life--;
    m_RadiusNow += m_GrowPerFrame;
    if (m_RadiusNow > m_RadiusMax) m_RadiusNow = m_RadiusMax;

    // 半径→スケール（球なのでXYZ同一）
    const float s = m_RadiusNow * m_ModelScaleFactor;
    m_Scale = Vector3(s, s, s);

    if (m_Life <= 0)
    {
        GameManager::GetInstance()->DeleteObject(this);
        return;
    }
}

void ExplosionEffect::Draw(Camera* cam)
{
    if (!m_Inited) return;

    auto& a = GetRenderAssets();
    if (!a.loaded) return;

    Matrix world =
        Matrix::CreateScale(m_Scale) *
        Matrix::CreateTranslation(m_Position);

    Renderer::SetWorldMatrix(&world);

    a.shader.SetGPU();
    a.meshRenderer.BeforeDraw();

    for (int i = 0; i < (int)a.subsets.size(); ++i)
    {
        int idx = a.subsets[i].MaterialIdx;

        if (idx < 0 || idx >= (int)m_Materials.size()) continue;

        m_Materials[idx]->SetGPU();

        if (m_Materials[idx]->isTextureEnable())
        {
            if (idx < (int)a.textures.size())
                a.textures[idx]->SetGPU();
        }

        a.meshRenderer.DrawSubset(
            a.subsets[i].IndexNum,
            a.subsets[i].IndexBase,
            a.subsets[i].VertexBase);
    }
}