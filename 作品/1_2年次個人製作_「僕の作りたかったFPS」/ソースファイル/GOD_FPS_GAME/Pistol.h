#pragma once
#include "WeaponBase.h"

class Pistol : public WeaponBase
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
	static void LoadAssets(RenderAssets& a);
private:
	//描画用
	float m_ForwardDist = 0.55f;
	float m_RightDist = 0.38f;
	float m_DownDist = 0.32f;

	float m_OffsetPitch = DirectX::XMConvertToRadians(-8.0f);
	float m_OffsetYaw = DirectX::XMConvertToRadians(85.0f);
	float m_OffsetRoll = DirectX::XMConvertToRadians(-12.0f);

	//リロード中かどうか
	bool m_IsReloading = false;

	//前フレームのRキー入力
	bool m_PrevR = false;

	//今フレーム発射したかどうか
	bool m_DidFireThisFrame = false;

public:
	void Init() override;
	void Update()  override;
	void Draw(Camera* cam)  override;
	void Uninit()  override;

	void Attack() override;

	// ロード用
	static void PreloadAssets();
};