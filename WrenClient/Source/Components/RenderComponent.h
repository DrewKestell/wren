#pragma once

#include "GameObject.h"
#include "Mesh.h"

class RenderComponent
{
	const unsigned int STRIDE{ sizeof(Vertex) };
	const unsigned int OFFSET{ 0 };

	unsigned int id{ 0 };
	long gameObjectId{ 0 };
	Mesh* mesh{ nullptr };
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	ID3D11ShaderResourceView* texture{ nullptr };

	void Initialize(const unsigned int id, const long gameObjectId, Mesh* mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag, GameObject& gameObject);

	void SetId(const unsigned int id);
	const long GetGameObjectId() const;
	void SetGameObjectId(const long id);

	friend class RenderComponentManager;
public:
	const unsigned int GetId() const;
};