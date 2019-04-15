#pragma once

#include "Mesh.h"

class Model
{
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11Buffer> constantBuffer;
	ComPtr<ID3D11SamplerState> samplerState;
	float targetX{ 0.0f };
	float targetZ{ 0.0f };
	XMMATRIX worldTransform{ XMMatrixIdentity() };
	ID3D11Device* device = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;
	std::vector<Mesh> meshes;

	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	void ProcessNode(aiNode *node, const aiScene *scene);
	void LoadModel(const std::string& path);
public:
	Model(ID3D11Device* device, const BYTE* vertexShaderBuffer, const int vertexShaderSize, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture, const std::string& path);
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform);
	void Translate(XMMATRIX matrix);
	XMFLOAT3 GetPosition() const;
	void SetPosition(const XMFLOAT3 pos);
};