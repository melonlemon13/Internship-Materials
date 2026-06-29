#include "TitleScene.h"
#include"input.h"
#include "GameManager.h"
#include "Texture2D.h"

void TitleScene::Init()
{
	//背景画像オブジェクトを作成
	Texture2D* pt = GameManager::GetInstance()->AddUIObject<Texture2D>();
	pt->SetTexture("assets/texture/Title.png");//画像を指定
	pt->SetScale(1920.0f, 1080.0f, 0.0f);//大きさを指定
	m_MySceneObjects.emplace_back(pt);
}

void TitleScene::Uninit()
{

}

void TitleScene::Update()
{
	if (Input::GetKeyTrigger(VK_LBUTTON))
	{
		GameManager::GetSound()->Play(SOUND_LABEL_SE_CLICK);
		GameManager::GetInstance()->ChangeScene(STAGE);
	}
}
