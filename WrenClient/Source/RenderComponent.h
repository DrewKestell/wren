#pragma once

#include "Mesh.h"
#include "GameObject.h"

class RenderComponent
{
	std::shared_ptr<Mesh> mesh{ nullptr };
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	ID3D11ShaderResourceView* texture{ nullptr };
	
public:
	void Reset();
	void Initialize(std::shared_ptr<Mesh> mesh, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform, GameObject& gameObject, const float updateLag);
};