#pragma once
#include <SimpleMath.h>

struct WallCollider
{
    DirectX::SimpleMath::Vector3 center; // 中心
    DirectX::SimpleMath::Vector3 half;   // 半サイズ
};
