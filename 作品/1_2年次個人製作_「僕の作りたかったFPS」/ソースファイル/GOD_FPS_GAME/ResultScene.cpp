#include "ResultScene.h"
#include"input.h"
#include "GameManager.h"
#include "Texture2D.h"

static char CalcRank(int totalKills)
{
	if (totalKills >= 60) return 'S';
	if (totalKills >= 50) return 'A';
	if (totalKills >= 40) return 'B';
	return 'C';
}

static void SetDigit(Texture2D* d, int n)
{
	if (!d) return;
	if (n < 0) n = 0;
	if (n > 9) n = 9;

	// number.png: 横10分割、(n+1)列を使う想定
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

void ResultScene::Init()
{
	GameManager::GetSound()->Play(SOUND_LABEL_SE_WIN);

	const auto& r = GameManager::GetInstance()->GetLastResult();

	// 背景（指定画像）
	m_Back = GameManager::GetInstance()->AddUIObject<Texture2D>();
	m_Back->SetTexture("assets/texture/GameClear.jpg");
	m_Back->SetPosition(0.0f, 0.0f, 0.0f);
	m_Back->SetScale(1920.0f, 1080.0f, 0.0f);
	m_UI.emplace_back(m_Back);

	// CLEARロゴ（任意：ファイルがあるなら表示）
	// 無いなら、このブロック丸ごと消してOK
	{
		m_Clear = GameManager::GetInstance()->AddUIObject<Texture2D>();
		m_Clear->SetTexture("assets/texture/Win.png"); 
		m_Clear->SetPosition(0.0f, m_ClearBaseY, 0.0f);
		m_Clear->SetScale(1120.0f, 630.0f, 0.0f);
		m_UI.emplace_back(m_Clear);
	}

	float y = BASE_Y;

	auto MakeDigits = [&](Texture2D* out[3])
		{
			for (int i = 0; i < 3; i++)
			{
				Texture2D* d = GameManager::GetInstance()->AddUIObject<Texture2D>();
				d->SetTexture("assets/texture/number.png");

				float x = RIGHT_X + i * DIGIT_STEP;
				d->SetPosition(x, y, 0.0f);
				d->SetScale(DIGIT_SCALE, DIGIT_SCALE, 0.0f);

				out[i] = d;
				m_UI.emplace_back(d);
			}
		};

	// 表示行の生成（順番は画像の左側ラベルに合わせて）
	MakeDigits(m_Total);   y += LINE_H;
	MakeDigits(m_Pistol);  y += LINE_H;
	MakeDigits(m_AR);      y += LINE_H;
	MakeDigits(m_Knife);   y += LINE_H;
	MakeDigits(m_Grenade); y += LINE_H;
	MakeDigits(m_ULT);     y += LINE_H;

	// ★ここが本番の反映
	Set3Digits(m_Total, r.totalKills);
	Set3Digits(m_Pistol, r.pistolKills);
	Set3Digits(m_AR, r.arKills);
	Set3Digits(m_Knife, r.knifeKills);
	Set3Digits(m_Grenade, r.grenadeKills);
	Set3Digits(m_ULT, r.ultKills);

	char rank = CalcRank(r.totalKills);

	const char* tex = nullptr;
	switch (rank)
	{
	case 'S': tex = "assets/texture/S.png"; break;
	case 'A': tex = "assets/texture/A.png"; break;
	case 'B': tex = "assets/texture/B.png"; break;
	default:  tex = "assets/texture/C.png"; break;
	}

	m_Rank = GameManager::GetInstance()->AddUIObject<Texture2D>();
	m_Rank->SetTexture(tex);
	m_Rank->SetPosition(760.0f, -360.0f, 0.0f);
	m_Rank->SetScale(420.0f, 420.0f, 0.0f);

	m_UI.emplace_back(m_Rank);


	// 演出タイマー初期化
	m_Time = 0.0f;
}

void ResultScene::Update()
{
	// ふわふわ（CLEARロゴ）
	if (m_Clear)
	{
		m_Time += 1.0f / 60.0f;          // 60fps想定
		float dy = sinf(m_Time * 2.0f) * 10.0f; // 振幅10px、速度は2.0fで調整
		m_Clear->SetPosition(0.0f, m_ClearBaseY + dy, 0.0f);
	}

	// Enterでタイトルへ
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		GameManager::GetInstance()->ChangeScene(TITLE);
		return;
	}

	// Rでリトライ
	if (Input::GetKeyTrigger(VK_R))
	{
		GameManager::GetInstance()->ChangeScene(STAGE);
		return;
	}
}

void ResultScene::Uninit()
{
	for (auto* ui : m_UI)
	{
		if (!ui) continue;
		GameManager::GetInstance()->DeleteObject(ui);
	}
	m_UI.clear();

	m_Back = nullptr;
	m_Clear = nullptr;
	m_UI.emplace_back(m_Rank);
}
