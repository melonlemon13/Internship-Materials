#pragma once
#include "WeaponBase.h"

#include <SimpleMath.h>
#include "MeshRenderer.h"
#include "StaticMesh.h"
#include "Material.h"
#include "Texture2D.h"

class UltSword : public WeaponBase
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
    static void LoadAssets(RenderAssets& a);
public:
    struct Params
    {
        //ここの値は仮
        // UltSkill側の値で上書きされる
        // 表示位置（カメラ手元追従の距離）
        float forwardDist = 1.1f;
        float rightDist = 1.4f;
        float downDist = 1.0f;

        // 斬撃仕様
        int   slashIntervalF = 20;
        float distance = 150.0f; // 射程
        float fullAngleDeg = 110.0f; // 扇形
        int   damage = 50;     // ダメージ
    };
private:
    bool m_Ready = false;
    bool m_Inited = false;

    Params m_Params{};
    bool m_HasPending = false;
    Params m_Pending{};

    int m_SlashTimer = 0;

    void ApplyParams(const Params& p);

    // 斬撃アニメ
    int  m_SlashAnimTimer = 0;     // 残りフレーム
    int  m_SlashAnimTotal = 14;    // アニメ総フレーム
    bool m_DoSlashAnim = false;    // 再生中フラグ
public:
    static RenderAssets& GetRenderAssets();
    static void PreloadAssets();

    void Init() override;
    void Update() override;
    void Draw(Camera* cam) override;
    void Uninit() override;

    // ULT開始時に呼ぶ（Init前でもOK：Pendingに溜める）
    void Setup(const Params& p);

    // ULT終了時に呼ぶ（非表示＋停止）
    void End();

    // 左クリック押しっぱ中にPlayerから呼ぶ
    void TrySlash();

    // WeaponBaseの純粋仮想を満たすため（PlayerからはTrySlashを使う）
    void Attack() override { TrySlash(); }
};

