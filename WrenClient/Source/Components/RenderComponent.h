#pragma once

#include "GameObject.h"
#include "Mesh.h"

static const unsigned int STRIDE{ sizeof(Vertex) };
static const unsigned int OFFSET{ 0 };

class RenderComponent
{
	unsigned int id{ 0 };
	long gameObjectId{ 0 };
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	ID3D11ShaderResourceView* texture{ nullptr };

	void Initialize(const unsigned int id, const long gameObjectId, Mesh* mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag, GameObject& gameObject);

	friend class RenderComponentManager;
public:
	Mesh* mesh{ nullptr };

	const unsigned int GetId() const;
};