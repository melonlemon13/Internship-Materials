#pragma once
#include "SceneBase.h"

class TitleScene : public SceneBase
{
private:
	std::vector<ObjectBase*> m_MySceneObjects;
public:
	TitleScene() {};
	~TitleScene() {};

	void Update() override;
	void Init() override;
	void Uninit() override;
};

