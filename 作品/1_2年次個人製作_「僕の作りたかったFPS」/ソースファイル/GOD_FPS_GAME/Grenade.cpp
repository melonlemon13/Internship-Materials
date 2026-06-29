#include "Grenade.h"

#include"Enemy.h"
#include <iostream>
#include "GameManager.h"
#include "WallCollider.h"
#include "ExplosionEffect.h"

using namespace DirectX::SimpleMath;

static float ClampF(float v, float a, float b)
{
    return (v < a) ? a : (v > b) ? b : v;
}

Grenade::RenderAssets& Grenade::GetRenderAssets()
{
    static RenderAssets s_assets;

    if (!s_assets.loaded)
    {
        LoadAssets(s_assets);
        s_assets.loaded = true;
    }

    return s_assets;
}

void Grenade::PreloadAssets()
{
    (void)GetRenderAssets();
}

void Grenade::LoadAssets(RenderAssets& a)
{
    StaticMesh staticmesh;

    std::u8string modelFile = u8"assets/model/Grenade/grenade.obj";
    std::string texDirectory = "assets/model/Grenade";

    std::string tmpStr(reinterpret_cast<const char*>(modelFile.c_str()), modelFile.size());
    staticmesh.Load(tmpStr, texDirectory);

    a.meshRenderer.Init(staticmesh);
    a.shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

    a.subsets = staticmesh.GetSubsets();

    // move の可能性があるので 1回だけ
    a.textures = staticmesh.GetTextures();

    std::vector<MATERIAL> mats = staticmesh.GetMaterials();
    a.materials.clear();
    a.materials.reserve(mats.size());

    for (int i = 0; i < (int)mats.size(); ++i)
    {
        std::unique_ptr<Material> m = std::make_unique<Material>();
        m->Create(mats[i]);
        a.materials.push_back(std::move(m));
    }
}
void Grenade::Init()
{
	m_Scale = { 0.03f, 0.03f, 0.03f };
	m_Rotation.x = 90.0f;

    m_Inited = false;
	//まだ投げられていない状態として初期化
    m_Ready = false;
    m_Exploded = false;

    m_Position = { 0,0,0 };
    m_Velocity = { 0,0,0 };
    m_YVel = 0.0f;
    m_Gravity = -3.0f;

    m_Timer = 0;
    m_Radius = 0.0f;
    m_Damage = 0;

    m_Targets = nullptr;

    m_DeleteQueued = false;

    // Init前にSetupされた分があれば、ここで反映してReadyにする
    if (m_HasPending)
    {
        ApplyParams(m_Pending);
        m_HasPending = false;
    }
    m_Inited = true;
}

void Grenade::Setup(const Params& p)
{
    m_Pending = p;
    m_HasPending = true;

    // Setupされた＝動いていい
    m_Ready = true;
    m_Exploded = false;

    m_Throw_SE = true;

    // Init済みなら即反映、未InitならInit側で反映
    if (m_Inited)
    {
        ApplyParams(m_Pending);
        m_HasPending = false;
    }
}

void Grenade::ApplyParams(const Params& p)
{
    m_Position = p.spawnPos;

    m_Velocity = p.xzVel;
    m_Velocity.y = 0.0f;

    m_YVel = p.yVel;
    m_Gravity = p.gravity;

    m_Timer = p.explodeFrames;
    m_Radius = p.radius;
    m_Damage = p.damage;

    m_Targets = p.targets;

    // 壁当たり判定情報
    m_WallCols = p.wallCols;
    m_CollisionRadius = p.collisionRadius;

    // 跳ね返り
    m_Restitution = p.restitution;
    m_BounceDamp = p.bounceDamp;

    m_Exploded = false;
    m_Ready = true;
}

