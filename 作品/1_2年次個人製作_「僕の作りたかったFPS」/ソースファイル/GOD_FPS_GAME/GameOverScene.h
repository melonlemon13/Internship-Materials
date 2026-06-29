#pragma once
#include "SceneBase.h"
#include <vector>
#include"Texture2D.h"

class GameOverScene : public SceneBase
{
private:
	// 生成したUIをまとめて管理（Uninitで一括削除用）
	std::vector<Texture2D*> m_UI;

	// 背景
	Texture2D* m_Back = nullptr;
	Texture2D* m_Lose = nullptr;

	// 生存時間（秒） 3桁
	Texture2D* m_Survival[3]{};

	// Kill数
	Texture2D* m_Total[3]{};
	Texture2D* m_Pistol[3]{};
	Texture2D* m_AR[3]{};
	Texture2D* m_Knife[3]{};
	Texture2D* m_Grenade[3]{};
	Texture2D* m_ULT[3]{};

	static constexpr float DIGIT_SCALE = 85.0f;
	static constexpr float DIGIT_STEP = 80.0f;
	static constexpr float RIGHT_X = 360.0f;
	static constexpr float BASE_Y = 120.0f;
	static constexpr float LINE_H = -95.0f; 

	float m_LoseBaseY = 0.0f;
	float m_LoseTime = 0.0f;

public:
	GameOverScene() {}
	~GameOverScene() {}

	void Init() override;
	void Update() override;
	void Uninit() override;
};

