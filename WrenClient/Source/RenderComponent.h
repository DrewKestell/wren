#pragma once

#include "GameObject.h"
#include "Mesh.h"

class RenderComponent
{
	unsigned int id{ 0 };
	unsigned int gameObjectId{ 0 };
	std::shared_ptr<Mesh> mesh{ nullptr };
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	ID3D11ShaderResourceView* texture{ nullptr };

	void Initialize(const unsigned int id, const unsigned int gameObjectId, std::shared_ptr<Mesh> mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, const float updateLag, GameObject& gameObject);

	void SetId(const unsigned int id);
	const unsigned int GetGameObjectId() const;
	void SetGameObjectId(const unsigned int id);

	friend class ObjectManager;
public:
	const unsigned int GetId() const;
};