void Grenade::UpdateSpin(bool grounded)
{ 
    // パラメータ
    const float stopSpeed = 0.05f;
    const float airAngDamp = 0.995f;
    const float groundAngDamp = 0.95f;
    const float spinGain = 0.02f;

    if (grounded)
    {
        // 摩擦
        m_Velocity.x *= m_Friction;
        m_Velocity.z *= m_Friction;

        // 摩擦後のXZ速度で判定
        const float vx = m_Velocity.x;
        const float vz = m_Velocity.z;
        const float speedXZ = sqrtf(vx * vx + vz * vz);

        if (speedXZ < stopSpeed)
        {
            // 完全停止
            m_Velocity.x = 0.0f;
            m_Velocity.z = 0.0f;
            m_AngVel = Vector3::Zero;
        }
        else
        {
            // 滑ってるほど回る
            m_AngVel.x += speedXZ * spinGain;

            // 接地中は強めに減衰
            m_AngVel *= groundAngDamp;
        }
    }
    else
    {
        // 空中は弱め減衰
        m_AngVel *= airAngDamp;
    }

    // 回転適用
    m_Rotation += m_AngVel;
}


void Grenade::Update()
{
    // Setupされるまで動かさない
    if (!m_Ready) return;

    // 爆発済みなら何もしない
    if (m_Exploded) return;

    // Timer減算
    if (m_Timer > 0)
    {
        --m_Timer;
    }

	// 重力適用
    m_YVel += m_Gravity;

    // 位置更新
    m_Position.y += m_YVel;
    m_Position.x += m_Velocity.x;
    m_Position.z += m_Velocity.z;

    // 壁当たり判定解決
    ResolveWallCollisionBounce();

	// 地面判定
    bool grounded = false;
    if (m_Position.y <= m_GroundY)
    {
        m_Position.y = m_GroundY;
        m_YVel = 0.0f;
        grounded = true;
        if(m_Throw_SE == true)
        {
            GameManager::GetSound()->Play(SOUND_LABEL_SE_THROW_GRENADE);
            m_Throw_SE = false;
		}
    }

	// 回転更新
    UpdateSpin(grounded);

    // 爆発判定
    if (m_Timer <= 0)
    {
        Explode();
        GameManager::GetSound()->Play(SOUND_LABEL_SE_EXPLOSION_GRENADE);
        return;
    }
}

void Grenade::Draw(Camera* cam)
{
    if (!m_Ready) return;

    if (m_Exploded) return;

    if(!m_Inited) return;

    auto& a = GetRenderAssets();
    if (!a.loaded) return;

    Matrix r = Matrix::CreateFromYawPitchRoll( m_Rotation.y, m_Rotation.x, m_Rotation.z);
    Matrix t = Matrix::CreateTranslation( m_Position.x, m_Position.y, m_Position.z);
    Matrix s = Matrix::CreateScale( m_Scale.x, m_Scale.y, m_Scale.z);

    Matrix worldmtx = s * r * t;
    Renderer::SetWorldMatrix(&worldmtx);

    a.shader.SetGPU();
    a.meshRenderer.BeforeDraw();

    for (int i = 0; i < (int)a.subsets.size(); ++i)
    {
        const int matIdx = a.subsets[i].MaterialIdx;

        if (matIdx < 0 || matIdx >= (int)a.materials.size()) continue;

        a.materials[matIdx]->SetGPU();

        if (a.materials[matIdx]->isTextureEnable())
        {
            if (0 <= matIdx && matIdx < (int)a.textures.size() && a.textures[matIdx])
            {
                a.textures[matIdx]->SetGPU();
            }
        }

        a.meshRenderer.DrawSubset(
            a.subsets[i].IndexNum,
            a.subsets[i].IndexBase,
            a.subsets[i].VertexBase);
    }
}

