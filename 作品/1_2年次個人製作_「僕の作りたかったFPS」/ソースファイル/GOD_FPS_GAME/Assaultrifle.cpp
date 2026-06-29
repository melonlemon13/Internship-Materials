#include "Assaultrifle.h"

#include"GameManager.h"
#include "input.h"

using namespace DirectX::SimpleMath;

Assaultrifle::RenderAssets& Assaultrifle::GetRenderAssets()
{
    static RenderAssets s_assets;

    if (!s_assets.loaded)
    {
        LoadAssets(s_assets);
        s_assets.loaded = true;
    }

    return s_assets;
}

void Assaultrifle::PreloadAssets()
{
    // 呼ぶだけでロード済みにできる
    (void)GetRenderAssets();
}

void Assaultrifle::LoadAssets(RenderAssets& a)
{
    // メッシュ読み込み
    StaticMesh staticmesh;

    std::u8string modelFile = u8"assets/model/AR/AR.obj";
    std::string texDirectory = "assets/model/AR";

    std::string tmpStr1(reinterpret_cast<const char*>(modelFile.c_str()), modelFile.size());
    staticmesh.Load(tmpStr1, texDirectory);

    // VB/IBなど（StaticMeshから作る）
    a.meshRenderer.Init(staticmesh);

    // シェーダ
    a.shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

    // サブセット / テクスチャ / マテリアル
    a.subsets = staticmesh.GetSubsets();

    // move の可能性があるので「1回だけ」呼ぶ
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
void Assaultrifle::Init()
{
	m_Active = false;

    (void)GetRenderAssets();

    // 見た目スケール
    m_Scale = { 0.15f, 0.15f, 0.15f };

    // 武器ステータス
    m_Attack_Power = 30;

    // マガジン
    m_MagCapacity = 25;
    m_MagAmmo = m_MagCapacity;

    // リロード
    m_Reload_Time = 150;     // 2.5秒くらい
    m_IsReloading = false;

    // ここは「定数として保持」する（絶対に減らさない）
    m_FireCooldown = 6;
    m_Timer = 0;

    // 射程など
    m_Range = 900.0f;   //900/1200 
    m_HalfAngleDeg = 3.5f;   // 基本精度

    // AR固有：拡散（ブレ）
    m_SpreadDeg = 0.0f;
    m_SpreadAddPerShot = 0.25f;        // 1発ごとに +0.25度
    m_SpreadRecoverPerFrame = 0.08f;   // 撃ってない時に戻る
    m_SpreadMaxDeg = 7.0f;
    m_DidFireThisFrame = false;

    // R入力
    m_PrevR = false;
}

void Assaultrifle::Update()
{
    if (!m_Owner) return;

    // 装備中でなければ処理をスキップ（＝切替中は凍結、戻すと再開）
    if (!GetActive()) return;

	// 表示されていなければ処理をスキップ
    if (!IsVisible()) return;

    UpdateTimers();

    FollowCameraHandPose(m_ForwardDist, m_RightDist, m_DownDist);

    //スキル中はリロード進行を止める
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

        // 下から生えてくる距離
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

    // 手動リロード
    bool nowR = Input::GetKeyPress(VK_R); // 押しっぱなしtrue
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
                GameManager::GetSound()->Play(SOUND_LABEL_SE_AR_RELOAD_START);
            }
        }
    }
    m_PrevR = nowR;

    // リロード完了
    if (m_IsReloading && m_Timer == 0)
    {
        m_MagAmmo = m_MagCapacity;
        m_IsReloading = false;
        //GameManager::GetSound()->Play(SOUND_LABEL_SE_AR_RELOAD_END);
    }

    // AR固有：拡散回復
    // そのフレームに「実際に発射できた」場合は Attack() 側で m_DidFireThisFrame=true になる
    if (!m_DidFireThisFrame)
    {
        m_SpreadDeg -= m_SpreadRecoverPerFrame;
        if (m_SpreadDeg < 0.0f) m_SpreadDeg = 0.0f;
    }

	// リコイルキック
    if (m_DidFireThisFrame)
    {
        Camera* cam = GameManager::GetInstance()->GetCamera();
        Vector3 f = cam->GetForward();
        Vector3 u = cam->GetUp();

        // 後ろ + 上 にキック
        m_RecoilOffset += (-f * m_RecoilKickBack) + (u * m_RecoilKickUp);
    }

    // 次フレーム用に戻す
    m_DidFireThisFrame = false;
}

void Assaultrifle::Draw(Camera* cam)
{
    // 装備中でなければ処理をスキップ
    if (!GetActive()) return;

	// 表示されていなければ処理をスキップ
    if (!IsVisible())
    {
        UpdateTimers();
        return;
    }

    auto& a = GetRenderAssets();
    if (!a.loaded) return;

    Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
    Matrix t = Matrix::CreateTranslation( m_Position.x, m_Position.y, m_Position.z);
    Matrix s = Matrix::CreateScale( m_Scale.x, m_Scale.y, m_Scale.z);

    Matrix worldmtx = s * r * t;
    Renderer::SetWorldMatrix(&worldmtx);

    a.shader.SetGPU();
    a.meshRenderer.BeforeDraw();

    for (int i = 0; i < (int)a.subsets.size(); ++i)
    {
        const int matIdx = a.subsets[i].MaterialIdx;

        // 念のため範囲チェック（クラッシュ回避）
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

void Assaultrifle::Uninit()
{

}

void Assaultrifle::Attack()
{
    // 装備中でなければ何もしない
    if (!GetActive()) return;
    if (!m_Owner) return;
    if (!m_Targets) return;

    // クールダウン or リロード中（Timerは射撃待ち/リロード待ちを兼用）
    if (m_Timer > 0) return;

    // 弾切れ → 自動リロード開始
    if (m_MagAmmo <= 0)
    {
        m_IsReloading = true;
        m_Timer = m_Reload_Time;
        GameManager::GetSound()->Play(SOUND_LABEL_VOICE_RELOAD);
        GameManager::GetSound()->Play(SOUND_LABEL_SE_AR_RELOAD_START);
        return;
    }

    // ===== 発射 =====
    m_MagAmmo--;
    m_Timer = m_FireCooldown;
    GameManager::GetSound()->Play(SOUND_LABEL_SE_AR_FIRING);

    // このフレームに「実際に撃てた」
    m_DidFireThisFrame = true;

    // 拡散を増やす（上限）
    m_SpreadDeg += m_SpreadAddPerShot;
    if (m_SpreadDeg > m_SpreadMaxDeg) m_SpreadDeg = m_SpreadMaxDeg;

    // 発射原点（プレイヤー位置）
    Vector3 origin = m_Owner->GetPosition();

    // 狙い方向（XZ）
    Vector3 forward = GetOwnerForwardXZ();

    // 扇形判定用（基本半角 + 拡散）
    float halfAngle = m_HalfAngleDeg + m_SpreadDeg;
    float cosLimit = cosf(DirectX::XMConvertToRadians(halfAngle));

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

    // ===== 距離減衰（AR用に仮）=====
    float dist = sqrtf(bestDist2);

    const float falloffStart = 500.0f;  // 5m
    const float minMul = 0.6f;          // 射程端で60%

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

    bestEnemy->TakeDamage(finalDamage,KillType::AR);
}