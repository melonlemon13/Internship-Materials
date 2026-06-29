#pragma once
#include "SkillBase.h"
#include <vector>

class Player;
class Enemy;

class GrenadeSkill : public SkillBase
{
private:
	Player* m_Owner = nullptr;

    // 敵リスト
    std::vector<Enemy*>* m_Targets = nullptr;
    
    // プレイヤー速度の継承率
    float m_InheritRate = 1.2f;

    // グレネードの生成に必要な固有パラメータ
	// 発射位置のオフセット
    float m_SpawnForwardDist = 6.0f;
	// 発射位置の上方向オフセット
    float m_SpawnUpDist = 12.0f;
	// 発射位置の右方向オフセット
	float m_SpawnRightDist = 10.0f;

	// 発射初速
    float m_ThrowXZSpeed = 3.0f; // cm/frame
	// 発射上方向速度
    float m_ThrowYVel = 1.0f; // cm/frame

	// 重力加速度
    float m_Gravity = -2.0f;
	// 爆発までのフレーム数
    int   m_ExplodeFrames = 75;    // frames
	// 爆発半径
    float m_Radius = 150.0f;

public:
    void Init(Player* owner);

    // 発動したらtrueを返す
    bool TryActivate();

    void Update();

    // グレネード生成処理
    void SpawnGrenade();

    //StageSceneから敵リスト取得し、セット
    void SetTargets(std::vector<Enemy*>* targets) { m_Targets = targets; }
};