void Grenade::Explode()
{
    // すでに爆発しているなら二重実行しない
    if (m_Exploded) return;

    // ダメージ判定
    ApplyDamage();

    // 爆発エフェクト生成（Update中なのでSpawnQueue→次フレームから表示）
    auto* fx = GameManager::GetInstance()->AddObject<ExplosionEffect>();
    fx->Setup(m_Position);

    // 爆発済みにする
    m_Exploded = true;

    if (!m_DeleteQueued)
    {
        m_DeleteQueued = true;
        // 自分自身を削除キューに登録
        GameManager::GetInstance()->DeleteObject(this);
    }

    // 見た目確認用の演出を入れるならここ（最小なら何もしなくてOK）
    // 例：一瞬だけスケールを上げる、爆発オブジェクトを生成する、など
}

void Grenade::ApplyDamage()
{
	// ターゲットリストがないなら何もしない
    if (!m_Targets) return;
	// ターゲットリストが空なら何もしない
    if (m_Targets->empty()) return;

    const float r2 = m_Radius * m_Radius;
    int shown = 0;
    for (auto* e : *m_Targets)
    {
        if (!e) continue;
		if (e->GetState() == Enemy::State::Dead) continue;
        if (shown < 1)
        {
            auto ep = e->GetPosition();;
            shown++;
        }
        // Enemyから座標を取る
        auto ep = e->GetPosition();

        const float dx = ep.x - m_Position.x;
        const float dz = ep.z - m_Position.z;

        const float d2 = dx * dx + dz * dz;
        if (d2 <= r2)
        {
            // Enemyへのダメージ
            e->TakeDamage(m_Damage, KillType::Grenade);
        }
    }
}

void Grenade::ResolveWallCollisionBounce()
{
    if (!m_WallCols) return;

    for (const auto& w : *m_WallCols)
    {
        const float minX = w.center.x - w.half.x;
        const float maxX = w.center.x + w.half.x;
        const float minZ = w.center.z - w.half.z;
        const float maxZ = w.center.z + w.half.z;

        const float cx = ClampF(m_Position.x, minX, maxX);
        const float cz = ClampF(m_Position.z, minZ, maxZ);

        float dx = m_Position.x - cx;
        float dz = m_Position.z - cz;

        const float d2 = dx * dx + dz * dz;
        const float r2 = m_CollisionRadius * m_CollisionRadius;

        if (d2 >= r2) continue;

        // めり込み修正
        float nx = 0.0f, nz = 0.0f;

        if (d2 < 0.000001f)
        {
            // どの面に近いかで法線を決めて脱出
            const float left = fabsf(m_Position.x - minX);
            const float right = fabsf(maxX - m_Position.x);
            const float back = fabsf(m_Position.z - minZ);
            const float front = fabsf(maxZ - m_Position.z);

            // 最小の面が押し出し方向
            float m = left; nx = -1.0f; nz = 0.0f;
            if (right < m) { m = right; nx = 1.0f; nz = 0.0f; }
            if (back < m) { m = back;  nx = 0.0f; nz = -1.0f; }
            if (front < m) { nx = 0.0f; nz = 1.0f; }

            // 押し出し（半径分）
            m_Position.x += nx * (m_CollisionRadius + 0.01f);
            m_Position.z += nz * (m_CollisionRadius + 0.01f);
        }
        else
        {
            const float d = sqrtf(d2);
            nx = dx / d;
            nz = dz / d;

            const float push = m_CollisionRadius - d;
            m_Position.x += nx * push;
            m_Position.z += nz * push;
        }

        // 跳ね返り
        // 壁へ向かっている成分だけ反射（離れていくなら何もしない）
        const float vn = m_Velocity.x * nx + m_Velocity.z * nz;
        if (vn < 0.0f)
        {
            // v' = v - (1+e)*(v·n)*n
            const float e = m_Restitution;

            m_Velocity.x = m_Velocity.x - (1.0f + e) * vn * nx;
            m_Velocity.z = m_Velocity.z - (1.0f + e) * vn * nz;

            // 跳ね返り後の減衰
            m_Velocity.x *= m_BounceDamp;
            m_Velocity.z *= m_BounceDamp;
        }

        // 1フレで複数壁に当たって変になるのを避けたいなら、ここでbreakしてもOK
        // break;
    }
}

void  Grenade::Uninit()
{

}