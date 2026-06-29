#pragma once
#include "ObjectBase.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Camera.h"
#include <memory>
#include <vector>

class LoadingUI : public ObjectBase
{
private:
    std::vector<VERTEX_3D> m_Vertices;
    std::vector<unsigned int> m_Indices;

    VertexBuffer<VERTEX_3D> m_VertexBuffer;
    IndexBuffer m_IndexBuffer;

    Shader m_Shader;
    Texture m_Texture;
    std::unique_ptr<Material> m_Material;

    bool m_Visible = true;
public:
    LoadingUI() = default;
    ~LoadingUI() override = default;

    void Init() override;
    void Update() override {}
    void Draw(Camera* cam) override;
    void Uninit() override {}

    void SetVisible(bool v) { m_Visible = v; }
};

