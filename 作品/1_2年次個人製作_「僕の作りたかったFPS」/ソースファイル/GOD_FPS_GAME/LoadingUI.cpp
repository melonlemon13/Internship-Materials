#include "LoadingUI.h"
#include "Renderer.h"
#include "Application.h"
using namespace DirectX::SimpleMath;

void LoadingUI::Init()
{
    // 頂点・インデックス初期化
    m_Vertices.clear();
    m_Indices.clear();

    m_Vertices.resize(4);
    m_Indices = { 0,1,2, 2,1,3 };

    const float W = (float)Application::GetWidth();
    const float H = (float)Application::GetHeight();

    m_Vertices[0].position = { 0.0f, 0.0f, 0.0f };
    m_Vertices[1].position = { W,    0.0f, 0.0f };
    m_Vertices[2].position = { 0.0f, H,    0.0f };
    m_Vertices[3].position = { W,    H,    0.0f };

    m_Vertices[0].uv = { 0,0 };
    m_Vertices[1].uv = { 1,0 };
    m_Vertices[2].uv = { 0,1 };
    m_Vertices[3].uv = { 1,1 };

    for (int i = 0; i < 4; ++i)
    {
        m_Vertices[i].normal = { 0,0,-1 };
        m_Vertices[i].color = { 1,1,1,1 };
    }

    m_VertexBuffer.Create(m_Vertices);
    m_IndexBuffer.Create(m_Indices);

    // シェーダ（UIでも今のlitTextureでOK）
    m_Shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

    // Now Loading画像
    m_Texture.Load("assets/texture/NOWLOADING.png");
    //m_Texture.Load("assets/texture/concrete.jpeg");

    // マテリアル
    m_Material = std::make_unique<Material>();
    MATERIAL mat{};
    mat.Ambient = { 1,1,1,1 };
    mat.Diffuse = { 1,1,1,1 };
    mat.Specular = { 0,0,0,0 };
    mat.Emission = { 0,0,0,0 };
    mat.Shiness = 0.0f;
    mat.TextureEnable = TRUE;
    m_Material->Create(mat);
}

void LoadingUI::Draw(Camera* cam)
{
    if (!m_Visible) return;

    // 2D描画用行列へ
    Renderer::SetWorldViewProjection2D();

    Matrix world = Matrix::Identity;
    Renderer::SetWorldMatrix(&world);

    auto* dc = Renderer::GetDeviceContext();
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_Shader.SetGPU();
    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();

    m_Texture.SetGPU();
    m_Material->SetGPU();

    dc->DrawIndexed((UINT)m_Indices.size(), 0, 0);
}