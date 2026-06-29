#pragma once
#include "WeaponBase.h"

class Knife : public WeaponBase
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
	//ナイフは近距離武器なので攻撃範囲を設定
	//距離
	float m_Distance = 15.0f;
	//範囲,正面方向に扇形に
	float m_Range = 35.0f;

	//リロード中かどうか
	//ナイフでは一度攻撃するたびにリロード扱いにする
	bool m_IsReloading = false;

	const float forwardDist = 0.60f;
	const float rightDist = 0.48f;
	const float downDist = 0.30f;

public:
	static void PreloadAssets();

	void Init() override;
	void Update()  override;
	void Draw(Camera* cam)  override;
	void Uninit()  override;

	void Attack() override;
};

