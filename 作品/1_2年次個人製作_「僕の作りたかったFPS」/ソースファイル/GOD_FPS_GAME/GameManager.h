#pragma once
#include <iostream>

#include"Renderer.h"
#include"TitleScene.h"
#include"StageScene.h"
#include"ResultScene.h"
#include"GameOverScene.h"
#include "Scene.h"
#include "ObjectBase.h"
#include "Player.h"
#include "Enemy.h"
#include "Assaultrifle.h"
#include "Pistol.h"
#include "sound.h"
#include"Grenade.h"

#include "PlayResultData.h"

class GameManager
{
private:
	GameManager(); // コンストラクタ

	static GameManager* m_Instance;//ゲームインスタンス
	SceneBase* m_Scene;//シーン

	// カメラ
	static Camera  m_Camera;
	// 3Dオブジェクト配列
	std::vector<std::unique_ptr<ObjectBase>> m_3DObjects;
	// UIオブジェクト配列
	std::vector<std::unique_ptr<ObjectBase>> m_UIObjects;
	// 生成キュー
	std::vector<std::unique_ptr<ObjectBase>> m_SpawnQueue;
	std::vector<std::unique_ptr<ObjectBase>> m_UISpawnQueue;
	// サウンド
	static Sound m_Sound;

	//削除キュー
	std::vector<ObjectBase*> m_DeleteQueue;

	// オブジェクト更新中フラグ
	bool m_IsUpdatingObjects = false;

	PlayResultData m_LastResult{};

public:
	~GameManager(); // デストラクタ

	static void Init(); // 初期化
	static void Update(); // 更新
	static void Draw(); // 描画
	static void Uninit(); // 終了処理
	static GameManager* GetInstance();
	void ChangeScene(SceneName sName);//シーンを変更
	void DeleteObject(ObjectBase* pt);//オブジェクトを削除する
	void DeleteAllObject();//オブジェクトをすべて削除する
	void ClearObjects();
	static Camera* GetCamera() {return &m_Camera;};
	static Sound* GetSound() { return &m_Sound; }

	void SetLastResult(const PlayResultData& r) { m_LastResult = r; }
	const PlayResultData& GetLastResult() const { return m_LastResult; }

	//Spawnキューを反映
	void FlushSpawnQueue();
	//deleteキューを反映
	void ApplyDeleteQueue();
	template<typename T>T* AddObject()
	{
		T* pt = new T;

		// m_Objects走査中だけ遅延
		if (m_Instance->m_IsUpdatingObjects)
		{
			m_Instance->m_SpawnQueue.emplace_back(pt);
			return pt;
		}

		// それ以外は即追加＋Init
		m_Instance->m_3DObjects.emplace_back(pt);
		pt->Init();
		return pt;
	}
	template<typename T>T* AddUIObject()
	{
		T* pt = new T;

		// m_Objects走査中だけ遅延
		if (m_Instance->m_IsUpdatingObjects)
		{
			m_Instance->m_UISpawnQueue.emplace_back(pt);
			return pt;
		}

		// それ以外は即追加＋Init
		m_Instance->m_UIObjects.emplace_back(pt);
		pt->Init();
		return pt;
	}

	//オブジェクトを取得する
	template<typename T>std::vector<T*> GetObjects()
	{
		std::vector<T*> res;
		for (auto& o : m_Instance->m_3DObjects)
		{
			//dynamic_castで型をチェック
			if (T* derivedObj = dynamic_cast<T*>(o.get()))
			{
				res.emplace_back(derivedObj);
			}
		}
		return res;
	}

	//オブジェクトの位置を取得
	template<typename T>
	std::vector<DirectX::SimpleMath::Vector3> GetPositions()
	{
		std::vector<DirectX::SimpleMath::Vector3> result;
		for (auto& obj : m_3DObjects)
		{
			if (T* d = dynamic_cast<T*>(obj.get()))
				result.emplace_back(d->GetPosition());
		}
		return result;
	}
};
