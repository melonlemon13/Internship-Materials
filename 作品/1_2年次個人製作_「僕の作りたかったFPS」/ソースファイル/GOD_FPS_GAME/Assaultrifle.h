#pragma once
#include "WeaponBase.h"

class Assaultrifle : public WeaponBase
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
	float m_ForwardDist = 1.3f;
	float m_RightDist = 1.1f;
	float m_DownDist = 0.9f;

	//前フレームのRキー入力
	bool m_PrevR = false;

	//リロード中かどうか
	bool m_IsReloading = false;

	// 拡散関連
	// 現在の追加拡散
	float m_SpreadDeg = 0.0f;
	// 1発撃つごとに増える量（度）
	float m_SpreadAddPerShot = 0.25f;    
	// 撃ってない時に戻る量（度/フレーム）
	float m_SpreadRecoverPerFrame = 0.08f;
	// 上限（度）
	float m_SpreadMaxDeg = 7.0f;          
	// そのフレームに実際に発射できたか
	bool  m_DidFireThisFrame = false;     


public:
	static void PreloadAssets();
	void Init() override;
	void Update()  override;
	void Draw(Camera* cam)  override;
	void Uninit()  override;

	void Attack() override;
};

