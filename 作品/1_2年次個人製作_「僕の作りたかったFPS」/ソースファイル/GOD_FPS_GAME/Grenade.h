#pragma once
#include "ObjectBase.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "StaticMesh.h"
#include "Material.h"

class Enemy;
struct WallCollider;

class Grenade : public ObjectBase
{
    // GrenadeSkillから一括で渡す初期化パラメータ
    //初期化ミス防止
public:
    struct Params
    {
        //ここの値は使わず、グレネードスキル側から受け取った値を使う
        DirectX::SimpleMath::Vector3 spawnPos = { 0,0,0 };

        // XZは Position 直叩きのため「XZ移動量」を持つ（yは0でOK）
        DirectX::SimpleMath::Vector3 xzVel = { 0,0,0 };

        // Y方向のみ速度を持つ
        float yVel = 0.0f;

        // 重力（cm/frame）
        float gravity = -2.0f;

        // 爆発までのフレーム
        int explodeFrames = 0;

        // 爆発半径（XZ）
        float radius = 0.0f;

        // ダメージ
        int damage = 0;

        // 対象
        std::vector<Enemy*>* targets = nullptr; 
        
		// 壁当たり判定情報へのポインタ
        const std::vector<WallCollider>* wallCols = nullptr;
        float collisionRadius = 0.0f;

        // 跳ね返り用
        float restitution = 0.0f; // 反発係数 
        float bounceDamp = 0.0f; // 跳ね返り後の速度減衰
    };

private:
    //AddObjectの仕組みを変えたので必要
	// 保持中の初期化パラメータ
	// Setupが呼ばれるまで保持
    // 壁コライダー
    const std::vector<WallCollider>* m_WallCols = nullptr;
    // 当たり判定半径
    float m_CollisionRadius = 10.0f;

    // 跳ね返り用
    float m_Restitution = 0.45f;
    float m_BounceDamp = 0.90f;

    Params m_Pending{};
    bool   m_HasPending = false;
    // Setupが呼ばれるまでUpdateで動かない
    bool m_Ready = false;
	// 既に初期化済みかどうか
    bool m_Inited = false;
    // 状態
    DirectX::SimpleMath::Vector3 m_Velocity = { 0,0,0 };
    float m_YVel = 0.0f;
    float m_Gravity = -3.0f;

    //摩擦
    float m_Friction = 0.95;

    int   m_Timer = 0;
    float m_Radius = 0.0f;
    int   m_Damage = 0;

    bool  m_Exploded = false;

	bool m_Throw_SE = true;

    // 外部参照
    std::vector<Enemy*>* m_Targets = nullptr;

	// 回転速度
	DirectX::SimpleMath::Vector3 m_AngVel = { 0.0f, 0.0f, 0.1f };

	// 地面Y座標
    float m_GroundY = -10.0f;

	// 自分自身を削除キューに登録済みかどうか
    bool m_DeleteQueued = false;

    bool loaded = false;

    //初期化用
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
    static void LoadAssets(RenderAssets& a);

public:
    static void PreloadAssets();

    void Init() override;
    void Update() override;

    void Draw(Camera* cam) override;
    void Uninit() override;

    void Setup(const Params& p);
	// Setupで渡されたパラメータを実際に反映する処理
    void ApplyParams(const Params& p);
    
	// 回転更新処理
    void UpdateSpin(bool grounded);

	// 爆発処理
    void Explode();
	// ダメージ適用処理
	void ApplyDamage();

	// 壁との衝突解決
    void ResolveWallCollisionBounce();
    //壁の当たり判定を設定
    void SetWallColliders(const std::vector<WallCollider>* cols) { m_WallCols = cols; }

    bool IsExploded() const { return m_Exploded; }
};

