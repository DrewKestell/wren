#pragma once

#include <Components/Component.h>
#include "GameObject.h"
#include "Mesh.h"

constexpr unsigned int STRIDE = sizeof(Vertex);
constexpr unsigned int OFFSET = 0;

class RenderComponent : public Component
{
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	ID3D11ShaderResourceView* texture{ nullptr };

	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag, GameObject& gameObject);

	friend class RenderComponentManager;
public:
	Mesh* mesh{ nullptr };
};