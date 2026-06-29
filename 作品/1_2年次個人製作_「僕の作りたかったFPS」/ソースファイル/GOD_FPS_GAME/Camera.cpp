#include "Renderer.h"
#include "Camera.h"
#include "Application.h"
#include"Input.h"
#include <algorithm>
#include "Wall.h"
#include "WallCollider.h"

using namespace DirectX::SimpleMath;



static float ClampF(float v, float a, float b)
{
	return (v < a) ? a : (v > b) ? b : v;
}

//=======================================
//初期化処理
//=======================================
void Camera::Init()
{
	m_Position = Vector3(0.0f, 20.0f, -50.0f);
	m_Target = Vector3(0.0f, 0.0f, 0.0f);
}


//=======================================
//更新処理
//=======================================
void Camera::Update()
{
	//左右キーでカメラを回転
	if (Input::GetKeyPress(VK_LEFT))
	{
		m_CameraDirection += 0.02f;
	}
	if (Input::GetKeyPress(VK_RIGHT))
	{
		m_CameraDirection -= 0.02f;
	}

	//カメラの位置を更新
	Vector3 pos = m_Target;
	pos.x += sin(m_CameraDirection) * 50;
	pos.y += 20;
	pos.z -= cos(m_CameraDirection);
	m_Position = pos;
}

//=======================================
//描画処理
//=======================================
void Camera::SetCamera(int mode)
{
	//3D
	if (mode == 0)
	{
	// ビュー変換後列作成
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_Position, m_Target, up); //左手系

	Renderer::SetViewMatrix(&m_ViewMatrix);

	//プロジェクション行列の生成
	constexpr float fieldOfView = DirectX::XMConvertToRadians(110.0f);    // 視野角
	
	float aspectRatio = static_cast<float>(Application::GetWidth()) / static_cast<float>(Application::GetHeight());	// アスペクト比	
	float nearPlane = 0.2f;       // ニアクリップ
	float farPlane = 20000.0f;      // ファークリップ

	//プロジェクション行列の生成
	Matrix projectionMatrix;
	projectionMatrix =  DirectX::XMMatrixPerspectiveFovLH( fieldOfView,  aspectRatio,  nearPlane,  farPlane);	//左手系

	Renderer::SetProjectionMatrix(&projectionMatrix);
	}
	//2D
	else if(mode == 1)
	{
		// ビュー変換後列作成
		Vector3 pos = { 0.0f,0.0f,-10.0f };
		Vector3 tgt = { 0.0f,0.0f,1.0f };
		Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
		m_ViewMatrix = DirectX::XMMatrixLookAtLH(pos,tgt, up); //左手系
		Renderer::SetViewMatrix(&m_ViewMatrix);

		//プロジェクション行列の生成
		float nearPlane = 0.05f;//ニアクリップ
		float farPlane = 1000.0f;//ファークリップ
		Matrix projectionMatrix = DirectX::XMMatrixOrthographicLH(static_cast<float>(Application::GetWidth()),
			static_cast<float>(Application::GetHeight()), nearPlane, farPlane);

		projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);
		Renderer::SetProjectionMatrix(&projectionMatrix);
	}
}


//=======================================
//終了処理
//=======================================
void Camera::Uninit()
{

}

Vector3 Camera::GetRight() const
{
	Vector3 f = GetForward();
	Vector3 upWorld(0.0f, 1.0f, 0.0f);

	//カメラの右
	Vector3 r = upWorld.Cross(f);
	if (r.LengthSquared() > 0.0001f) r.Normalize();
	return r;
}

Vector3 Camera::GetUp() const
{
	Vector3 f = GetForward();
	Vector3 r = GetRight();

	//カメラの上
	Vector3 u = f.Cross(r);
	if (u.LengthSquared() > 0.0001f) u.Normalize();
	return u;
}

void Camera::SetTarget(Vector3 target)
{
	//カメラの注視点を更新
	m_Target = target;
}

void Camera::UpdateFPS(const Vector3& playerPos,
	const Vector3& playerRot,
	const std::vector<WallCollider>& walls,
	float playerRadius)
{
	//プレイヤーの頭の位置
	const float EyeHeight = 17.0f;

	//カメラ位置を設定
	m_Position = playerPos + Vector3(0.0f, EyeHeight, 0.0f);

	ResolveCircleVsWallsXZ(m_Position, playerRadius, walls);

	//プレイヤーの回転からforward(前方)を作る
	Vector3 forward;
	forward.x = cosf(playerRot.x) * sinf(playerRot.y);
	forward.y = sinf(playerRot.x);
	forward.z = cosf(playerRot.x) * cosf(playerRot.y);
	forward.Normalize();

	//注視点,プレイヤーの頭の少し先に設定
	//これでプレイヤーがゲーム画面に映ることはない
	m_Target = m_Position + forward;

	m_Rotation = playerRot;
}

void Camera::ResolveCircleVsWallsXZ(
	DirectX::SimpleMath::Vector3& pos,
	float radius,
	const std::vector<WallCollider>& walls
)
{
	for (const auto& w : walls)
	{
		float minX = w.center.x - w.half.x;
		float maxX = w.center.x + w.half.x;
		float minZ = w.center.z - w.half.z;
		float maxZ = w.center.z + w.half.z;

		float cx = ClampF(pos.x, minX, maxX);
		float cz = ClampF(pos.z, minZ, maxZ);

		float dx = pos.x - cx;
		float dz = pos.z - cz;

		float d2 = dx * dx + dz * dz;
		float r2 = radius * radius;

		if (d2 < r2)
		{
			if (d2 < 0.000001f)
			{
				float left = fabsf(pos.x - minX);
				float right = fabsf(maxX - pos.x);
				float back = fabsf(pos.z - minZ);
				float front = fabsf(maxZ - pos.z);

				float m = (std::min)((std::min)(left, right), (std::min)(back, front));
				if (m == left)       pos.x = minX - radius;
				else if (m == right) pos.x = maxX + radius;
				else if (m == back)  pos.z = minZ - radius;
				else                 pos.z = maxZ + radius;

				continue;
			}

			float d = sqrtf(d2);
			float nx = dx / d;
			float nz = dz / d;

			float push = radius - d;
			pos.x += nx * push;
			pos.z += nz * push;
		}
	}
}

Vector3 Camera::GetForward() const
{
	Vector3 f = m_Target - m_Position;
	f.Normalize();
	return f;
}
