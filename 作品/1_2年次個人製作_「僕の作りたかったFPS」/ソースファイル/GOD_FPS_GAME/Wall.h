#pragma once
#include "ObjectBase.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Material.h"

class Wall : public ObjectBase
{
private:
    std::vector<VERTEX_3D> m_Vertices;
    std::vector<unsigned int> m_Indices;

    VertexBuffer<VERTEX_3D> m_VertexBuffer;
    IndexBuffer m_IndexBuffer;

    Shader m_Shader;

    Texture m_Texture;
    std::unique_ptr<Material> m_Material;

public:
    Wall() = default;
    ~Wall() override = default;

    void Init() override;
    void Update() override{}
    void Draw(Camera* cam) override;
    void Uninit() override{}

	void SetScale(const DirectX::SimpleMath::Vector3& scale) { m_Scale = scale; }
	void SetPosition(const DirectX::SimpleMath::Vector3& pos) { m_Position = pos; }
};

