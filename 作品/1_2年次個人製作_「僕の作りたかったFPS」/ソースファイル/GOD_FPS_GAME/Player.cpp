#include "Player.h"
#include "input.h"
#include <algorithm>
#include "WallCollider.h"

#include"WeaponBase.h"
#include"Pistol.h"
#include"Assaultrifle.h"
#include"Knife.h"
#include "GameManager.h"

using namespace DirectX::SimpleMath;

static float ClampF(float v, float a, float b)
{
	return (v < a) ? a : (v > b) ? b : v;
}

void Player::Init()
{
	{
		//描画のための初期化
		//頂点データ
		m_Vertices.resize(24);
		{//1
			m_Vertices[0].position = Vector3(-1, 1, 1);
			m_Vertices[1].position = Vector3(1, 1, 1);
			m_Vertices[2].position = Vector3(-1, 1, -1);
			m_Vertices[3].position = Vector3(1, 1, -1);

			m_Vertices[0].color = Color(1, 1, 1, 1);
			m_Vertices[1].color = Color(1, 1, 1, 1);
			m_Vertices[2].color = Color(1, 1, 1, 1);
			m_Vertices[3].color = Color(1, 1, 1, 1);

			m_Vertices[0].uv = Vector2(0, 0);
			m_Vertices[1].uv = Vector2(0.33f, 0);
			m_Vertices[2].uv = Vector2(0, 0.5f);
			m_Vertices[3].uv = Vector2(0.33f, 0.5f);

			m_Vertices[0].normal = Vector3(0, 1, 0);
			m_Vertices[1].normal = Vector3(0, 1, 0);
			m_Vertices[2].normal = Vector3(0, 1, 0);
			m_Vertices[3].normal = Vector3(0, 1, 0);

		}

		{//2
			m_Vertices[4].position = Vector3(-1, 1, -1);
			m_Vertices[5].position = Vector3(1, 1, -1);
			m_Vertices[6].position = Vector3(-1, -1, -1);
			m_Vertices[7].position = Vector3(1, -1, -1);

			m_Vertices[4].color = Color(1, 1, 1, 1);
			m_Vertices[5].color = Color(1, 1, 1, 1);
			m_Vertices[6].color = Color(1, 1, 1, 1);
			m_Vertices[7].color = Color(1, 1, 1, 1);

			m_Vertices[4].uv = Vector2(0.33f, 0);
			m_Vertices[5].uv = Vector2(0.66f, 0);
			m_Vertices[6].uv = Vector2(0.33f, 0.5f);
			m_Vertices[7].uv = Vector2(0.66f, 0.5f);

			m_Vertices[4].normal = Vector3(0, 0, -1);
			m_Vertices[5].normal = Vector3(0, 0, -1);
			m_Vertices[6].normal = Vector3(0, 0, -1);
			m_Vertices[7].normal = Vector3(0, 0, -1);
		}

		{//4
			m_Vertices[8].position = Vector3(-1, 1, -1);
			m_Vertices[9].position = Vector3(-1, -1, -1);
			m_Vertices[10].position = Vector3(-1, -1, 1);
			m_Vertices[11].position = Vector3(-1, 1, 1);

			m_Vertices[8].color = Color(1, 1, 1, 1);
			m_Vertices[9].color = Color(1, 1, 1, 1);
			m_Vertices[10].color = Color(1, 1, 1, 1);
			m_Vertices[11].color = Color(1, 1, 1, 1);

			m_Vertices[8].uv = Vector2(0, 0.5f);
			m_Vertices[9].uv = Vector2(0.33f, 0.5f);
			m_Vertices[10].uv = Vector2(0.33f, 1.0f);
			m_Vertices[11].uv = Vector2(0, 1.0f);

			m_Vertices[8].normal = Vector3(-1, 0, 0);
			m_Vertices[9].normal = Vector3(-1, 0, 0);
			m_Vertices[10].normal = Vector3(-1, 0, 0);
			m_Vertices[11].normal = Vector3(-1, 0, 0);
		}

		{//5
			m_Vertices[12].position = Vector3(-1, 1, 1);
			m_Vertices[13].position = Vector3(-1, -1, 1);
			m_Vertices[14].position = Vector3(1, -1, 1);
			m_Vertices[15].position = Vector3(1, 1, 1);

			m_Vertices[12].color = Color(1, 1, 1, 1);
			m_Vertices[13].color = Color(1, 1, 1, 1);
			m_Vertices[14].color = Color(1, 1, 1, 1);
			m_Vertices[15].color = Color(1, 1, 1, 1);

			m_Vertices[12].uv = Vector2(0.33f, 0.5f);
			m_Vertices[13].uv = Vector2(0.66f, 0.5f);
			m_Vertices[14].uv = Vector2(0.66, 1.0f);
			m_Vertices[15].uv = Vector2(0.33f, 1.0f);

			m_Vertices[12].normal = Vector3(0, 0, 1);
			m_Vertices[13].normal = Vector3(0, 0, 1);
			m_Vertices[14].normal = Vector3(0, 0, 1);
			m_Vertices[15].normal = Vector3(0, 0, 1);
		}

		{//3
			m_Vertices[16].position = Vector3(1, 1, 1);
			m_Vertices[17].position = Vector3(1, -1, 1);
			m_Vertices[18].position = Vector3(1, -1, -1);
			m_Vertices[19].position = Vector3(1, 1, -1);

			m_Vertices[16].color = Color(1, 1, 1, 1);
			m_Vertices[17].color = Color(1, 1, 1, 1);
			m_Vertices[18].color = Color(1, 1, 1, 1);
			m_Vertices[19].color = Color(1, 1, 1, 1);

			m_Vertices[16].uv = Vector2(0.66f, 0);
			m_Vertices[17].uv = Vector2(1.0f, 0);
			m_Vertices[18].uv = Vector2(1.0f, 0.5f);
			m_Vertices[19].uv = Vector2(0.66f, 0.5f);

			m_Vertices[16].normal = Vector3(1, 0, 0);
			m_Vertices[17].normal = Vector3(1, 0, 0);
			m_Vertices[18].normal = Vector3(1, 0, 0);
			m_Vertices[19].normal = Vector3(1, 0, 0);
		}

		{//6
			m_Vertices[20].position = Vector3(1, -1, 1);
			m_Vertices[21].position = Vector3(-1, -1, 1);
			m_Vertices[22].position = Vector3(-1, -1, -1);
			m_Vertices[23].position = Vector3(1, -1, -1);

			m_Vertices[20].color = Color(1, 1, 1, 1);
			m_Vertices[21].color = Color(1, 1, 1, 1);
			m_Vertices[22].color = Color(1, 1, 1, 1);
			m_Vertices[23].color = Color(1, 1, 1, 1);

			m_Vertices[20].uv = Vector2(0.66f, 0.5f);
			m_Vertices[21].uv = Vector2(1.0f, 0.5f);
			m_Vertices[22].uv = Vector2(1.0, 1.0f);
			m_Vertices[23].uv = Vector2(0.66f, 1.0f);

			m_Vertices[20].normal = Vector3(0, -1, 0);
			m_Vertices[21].normal = Vector3(0, -1, 0);
			m_Vertices[22].normal = Vector3(0, -1, 0);
			m_Vertices[23].normal = Vector3(0, -1, 0);
		}

		// 頂点バッファ生成
		m_VertexBuffer.Create(m_Vertices);

		// インデックスバッファ生成
		m_Indices.resize(36);

		m_Indices = {
		0, 1, 2,
		1, 3, 2,
		4, 5, 6,
		5, 7, 6,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
		};

		// インデックスバッファ生成
		m_IndexBuffer.Create(m_Indices);

		// シェーダオブジェクト生成
		//m_Shader.Create("shader/unlitTextureVS.hlsl", "shader/unlitTexturePS.hlsl");
		m_Shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");
		bool sts = m_Texture.Load("assets/texture/dice.png");
		assert(sts == true);
	}

	//HP
	m_Hp = 100;

	//スピード
	//ゲーム中に変更が加わるのでここで初期化
	m_Speed_W = 2.0f;
	m_Speed_A = 1.8f;
	m_Speed_S = 1.8f;
	m_Speed_D = 1.8f;

	//視点
	//ゲーム中に変更が加わるかもしれないのでここで初期化
	m_Sens = 0.0007f;

	//向き
	m_Rotation = Vector3(0.0f, 0.0f, 0.0f);

	m_EnemyNearSE_Cooldown = 0;

	//SE用フラグ
	low_voice_flg = true;

	//ダメージ間隔初期化
	m_Damage_Interval = 60.0f;
	m_Now_Damage_Interval = 0.0f;

	//現在の武器をナイフに設定
	m_Now_Weapon = m_KNIFE_SLOT;
	m_Old_Weapon = m_Now_Weapon;

	//武器を追加
	m_pWeapon[m_PISTROL_SLOT] = GameManager::GetInstance()->AddObject<Pistol>();
	m_pWeapon[m_ASSAULT_SLOT] = GameManager::GetInstance()->AddObject<Assaultrifle>();
	m_pWeapon[m_KNIFE_SLOT] = GameManager::GetInstance()->AddObject<Knife>();

	//全ての武器にOwnerセット
	for (int i = 0; i < 3; ++i)
	{
		if (m_pWeapon[i]) m_pWeapon[i]->SetOwner(this);
	}

	//Activeの初期状態を整える
	for (int i = 0; i < 3; ++i)
	{
		//ナイフだけtrueに,他はfalseに
		if (m_pWeapon[i]) m_pWeapon[i]->SetActive(i == m_Now_Weapon);
	}

	//スキル初期化
	m_Blink_Skill.Init(this);
	m_Grenade_Skill.Init(this);
	m_Ult_Skill.Init(this);

	//スキル用変数初期化
	m_Blink_Timer = 0;
	m_BlinkDir = { 0,0,0 };
	// スペースキー離すまでブリンク禁止フラグ
	m_BlockBlinkUntilSpaceRelease = false;
	// 空中ブリンク（追加移動）の状態
	m_AirBlinkActive = false;
	m_AirBlinkFrame = 0;
	m_AirBlinkTotalFrames = 0;

	m_AirBlinkTotalDist = 0.0f;   // 合計距離（cm）
	m_AirBlinkPrevEase = 0.0f;    // 前フレームのEase進捗（0〜1）

	// ULT刀生成
	m_UltSword = GameManager::GetInstance()->AddObject<UltSword>();
	m_UltSword->SetOwner(this);
	m_UltSword->SetActive(true);
	m_UltSword->SetVisible(false);
	m_UltSword->End();

	m_Scale = Vector3(8.5f, 8.5f, 8.5f);
}

