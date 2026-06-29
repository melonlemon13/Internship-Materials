#pragma once

#include	<SimpleMath.h>

struct WallCollider;

//-----------------------------------------------------------------------------
//Cameraクラス
//-----------------------------------------------------------------------------
class Camera {
private:
	DirectX::SimpleMath::Vector3 m_Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3 m_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3 m_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

	DirectX::SimpleMath::Vector3 m_Target{};
	DirectX::SimpleMath::Matrix	 m_ViewMatrix{};

	float m_CameraDirection = 0;//カメラの方向

	//マウス視点移動用の変数
	float m_Yaw = 0.0f;
	float m_Pitch = 0.0f;

public:
	void Init();
	void Update();
	void SetCamera(int mode = 0);
	void Uninit();

	//プレイヤーの右下に武器を追従させるための関数
	//カメラ基準で画面の右方向を返す
	DirectX::SimpleMath::Vector3 GetRight() const;
	//カメラ基準で画面の右方向を返す
	DirectX::SimpleMath::Vector3 GetUp() const;
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_Position; }
	DirectX::SimpleMath::Vector3 GetRotation() const { return m_Rotation; }

	//FPSゲーム用に新たなUpdateを作成
	void UpdateFPS(const DirectX::SimpleMath::Vector3& playerPos,
		const DirectX::SimpleMath::Vector3& playerRot,
		const std::vector<WallCollider>& walls,
		float playerRadius);
	static void ResolveCircleVsWallsXZ(
		DirectX::SimpleMath::Vector3& pos,
		float radius,
		const std::vector<WallCollider>& walls
	);
	DirectX::SimpleMath::Vector3 GetForward() const;

	void SetTarget(DirectX::SimpleMath::Vector3 target);//カメラターゲットを設定
};