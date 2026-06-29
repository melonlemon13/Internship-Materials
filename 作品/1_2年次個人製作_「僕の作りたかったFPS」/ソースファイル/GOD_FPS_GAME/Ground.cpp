#include "Ground.h"
#include"stb_image.h"
#include <iostream>
#include "GameManager.h"

using namespace DirectX::SimpleMath;

//=======================================
//初期化処理
//=======================================
void Ground::Init()
{
	//床を1枚の四角形で作成

	float W = 1200.0f;
	float D = 1200.0f;

	m_Vertices.resize(4);

	m_Vertices[0].position = { -W * 0.5f, 0.0f, +D * 0.5f };
	m_Vertices[1].position = { +W * 0.5f, 0.0f, +D * 0.5f };
	m_Vertices[2].position = { -W * 0.5f, 0.0f, -D * 0.5f };
	m_Vertices[3].position = { +W * 0.5f, 0.0f, -D * 0.5f };

	m_Indices = { 0,1,2, 2,1,3 };

	float tile = 30.0f;
	m_Vertices[0].uv = Vector2(0, 0);
	m_Vertices[1].uv = Vector2(tile, 0);
	m_Vertices[2].uv = Vector2(0, tile);
	m_Vertices[3].uv = Vector2(tile, tile);

	for (int i = 0; i < 4; ++i)
	{
		m_Vertices[i].color = Color(1, 1, 1, 1);
		m_Vertices[i].normal = Vector3(0, 1, 0);
	}

	m_Indices = { 0,1,2, 2,1,3 };

	//読み込む画像ファイルのパス
	const char* filename = "assets/texture/concrete.jpeg";

	//画像データを格納するファイルパス
	unsigned char* imageDate = nullptr;
	int width, height, channels;

	//頂点バッファ生成
	m_VertexBuffer.Create(m_Vertices);

	// インデックスバッファ生成
	m_IndexBuffer.Create(m_Indices);

	// シェーダオブジェクト生成
	m_Shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

	//テクスチャをロード
	bool sts = m_Texture.Load("assets/texture/concrete.jpeg");
	assert(sts == true);

	//マテリアル情報取得
	m_Material = std::make_unique<Material>();
	MATERIAL mtrl;
	mtrl.Diffuse = Color(1, 1, 1, 1);
	mtrl.TextureEnable = true;//テクスチャを使うか否かのフラグ
	m_Material->Create(mtrl);

	//Groundの位置や大きさを調整
	m_Position = { 0.0f, -20.0f, 0.0f };
}

//=======================================
//更新処理
//=======================================
void Ground::Update()
{
	
}

//=======================================
//描画処理
//=======================================
void Ground::Draw(Camera* cam)
{
	// SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	// 描画の処理
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// トポロジーをセット（プリミティブタイプ）
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_Shader.SetGPU();
	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();

	m_Texture.SetGPU();
	m_Material->SetGPU();

	devicecontext->DrawIndexed(
		m_Indices.size(),	// 描画するインデックス数
		0,					// 最初のインデックスバッファの位置
		0);
}

//=======================================
//終了処理
//=======================================
void Ground::Uninit()
{
	GameManager::GetInstance()->DeleteObject(this);
}

//=======================================
//頂点情報を取得
//=======================================
std::vector<VERTEX_3D>Ground::GetVertices()
{
	std::vector<VERTEX_3D>res;
	res.resize(m_Vertices.size());

	//頂点情報を変換
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Position.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);
	Matrix worldmtx = s * r * t;

	//ワールド変換してデータを代入
	for (int i = 0; i < m_Vertices.size(); i++)
	{
		res[i].position = Vector3::Transform(m_Vertices[i].position, worldmtx);
		res[i].normal = Vector3::Transform(m_Vertices[i].normal, worldmtx);
		res[i].color = m_Vertices[i].color;
		res[i].uv = m_Vertices[i].uv;
	}
	return res;
}

