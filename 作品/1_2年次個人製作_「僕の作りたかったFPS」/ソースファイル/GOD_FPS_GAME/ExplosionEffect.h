#pragma once
#include "ObjectBase.h"
#include "SimpleMath.h"
#include "MeshRenderer.h"
#include "StaticMesh.h"
#include "Material.h"
#include <vector>
#include <memory>
#include "Texture2D.h"

class ExplosionEffect : public ObjectBase
{
private:
    struct RenderAssets
    {
        bool loaded = false;

        MeshRenderer meshRenderer;
        Shader shader;

        std::vector<SUBSET> subsets;
        std::vector<std::unique_ptr<Texture>> textures;
        std::vector<std::unique_ptr<Material>> materials;
    };

    static RenderAssets& GetRenderAssets();
    static void LoadAssets(RenderAssets& a); Shader shader;

    // -----------------------------
    // Pending方式
    // -----------------------------
    struct PendingParams
    {
        DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
    };

    void ApplyParams(const PendingParams& p);

private:
    // 個体用（色変えなどする場合に必要）
    std::vector<std::unique_ptr<Material>> m_Materials;

    // 膨張(半径)
    float m_RadiusNow = 0.0f;

    // 寿命管理
    int m_Life = 24;

    // 固定パラメータ
    float m_RadiusMax = 150.0f;
    float m_GrowPerFrame = 150.0f / 24.0f; // = 6.25

    float m_ModelScaleFactor = 1.0f;

    // Pendingフラグ
    PendingParams m_Pending{};
    bool m_HasPending = false;

    // Init済みフラグ
    bool m_Inited = false;

public:
    static void PreloadAssets();
    // 爆発座標を渡す（Update中にAddObjectされても安全に反映できるようPending対応）
    void Setup(const DirectX::SimpleMath::Vector3& pos);

    void Init() override;
    void Update() override;
    void Draw(Camera* cam) override;
    void Uninit() override;
};