void Player::Update()
{
	//武器切り替え
	if (!IsWeaponSwitchBlocked())
	{
		HandleWeaponHotkeys();
	}

	//視点移動
	UpdateLook();

	//プレイヤー移動
	move();

	// スキル更新の前に、今フレームのブロック状態を作る
	UpdateSkillBlocks();
	//スキル更新
	SkillUpdate();
	//スキルのブロック状態を更新
	UpdateSkillBlocks();
	//ブリンク移動更新
	UpdateBlinkMove();
	//スキルのブロック状態を武器に適用
	ApplySkillBlocksToWeapons();

	//疑似速度計算
	m_MoveVelocity = m_Position - m_PrevPosition;
	m_MoveVelocity.y = 0.0f;
	m_PrevPosition = m_Position;

	//壁との当たり判定を解決
	ResolveWallCollision();

	//攻撃
	Attack();

	//ダメージ間隔を減少
	if (m_Now_Damage_Interval > 0)
	{
		m_Now_Damage_Interval--;
		if (m_Now_Damage_Interval < 0)
		{
			m_Now_Damage_Interval = 0;
		}
	}

	//敵接近SE更新
	UpdateEnemyNearSE();

	//ULTスキル処理
	bool ultNow = m_Ult_Skill.IsActive();

	// ULTに入った瞬間
	if (ultNow && !m_WasUltActivePrev)
	{
		GameManager::GetSound()->Play(SOUND_LABEL_VOICE_ULT);
		m_PreUltWeapon = m_Now_Weapon;   // 発動前武器を保存

		// ULT刀のパラメータをセットしてReady=trueにする
		if (m_UltSword)
		{
			UltSword::Params p;
			p.slashIntervalF = 15;
			p.distance = 150.0f;
			p.fullAngleDeg = 110.0f;
			p.damage = 50;

			// 手元位置（必要なら調整）
			p.forwardDist = 1.8f;
			p.rightDist = 0.6f;
			p.downDist = 0.4f;

			m_UltSword->Setup(p);

			// 念のため（InitでActive=trueにしてるなら不要だけど安全）
			m_UltSword->SetActive(true);
		}
	}

	// ULTが終わった瞬間（Active→Cooldown）
	if (!ultNow && m_WasUltActivePrev)
	{
		// ULT刀を停止＆非表示
		if (m_UltSword)
		{
			m_UltSword->End();          // Ready=false, Visible=false, timerリセット
			m_UltSword->SetVisible(false);
		}

		ChangeWeapon(m_PreUltWeapon);    // 元に戻す
	}

	m_WasUltActivePrev = ultNow;

	//カメラを更新
	//ここに書くのは本当に良くない
	GameManager::GetInstance()->GetCamera()->UpdateFPS(m_Position, m_Rotation, *m_WallCols, m_CollisionRadius);
}

