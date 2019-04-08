#pragma once

class Mesh;

class Model
{
	float targetX = 0.0f;
	float targetZ = 0.0f;
	XMMATRIX worldTransform = XMMatrixIdentity();
	ID3D11Device* device = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11Buffer* constantBuffer = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;
	ID3D11SamplerState* samplerState = nullptr;
	std::vector<Mesh> meshes;

	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	void ProcessNode(aiNode *node, const aiScene *scene);
	void LoadModel(std::string& path);
public:
	Model(ID3D11Device* device, BYTE* vertexShaderBuffer, int vertexShaderSize, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture, std::string& path);
	void Draw(ID3D11DeviceContext* immediateContext, XMMATRIX viewTransform, XMMATRIX projectionTransform);
	void Translate(XMMATRIX matrix);
	XMFLOAT3 GetPosition();
	void SetPosition(XMFLOAT3 pos);
};