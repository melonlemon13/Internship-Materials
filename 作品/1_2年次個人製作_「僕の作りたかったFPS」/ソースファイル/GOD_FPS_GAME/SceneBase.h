#pragma once
#include <memory>
#include"ObjectBase.h"
//形上継承するだけで中身のないクラス
class SceneBase
{
public:
	SceneBase() {/*処理なし*/ };
	~SceneBase() {/*処理なし*/ };

	virtual void Update() = 0;
	virtual void Init() = 0;
	virtual void Uninit() = 0;
};