void Player::Draw(Camera* cam)
{
	////カメラを選択する
	//cam->SetCamera();

	//// SRT情報作成
	//Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	//Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	//Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	//Matrix worldmtx;
	//worldmtx = s * r * t;
	//Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	//// 描画の処理
	//ID3D11DeviceContext* devicecontext;
	//devicecontext = Renderer::GetDeviceContext();

	//// トポロジーをセット（プリミティブタイプ）
	//devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//m_Shader.SetGPU();
	//m_VertexBuffer.SetGPU();
	//m_IndexBuffer.SetGPU();

	//m_Texture.SetGPU();

	//devicecontext->DrawIndexed(
	//	m_Indices.size(),	// 描画するインデックス数
	//	0,					// 最初のインデックスバッファの位置
	//	0);
}

void Player::Uninit()
{
	if (m_UltSword)
	{
		GameManager::GetInstance()->DeleteObject(m_UltSword);
		m_UltSword = nullptr;
	}

	for (int i = 0; i < WEAPON_MAX; ++i)
	{
		if (!m_pWeapon[i]) continue;
		GameManager::GetInstance()->DeleteObject(m_pWeapon[i]);
		m_pWeapon[i] = nullptr;
	}
}

void Player::UpdateLook()
{
	// 感度調整
	if (Input::GetKeyTrigger(VK_LEFT))
	{
		m_Sens -= m_SensStep;
	}
	if (Input::GetKeyTrigger(VK_RIGHT))
	{
		m_Sens += m_SensStep;
	}

	// 範囲制限
	m_Sens = std::clamp(m_Sens, m_MinSens, m_MaxSens);

	//マウス横入力用変数
	float dx = (float)Input::GetMouseMoveX();
	//マウス縦入力用変数
	float dy = (float)Input::GetMouseMoveY();

	//入力があった分*Sens分視点を移動
	//xとyが一致しないのでややこしい
	//注意
	//左右
	m_Rotation.y += dx * m_Sens;
	//上下
	//-にすることにより上下を反転
	//マウスを下に移動させると視点も下に、マウスを上に移動させると視点も上に
	m_Rotation.x -= dy * m_Sens;

	//視点が上下限を超えないように丸見込み
	//clamp 値がある範囲からはみ出ないように強制的に収めるための関数
	//if文での丸め込みと意味は同じだがこちらの方が簡潔
	m_Rotation.x = std::clamp(m_Rotation.x, -m_PITCH_LIMIT, m_PITCH_LIMIT);
}

