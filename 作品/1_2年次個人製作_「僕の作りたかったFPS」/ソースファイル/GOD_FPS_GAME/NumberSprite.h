#pragma once

#include <vector>
#include <SimpleMath.h>
#include "Texture2D.h"

class NumberSprite
{
public:
    // texturePath: digits.png のパス
    // maxDigits  : 最大桁数（例：3なら 0～999 まで）
    // splitX/Y   : スプライトシート分割数（0～9なら splitX=10, splitY=1）
    bool Init(const char* texturePath, int maxDigits, float splitX = 10.0f, float splitY = 1.0f);

    // 値セット（0以上を想定。負なら0に丸めます）
    void SetNumber(int value);

    // 見た目設定
    void SetBasePos(const DirectX::SimpleMath::Vector3& pos) { m_BasePos = pos; m_Dirty = true; }
    void SetSpacing(float s) { m_Spacing = s; m_Dirty = true; }
    void SetScale(const DirectX::SimpleMath::Vector3& scl);

    // 先頭ゼロを表示するか（例：007）
    void SetLeadingZeros(bool enable) { m_LeadingZeros = enable; m_Dirty = true; }

    // 最大桁数（変更したら内部のTexture2D枚数も作り直し）
    bool SetMaxDigits(int maxDigits);

    // 更新・描画
    void Update();           // 値が変わったらUV/位置を更新
    void Draw(Camera* cam);  // 内部のTexture2Dを順に描画

private:
    void RebuildTextures();
    void RefreshDigits(); // UV/位置更新

private:
    std::vector<Texture2D> m_Digits; // 左→右
    int   m_MaxDigits = 0;

    int   m_Value = 0;
    int   m_LastValue = -1;

    bool  m_LeadingZeros = false;
    bool  m_Inited = false;
    bool  m_Dirty = true;

    float m_SplitX = 10.0f;
    float m_SplitY = 1.0f;

    DirectX::SimpleMath::Vector3 m_BasePos = { -8.5f, 4.5f, 0.0f };
    float m_Spacing = 0.8f;
    DirectX::SimpleMath::Vector3 m_Scale = { 0.7f, 0.7f, 1.0f };

    std::string m_TexturePath;

    DirectX::SimpleMath::Vector3 m_HidePos = { 9999.0f, 9999.0f, 0.0f };
};

