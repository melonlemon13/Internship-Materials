#include "GameOverScene.h"

#include"input.h"
#include "GameManager.h"
#include "Texture2D.h"

static void SetDigit(Texture2D* d, int n)
{
	if (!d) return;
	if (n < 0) n = 0;
	if (n > 9) n = 9;

	// number.png: 横10分割、(n+1)列を使う想定（あなたの方式）
	d->SetUV((float)(n + 1), 1, 10, 1);
}

static void Set3Digits(Texture2D* digits[3], int value)
{
	if (value < 0) value = 0;
	if (value > 999) value = 999;

	int h = value / 100;
	int t = (value / 10) % 10;
	int o = value % 10;

	SetDigit(digits[0], h);
	SetDigit(digits[1], t);
	SetDigit(digits[2], o);
}

void GameOverScene::Init()
{
	GameManager::GetSound()->Play(SOUND_LABEL_SE_GAMEOVER);

	const auto& r = GameManager::GetInstance()->GetLastResult();

	// 背景
	m_Back = GameManager::GetInstance()->AddUIObject<Texture2D>();
	m_Back->SetTexture("assets/texture/GameOver.jpg");
	m_Back->SetPosition(0.0f, 0.0f, 0.0f);
	m_Back->SetScale(1920.0f, 1080.0f, 0.0f);
	m_UI.emplace_back(m_Back);

	m_LoseBaseY = 400.0f;
	m_LoseTime = 0.0f;

	//GameObver
	m_Lose = GameManager::GetInstance()->AddUIObject<Texture2D>();
	m_Lose->SetTexture("assets/texture/Lose.png");
	m_Lose->SetPosition(0.0f, 400.0f, 0.0f);
	m_Lose->SetScale(1120.0f, 630.0f, 0.0f);
	m_UI.emplace_back(m_Lose);

	float y = BASE_Y;

	auto MakeDigits = [&](Texture2D* out[3])
		{
			const float startX = RIGHT_X - DIGIT_STEP * 2.0f;

			for (int i = 0; i < 3; i++)
			{
				Texture2D* d = GameManager::GetInstance()->AddUIObject<Texture2D>();
				d->SetTexture("assets/texture/number.png");

				float x = startX + i * DIGIT_STEP;
				d->SetPosition(x, y, 0.0f);
				d->SetScale(DIGIT_SCALE, DIGIT_SCALE, 0.0f);

				out[i] = d;
				m_UI.emplace_back(d);
			}
		};

	// Total
	MakeDigits(m_Total);
	y += LINE_H;

	// Pistol
	MakeDigits(m_Pistol);
	y += LINE_H;

	// AR
	MakeDigits(m_AR);
	y += LINE_H;

	// Knife
	MakeDigits(m_Knife);
	y += LINE_H;

	// Grenade
	MakeDigits(m_Grenade);
	y += LINE_H;

	// ULT
	MakeDigits(m_ULT);
	y += LINE_H;

	// Survival
	MakeDigits(m_Survival);

	Set3Digits(m_Total, r.totalKills);
	Set3Digits(m_Pistol, r.pistolKills);
	Set3Digits(m_AR, r.arKills);
	Set3Digits(m_Knife, r.knifeKills);
	Set3Digits(m_Grenade, r.grenadeKills);
	Set3Digits(m_ULT, r.ultKills);
	Set3Digits(m_Survival, r.survivalFrames / 60);
}

void GameOverScene::Update()
{
	m_LoseTime += 1.0f / 60.0f;

	float offset = sinf(m_LoseTime) * 20.0f;
	m_Lose->SetPosition(0.0f, m_LoseBaseY + offset, 0.0f);

	// Enterでタイトルへ
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		GameManager::GetInstance()->ChangeScene(TITLE);
		return;
	}

	// Rでリトライ（必要なら）
	if (Input::GetKeyTrigger(VK_R))
	{
		GameManager::GetInstance()->ChangeScene(STAGE);
		return;
	}
}

void GameOverScene::Uninit()
{
	for (auto* ui : m_UI)
	{
		if (!ui) continue;
		GameManager::GetInstance()->DeleteObject(ui);
	}
	m_UI.clear();

	m_Back = nullptr;
}