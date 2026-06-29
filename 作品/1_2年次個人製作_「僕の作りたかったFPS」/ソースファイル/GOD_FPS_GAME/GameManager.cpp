#include "GameManager.h"
#include "Renderer.h"
#include"input.h"

GameManager* GameManager::m_Instance;//ゲームインスタンス
Camera GameManager::m_Camera;
Sound GameManager::m_Sound;

// コンストラクタ
GameManager::GameManager()
{
	m_Scene = nullptr;
}

// デストラクタ
GameManager::~GameManager()
{
	delete m_Scene;
	DeleteAllObject();
}

// 初期化
void GameManager::Init()
{
	//インスタンス作成
	m_Instance = new GameManager;
	
	// 描画終了処理
	Renderer::Init();

	//入力処理初期化
	Input::Create();

	// サウンド初期化
	m_Sound.Init();

	// カメラ初期化
	m_Instance->m_Camera.Init();

	//最初のシーンを読み込む
	m_Instance->m_Scene = new TitleScene;
	m_Instance->m_Scene->Init();
}

// 更新
void GameManager::Update()
{
	//入力処理更新
	Input::Update();

	//シーン更新
	m_Instance->m_Scene->Update();

	//3Dオブジェクトを更新
	m_Instance->m_IsUpdatingObjects = true;
	for (auto& obj : m_Instance->m_3DObjects)
	{
		if (!obj) continue;
		obj->Update();
	}

	// UIオブジェクト更新
	for (auto& obj : m_Instance->m_UIObjects)
	{
		if (!obj) continue;
		obj->Update();
	}

	m_Instance->m_IsUpdatingObjects = false;

	// ここで削除キューを反映
	m_Instance->ApplyDeleteQueue();

	// 最後に生成キューを反映
	m_Instance->FlushSpawnQueue();
}

// 描画
void GameManager::Draw()
{
	// 描画前処理
	Renderer::DrawStart();

	// 3D
	m_Instance->m_Camera.SetCamera(0);
	for (auto& o : m_Instance->m_3DObjects)
	{
		//std::cout << o << std::endl;
		if (!o) continue;
		o->Draw(&m_Instance->m_Camera);
	}

	// UI(2D)
	m_Instance->m_Camera.SetCamera(1);
	for (auto& o : m_Instance->m_UIObjects)
	{
		//std::cout << o << std::endl;
		if (!o) continue;
		o->Draw(&m_Instance->m_Camera);
	}

	// 描画後処理
	Renderer::DrawEnd();
}

// 終了処理
void GameManager::Uninit()
{
	// カメラ終了処理
	m_Instance->m_Camera.Uninit();

	//入力処理終了
	Input::Release();

	// 描画終了処理
	Renderer::Uninit();

	//サウンド終了処理
	m_Sound.Uninit();

	//インスタンス削除
	delete m_Instance;
}

//インスタンスを取得
GameManager* GameManager::GetInstance()
{
	return m_Instance;
}

//シーンを切り替える
void GameManager::ChangeScene(SceneName sName)
{
	//読み込み済みのシーンがあれば削除
	if (m_Instance->m_Scene != nullptr)
	{
		m_Instance->m_Scene->Uninit();
		delete m_Instance->m_Scene;
		m_Instance->m_Scene = nullptr;
	}
	m_Instance->DeleteAllObject();
	m_Instance->m_DeleteQueue.clear();

	switch (sName)
	{
	case TITLE:
		m_Instance->m_Scene = new TitleScene;//メモリを確保
		break;
	case STAGE:
		m_Instance->m_Scene = new StageScene;//メモリを確保
		break;
	case RESULT:
		m_Instance->m_Scene = new ResultScene;//メモリを確保
		break;
	case GAMEOVER:
		m_Instance->m_Scene = new GameOverScene;//メモリを確保
		break;
	}

	if (m_Instance->m_Scene)
	{
		m_Instance->m_Scene->Init();
		// シーン初期化でAddObjectされたものを全て反映＆Initする
		m_Instance->FlushSpawnQueue();
	}
}