void Player::move()
{
	//ローカル変数宣言
	//前(移動)方向(角度)を決定
	Vector3 forward(sinf(m_Rotation.y), 0.0f, cosf(m_Rotation.y));
	//正規化
	forward.Normalize();
	//世界の上とforward(前方向)に対して必ず直角なright(右)を作成
	Vector3 right = Vector3(0, 1, 0).Cross(forward);
	//正規化
	right.Normalize();

	//forwardとrightを使って求めた進行方向を入れるための変数
	Vector3 dir(0, 0, 0);
	//スピード
	//入力があったキーに設定されているスピードを代入
	float speed = 0.0f;

	{//WASD移動
		{//入力処理
			//A
			if (Input::GetKeyPress(VK_A))
			{
				dir -= right;
				speed = m_Speed_A;
			}
			//S
			if (Input::GetKeyPress(VK_S))
			{
				dir -= forward;
				speed = m_Speed_S;
			}
			//D
			if (Input::GetKeyPress(VK_D))
			{
				dir += right;
				speed = m_Speed_D;
			}
			//W
			//ここを最後に持ってくることによってWA,WDみたいな入力の時でもダッシュできる
			//あんまりきれいじゃない
			if (Input::GetKeyPress(VK_W))
			{
				dir += forward;
				speed = m_Speed_W;
				//左Shift入力があればダッシュ1.5倍のスピードに
				if (Input::GetKeyPress(VK_LSHIFT))
				{
					speed *= 1.5f;
				}
			}
		}

		//ULTスキルの移動速度倍率を取得して速度に掛ける
		float ultMul = m_Ult_Skill.GetMoveSpeedMul();
		speed *= ultMul;

		{//移動処理
			//最後に、入力がある間、今まで作った値で移動させる
			//LengthSquared() このベクトルがどれくらいの大きさか
			//ほぼ0より大きい場合にのみ移動
			//これがないと変な値が正規化されてしまい、バグの原因になる
			if (dir.LengthSquared() > 0.0001f)
			{
				dir.Normalize();
				m_Position += dir * speed;
			}
		}
	}

	{//ジャンプ
		{
			//入力処理
			if (Input::GetKeyTrigger(VK_SPACE))
			{
				if (m_On_Ground)
				{
					// 地上：ジャンプ
					m_Velocity.y = m_Jump_Speed;
					m_On_Ground = false;

					// このSPACEが離されるまでブリンク禁止
					m_BlockBlinkUntilSpaceRelease = true;
				}
			}
		}

		{//ジャンプ処理,重力処理
			//ジャンプできない(空中にいる)とき
			if (m_On_Ground == false)
			{
				//60FPSなので、1/60に一回重力を加える
				m_Velocity.y += m_Gravity * (1.0f / 60.0f);
				//同じく60FPSなので、1/60に一回速度を加える
				m_Position.y += m_Velocity.y * (1.0f / 60.0f);

				//地面より下に行きそうなら
				if (m_Position.y <= m_GroundY)
				{
					//プレイヤー着地
					m_Position.y = m_GroundY;
					//速度を0に
					m_Velocity.y = 0.0f;
					//ジャンプ可能に
					m_On_Ground = true;
				}
			}
		}
	}
}

