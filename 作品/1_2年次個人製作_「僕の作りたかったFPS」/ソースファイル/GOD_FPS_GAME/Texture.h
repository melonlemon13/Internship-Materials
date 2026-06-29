#pragma once

#include	<d3d11.h>
#include	<string>
#include	<wrl/client.h> // ComPtrの定義を含むヘッダファイル
#include	<filesystem>

using Microsoft::WRL::ComPtr;

//-----------------------------------------------------------------------------
//Textureクラス
//-----------------------------------------------------------------------------
class Texture
{
	std::string m_texname{}; // ファイル名
	ComPtr<ID3D11ShaderResourceView> m_srv{}; // シェーダーリソースビュー

	int m_width; // 幅
	int m_height; // 高さ
	int m_bpp; // BPP
public:
	bool Load(const std::string& filename);
	bool LoadFromFemory(const unsigned char* data,int len);

	// 1x1の単色テクスチャを作る
	bool CreateSolid1x1RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	// 白テクスチャを返す
	static Texture& White();


	void SetGPU();
};