//オブジェクトを削除する
void GameManager::DeleteObject(ObjectBase* pt)
{
	m_Instance->m_DeleteQueue.push_back(pt);
}

//オブジェクトをすべて削除する
void GameManager::DeleteAllObject()
{
	//オブジェクト終了処理
	//3Dオブジェクト
	for (auto& o : m_Instance->m_3DObjects)
	{
		o->Uninit();
	}

	//2Dオブジェクト
	for (auto& o : m_Instance->m_UIObjects)
	{
		o->Uninit();
	}

	m_Instance->m_3DObjects.clear();//すべて削除
	m_Instance->m_UIObjects.clear();//すべて削除
	m_Instance->m_3DObjects.shrink_to_fit();
	m_Instance->m_UIObjects.shrink_to_fit();

	m_Instance->m_SpawnQueue.clear();
	m_Instance->m_UISpawnQueue.clear();
}

void GameManager::ClearObjects()
{
	for (auto& obj : m_Instance->m_3DObjects)
	{
		if (obj) obj->Uninit();
	}
	m_Instance->m_3DObjects.clear();
}

void GameManager::FlushSpawnQueue()
{
	// Init中にAddObjectされても安全にするため、
	// 「今溜まっている分」をローカルへ退避してから処理する。
	// Init中に増えた分は m_SpawnQueue に新しく溜まるので、空になるまで繰り返す。

	while (!m_Instance->m_SpawnQueue.empty())
	{
		// いま溜まっている分をローカルに退避（これ以降 m_SpawnQueue は空）
		std::vector<std::unique_ptr<ObjectBase>> batch;
		batch.swap(m_Instance->m_SpawnQueue);

		// batch を m_Objects に移しつつ Init
		for (auto& up : batch)
		{
			if (!up) continue;

			m_Instance->m_3DObjects.emplace_back(std::move(up));

			ObjectBase* raw = m_Instance->m_3DObjects.back().get();
			if (!raw) continue;

			raw->Init();
		}

		// batch はここで破棄される（中身は全て move 済み）
	}

	while (!m_Instance->m_UISpawnQueue.empty())
	{
		// いま溜まっている分をローカルに退避（これ以降 m_UISpawnQueue は空）
		std::vector<std::unique_ptr<ObjectBase>> batch;
		batch.swap(m_Instance->m_UISpawnQueue);

		// batch を m_UIObjects に移しつつ Init
		for (auto& up : batch)
		{
			if (!up) continue;

			m_Instance->m_UIObjects.emplace_back(std::move(up));

			ObjectBase* raw = m_Instance->m_UIObjects.back().get();
			if (!raw) continue;

			raw->Init();
		}

		// batch はここで破棄される（中身は全て move 済み）
	}
}

void GameManager::ApplyDeleteQueue()
{
	/*
	begin / end	配列の範囲指定
	remove_if	消す候補を後ろに集める（まだ消さない）
	erase	実際に消す
	erase + remove_if	vector 削除の安全な唯一の正解*/
	if (m_DeleteQueue.empty()) return;

	// 1つずつ消す
	for (ObjectBase* del : m_DeleteQueue)
	{
		if (!del) continue;

		// 3Dオブジェクト配列から削除
		{
			auto& objs = m_3DObjects;
			objs.erase(
				std::remove_if(
					objs.begin(),
					objs.end(),
					[del](const std::unique_ptr<ObjectBase>& o)
					{
						if (!o) return true;
						if (o.get() != del) return false;

						// 消す直前に終了処理
						o->Uninit();
						return true;
					}
				),
				objs.end()
			);
		}

		// UIオブジェクト配列から削除
		{
			auto& objs = m_UIObjects;
			objs.erase(
				std::remove_if(
					objs.begin(),
					objs.end(),
					[del](const std::unique_ptr<ObjectBase>& o)
					{
						if (!o) return true;
						if (o.get() != del) return false;

						o->Uninit();
						return true;
					}
				),
				objs.end()
			);
		}
	}

	// 削除依頼を処理し終わったのでクリア
	m_DeleteQueue.clear();
}