void Player::Attack()
{
	//ULTスキル発動中はULT攻撃のみ
	if (m_Ult_Skill.IsActive())
	{
		if (Input::GetKeyPress(VK_LBUTTON))
		{
			m_UltSword->TrySlash();
		}
		return;
	}

	//攻撃ブロック中は攻撃しない
	if (m_SkillBlocks.attack) return;

	//マウスの左クリックが押されている間
	if (Input::GetKeyPress(VK_LBUTTON))
	{
		//現在所持している武器のAttack()を呼び出す
		m_pWeapon[m_Now_Weapon]->Attack();
	}
}

void Player::ApplyDamage(int dmg)
{
	if (m_Now_Damage_Interval > 0) return;
	if (m_Hp <= 0) return;
	m_Hp -= dmg;
	//死亡処理
	if (m_Hp <= 0) return;
	m_Now_Damage_Interval = m_Damage_Interval;
	if (m_Hp <= 20 && low_voice_flg == true)
	{
		low_voice_flg = false;
		GameManager::GetSound()->Play(SOUND_LABEL_VOICE_LOW_HP);
	}
	else
	{
		GameManager::GetSound()->Play(SOUND_LABEL_VOICE_TAKE_DAMAGE);
	}
}

Vector3 Player::GetForward() const
{
	float yaw = m_Rotation.y;

	return DirectX::SimpleMath::Vector3(
		sinf(yaw),   // X
		0.0f,        // Y（上下は無視）
		cosf(yaw)    // Z
	);
}

