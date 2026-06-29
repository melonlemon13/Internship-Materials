#pragma once
#include "SceneBase.h"
#include <vector>
#include"Texture2D.h"

class ResultScene : public SceneBase
{
private:
	// 生成したUIをまとめて管理（Uninitで一括削除）
	std::vector<Texture2D*> m_UI;

	// 背景
	Texture2D* m_Back = nullptr;

	Texture2D* m_Rank = nullptr;

	// CLEARロゴ（任意：無くてもOK）
	Texture2D* m_Clear = nullptr;

	// Kill数
	Texture2D* m_Total[3]{};
	Texture2D* m_Pistol[3]{};
	Texture2D* m_AR[3]{};
	Texture2D* m_Knife[3]{};
	Texture2D* m_Grenade[3]{};
	Texture2D* m_ULT[3]{};

	// ---- レイアウト調整（あなたが今使ってる値をそのまま）----
	static constexpr float DIGIT_SCALE = 85.0f;  // 数字サイズ
	static constexpr float DIGIT_STEP = 78.0f;  // 桁の間隔

	static constexpr float RIGHT_X = 420.0f;     // 数字のX（列の開始）
	static constexpr float BASE_Y = 110.0f;     // Total行のY
	static constexpr float LINE_H = -95.0f;     // 行間（下に行くのでマイナス）

	// CLEARロゴ演出用（上下ふわふわ）
	float m_Time = 0.0f;
	float m_ClearBaseY = 400.0f;

public:
	ResultScene() {}
	~ResultScene() {}

	void Init() override;
	void Update() override;
	void Uninit() override;
};

