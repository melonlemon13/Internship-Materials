#include "Wall.h"
#include "Renderer.h"
using namespace DirectX::SimpleMath;

void Wall::Init()
{
    // 単位立方体（中心原点、辺の長さ1）
    // 6面 * 4頂点 = 24頂点（法線とUVを面ごとに分けるため）
    m_Vertices.clear();
    m_Indices.clear();
    m_Vertices.resize(24);
    m_Indices.resize(36);

    const float h = 0.5f;

    struct Face {
        Vector3 n;
        Vector3 p0, p1, p2, p3; // CCW
    };

    Face faces[6] = {
        // +Z
        { {0,0,1},  {-h,-h, h}, { h,-h, h}, {-h, h, h}, { h, h, h} },
        // -Z
        { {0,0,-1}, { h,-h,-h}, {-h,-h,-h}, { h, h,-h}, {-h, h,-h} },
        // +X
        { {1,0,0},  { h,-h, h}, { h,-h,-h}, { h, h, h}, { h, h,-h} },
        // -X
        { {-1,0,0}, {-h,-h,-h}, {-h,-h, h}, {-h, h,-h}, {-h, h, h} },
        // +Y
        { {0,1,0},  {-h, h, h}, { h, h, h}, {-h, h,-h}, { h, h,-h} },
        // -Y
        { {0,-1,0}, {-h,-h,-h}, { h,-h,-h}, {-h,-h, h}, { h,-h, h} },
    };

    // 各面4頂点：UVはとりあえず 0..1
    for (int f = 0; f < 6; ++f)
    {
        int v = f * 4;
        m_Vertices[v + 0].position = faces[f].p0;
        m_Vertices[v + 1].position = faces[f].p1;
        m_Vertices[v + 2].position = faces[f].p2;
        m_Vertices[v + 3].position = faces[f].p3;

        m_Vertices[v + 0].uv = { 0,1 };
        m_Vertices[v + 1].uv = { 1,1 };
        m_Vertices[v + 2].uv = { 0,0 };
        m_Vertices[v + 3].uv = { 1,0 };

        for (int i = 0; i < 4; ++i)
        {
            m_Vertices[v + i].normal = faces[f].n;
            m_Vertices[v + i].color = { 1,1,1,1 };
        }

        int i = f * 6;
        // 2 triangles (0,1,2) (2,1,3)
        m_Indices[i + 0] = v + 0;
        m_Indices[i + 1] = v + 1;
        m_Indices[i + 2] = v + 2;
        m_Indices[i + 3] = v + 2;
        m_Indices[i + 4] = v + 1;
        m_Indices[i + 5] = v + 3;
    }

    m_VertexBuffer.Create(m_Vertices);
    m_IndexBuffer.Create(m_Indices);

    m_Shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

    // テクスチャ（床と同じでOK）
    m_Texture.Load("assets/texture/concrete.jpeg");

    // Material 作成
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

void Wall::Draw(Camera* cam)
{
    // SRT情報作成
    Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
    Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
    Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

    Matrix worldmtx = s * r * t;
    Renderer::SetWorldMatrix(&worldmtx);

    // 描画の処理
    ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();

    // トポロジー
    devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_Shader.SetGPU();
    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();

    m_Texture.SetGPU();
    m_Material->SetGPU();

    devicecontext->DrawIndexed((UINT)m_Indices.size(), 0, 0);
}