Vector3 Player::GetRight() const
{
	Vector3 forward = GetForward();
	Vector3 up(0.0f, 1.0f, 0.0f);

	Vector3 right = up.Cross(forward);
	right.Normalize();
	return right;
}

void Player::ResolveWallCollision()
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

void Player::SetupWeapons(std::vector<Enemy*>* targets)
{
	for (int i = 0; i < WEAPON_MAX; ++i)
	{
		if (m_pWeapon[i])
		{
			m_pWeapon[i]->SetOwner(this);
			m_pWeapon[i]->SetTargets(targets);
			m_pWeapon[i]->SetActive(false);
		}
	}

	// 初期武器：ナイフ（インデックス 2）
	m_Now_Weapon = 2;

	if (0 <= m_Now_Weapon && m_Now_Weapon < WEAPON_MAX && m_pWeapon[m_Now_Weapon])
	{
		m_pWeapon[m_Now_Weapon]->SetActive(true);
	}

	if (m_UltSword)
	{
		m_UltSword->SetOwner(this);
		m_UltSword->SetTargets(targets);
	}
}

void Player::HandleWeaponHotkeys()
{
	// スキルが武器切替をブロックしている間は変更不可
	if (IsWeaponSwitchBlocked()) return;
	// 押しっぱなし対策：前フレーム状態を保持
	static bool prev1 = false;
	static bool prev2 = false;
	static bool prev3 = false;

	bool now1 = Input::GetKeyPress('1');
	bool now2 = Input::GetKeyPress('2');
	bool now3 = Input::GetKeyPress('3');

	// 立ち上がり（false→true）だけ反応
	//ややこしい
	if (now1 && !prev1) ChangeWeapon(0);
	if (now2 && !prev2) ChangeWeapon(1);
	if (now3 && !prev3) ChangeWeapon(2);

	prev1 = now1;
	prev2 = now2;
	prev3 = now3;
}

void Player::ChangeWeapon(int newIndex)
{
	// 範囲外は無視
	if (newIndex < 0 || newIndex >= WEAPON_MAX) return;

	// その武器が存在しないなら無視
	if (!m_pWeapon[newIndex]) return;

	// 同じなら何もしない
	if (newIndex == m_Now_Weapon) return;

	// 旧武器をOFF
	if (0 <= m_Now_Weapon && m_Now_Weapon < WEAPON_MAX && m_pWeapon[m_Now_Weapon])
	{
		m_pWeapon[m_Now_Weapon]->SetActive(false);
	}

	// 新武器をON
	m_Now_Weapon = newIndex;
	m_pWeapon[m_Now_Weapon]->SetActive(true);
}

void Player::SkillUpdate()
{
	//CT更新
	{
		m_Blink_Skill.Update();
		m_Grenade_Skill.Update();
		m_Ult_Skill.Update();
	}

	// スペースキーが離されるとブリンク禁止解除
	if (Input::GetKeyRelease(VK_SPACE))
	{
		m_BlockBlinkUntilSpaceRelease = false;
	}

	// スキル発動入力をまとめてブロック
	if (IsOtherSkillsBlocked())
	{
		return;
	}

	{// 発動入力
		// ブリンク発動入力
		if (Input::GetKeyTrigger(VK_SPACE) && (!m_BlockBlinkUntilSpaceRelease))
		{
			m_Blink_Skill.TryActivate();
		}

		// グレネード発動入力
		if (Input::GetKeyTrigger(VK_E))
		{
			m_Grenade_Skill.TryActivate();
		}

		// ULT発動入力
		if (Input::GetKeyTrigger(VK_Q))
		{
			m_Ult_Skill.TryActivate();
		}
	}
}

