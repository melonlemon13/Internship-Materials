#include"WeaponBase.h"

#include"GameManager.h"

using namespace DirectX::SimpleMath;

void WeaponBase::FollowCameraHandPose(float forwardDist, float rightDist, float downDist)
{
	Camera* cam = GameManager::GetInstance()->GetCamera();
	auto base = cam->GetPosition();
	auto f = cam->GetForward();
	auto r = cam->GetRight();
	auto u = cam->GetUp();

	m_Position = base + f * forwardDist + r * rightDist - u * downDist;

	auto camRot = cam->GetRotation();
	m_Rotation = { -camRot.x, camRot.y, 0.0f };

	// ===== recoil apply =====
	m_Position += m_RecoilOffset;

	// ===== recoil return =====
	m_RecoilOffset *= m_RecoilReturn;

	// ‚Ù‚Úƒ[ƒ‚É‚È‚Á‚½‚ç~‚ß‚éi”÷U“®–h~j
	if (m_RecoilOffset.LengthSquared() < 0.000001f)
	{
		m_RecoilOffset = { 0,0,0 };
	}
}

Vector3 WeaponBase::GetOwnerForwardXZ() const
{
	auto rot = m_Owner->GetRotation();
	DirectX::SimpleMath::Vector3 f(sinf(rot.y), 0.0f, cosf(rot.y));
	f.Normalize();
	return f;
}

bool WeaponBase::IsReloadBlockedBySkill() const
{
	if (!m_Owner) return false;
	return m_Owner->IsReloadBlocked();
}