Vector3 Player::GetForwardXZ() const
{
	Camera* cam = GameManager::GetInstance()->GetCamera();
	Vector3 f = cam->GetForward();      // (x,y,z)
	f.y = 0.0f;
	// ほぼ0ならゼロで返す
	if (f.LengthSquared() <= 0.00001f) return Vector3(0, 0, 0);
	f.Normalize();
	return f;
}

Vector3 Player::GetRightXZ() const
{
	Camera* cam = GameManager::GetInstance()->GetCamera();

	Vector3 r = cam->GetRight();          // (x,y,z)
	r.y = 0.0f;

	// ほぼ0ならゼロで返す
	if (r.LengthSquared() <= 0.00001f) return Vector3(0, 0, 0);

	r.Normalize();
	return r;
}

Vector3 Player::GetMoveInputDirXZ() const
{
	// WASDを -1/0/+1 にする
	//条件 ? A : B
	// 条件がtrueならAを、falseならBを返す
	float w = Input::GetKeyPress(VK_W) ? 1.0f : 0.0f;
	float s = Input::GetKeyPress(VK_S) ? 1.0f : 0.0f;
	float d = Input::GetKeyPress(VK_D) ? 1.0f : 0.0f;
	float a = Input::GetKeyPress(VK_A) ? 1.0f : 0.0f;

	float forward = w - s; // W=+、S=-
	float right = d - a; // D=+、A=-

	// 入力が無い
	if (forward == 0.0f && right == 0.0f)
		return Vector3(0, 0, 0);

	// カメラ基準のXZ軸
	Vector3 f = GetForwardXZ();
	Vector3 r = GetRightXZ();

	// 合成（カメラ基準移動）
	Vector3 dir = f * forward + r * right;

	// 正規化（斜め入力が速くならないようにする）
	if (dir.LengthSquared() > 0.00001f)
		dir.Normalize();

	return dir;
}

void Player::StartBlink(const Vector3& dirXZ, float totalDistCm, int frames)
{
	Vector3 d(dirXZ.x, 0.0f, dirXZ.z);

	if (d.LengthSquared() <= 0.00001f) return;
	d.Normalize();

	m_AirBlinkActive = true;
	m_AirBlinkFrame = 0;
	m_AirBlinkTotalFrames = (frames <= 0) ? 1 : frames;

	m_AirBlinkTotalDist = totalDistCm;
	m_AirBlinkPrevEase = 0.0f;

	m_BlinkDir = d;
}

void Player::UpdateBlinkMove()
{
	if (!m_AirBlinkActive) return;

	// フレームカウント進行
	m_AirBlinkFrame++;

	// イージング計算
	//イージングとは、アニメーションの動きに自然な「緩急（かんきゅう）」をつける技術
	float t = (float)m_AirBlinkFrame / (float)m_AirBlinkTotalFrames; // 0→1
	float ease = EaseOut(t);

	// 前フレームとの差分移動量を計算して移動
	float delta = ease - m_AirBlinkPrevEase;
	m_AirBlinkPrevEase = ease;

	// 移動量計算・適用
	float moveDist = delta * m_AirBlinkTotalDist; // cm
	m_Position += m_BlinkDir * moveDist;

	// 終了判定
	if (m_AirBlinkFrame >= m_AirBlinkTotalFrames)
	{
		m_AirBlinkActive = false;
	}
}

float Player::EaseOut(float t) const
{
	// 0〜1に丸める
	t = std::clamp(t, 0.0f, 1.0f);

	// EaseOut Quadratic: 1 - (1-t)^2
	float u = 1.0f - t;
	return 1.0f - (u * u);
}

void Player::UpdateEnemyNearSE()
{
	// クールダウン減少
	if (m_EnemyNearSE_Cooldown > 0)
		--m_EnemyNearSE_Cooldown;

	// 敵リストが無ければ何もしない
	if (!m_Enemies) return;

	// 判定範囲
	constexpr float kRange = 50.0f;
	const float kRange2 = kRange * kRange;

	const auto p = m_Position;

	bool anyInRange = false;

	for (auto* e : *m_Enemies)
	{
		if (!e) continue;

		if (e->GetState() == Enemy::State::Dead) continue;

		auto ep = e->GetPosition();

		float dx = ep.x - p.x;
		float dz = ep.z - p.z;
		float d2 = dx * dx + dz * dz;

		if (d2 <= kRange2)
		{
			anyInRange = true;
			break;
		}
	}

	// 範囲内の敵がいて、クールダウンが終わってたら鳴らす
	if (anyInRange && m_EnemyNearSE_Cooldown <= 0)
	{
		GameManager::GetSound()->Play(SOUND_LABEL_SE_WARNING);

		m_EnemyNearSE_Cooldown = 60;
	}
}

void Player::UpdateSkillBlocks()
{
	// まず全部解除
	m_SkillBlocks = SkillBase::Blocks{};

	// ActiveなスキルのBlocksを合成
	//ブリンク
	if (m_Blink_Skill.IsActive())
	{
		auto b = m_Blink_Skill.GetBlocks();
		m_SkillBlocks.attack |= b.attack;
		m_SkillBlocks.reload |= b.reload;
		m_SkillBlocks.weaponSwitch |= b.weaponSwitch;
		m_SkillBlocks.otherSkills |= b.otherSkills;
		m_SkillBlocks.weaponActive |= b.weaponActive;
	}

	//グレネード
	if (m_Grenade_Skill.IsActive())
	{
		auto b = m_Grenade_Skill.GetBlocks();
		m_SkillBlocks.attack |= b.attack;
		m_SkillBlocks.reload |= b.reload;
		m_SkillBlocks.weaponSwitch |= b.weaponSwitch;
		m_SkillBlocks.otherSkills |= b.otherSkills;
		m_SkillBlocks.weaponActive |= b.weaponActive;
	}

	//ウルト
	if (m_Ult_Skill.IsActive())
	{
		auto b = m_Ult_Skill.GetBlocks();
		m_SkillBlocks.attack |= b.attack;
		m_SkillBlocks.reload |= b.reload;
		m_SkillBlocks.weaponSwitch |= b.weaponSwitch;
		m_SkillBlocks.otherSkills |= b.otherSkills;
		m_SkillBlocks.weaponActive |= b.weaponActive;
	}
}

void Player::ApplySkillBlocksToWeapons()
{
	// weaponActiveがtrueの間は武器を隠す
	// SetVisibleはfalseで描画しないという意味なので逆転させる
	const bool normalVisible = !(m_SkillBlocks.weaponActive);

	for (int i = 0; i < WEAPON_MAX; ++i)
	{
		if (m_pWeapon[i])
		{
			m_pWeapon[i]->SetVisible(normalVisible);
		}
	}

	// ULT刀は別制御
	if (m_UltSword)
	{
		m_UltSword->SetVisible(m_Ult_Skill.IsActive());
	}
}

bool Player::IsWeaponSwitchBlocked() const
{
	return m_SkillBlocks.weaponSwitch;
}

bool Player::IsReloadBlocked() const
{
	return m_SkillBlocks.reload;
}

bool Player::IsOtherSkillsBlocked() const
{
	return m_SkillBlocks.otherSkills;
}

void Player::OnKillEnemy(int kill)
{
	m_Ult_Skill.OnKill(kill);
}

void Player::OnKillEnemy(KillType type)
{
	// 総Kill
	m_KillStats.total++;

	// 内訳
	switch (type)
	{
	case KillType::Pistol:  m_KillStats.pistol++;  break;
	case KillType::AR:      m_KillStats.ar++;      break;
	case KillType::Knife:   m_KillStats.knife++;   break;
	case KillType::Grenade: m_KillStats.grenade++; break;
	case KillType::ULT:     m_KillStats.ult++;     break;
	default:                break;
	}

	// ULT短縮
	OnKillEnemy(1);
}