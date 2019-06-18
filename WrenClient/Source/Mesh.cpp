#include "stdafx.h"
#include "Mesh.h"
#include "ConstantBufferPerObject.h"

Mesh::Mesh(const std::string& path, ID3D11Device* device, const BYTE* vertexShaderBuffer, const int vertexShaderSize)
{
	// import mesh
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	const std::string directory = path.substr(0, path.find_last_of('\\'));

	const auto mesh = scene->mMeshes[scene->mRootNode->mChildren[0]->mMeshes[0]];

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		XMFLOAT3 vector;

		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			XMFLOAT2 texVec;

			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			texVec.x = mesh->mTextureCoords[0][i].x;
			texVec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = texVec;
		}
		else
			vertex.TexCoords = XMFLOAT2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) // retrieve indices
	{
		auto face = mesh->mFaces[i];

		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	indexCount = (int)indices.size();

	// create SamplerState
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, samplerState.ReleaseAndGetAddressOf());

	// create constant buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth = sizeof(ConstantBufferPerObject);
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());

	// create InputLayout
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	device->CreateInputLayout(ied, ARRAYSIZE(ied), vertexShaderBuffer, vertexShaderSize, inputLayout.ReleaseAndGetAddressOf());

	// create vertex buffer using existing bufferDesc
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = (UINT)(sizeof(Vertex) * vertices.size());
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices.data();

	device->CreateBuffer(&bufferDesc, &vertexData, vertexBuffer.ReleaseAndGetAddressOf());

	// create index buffer using existing bufferDesc
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.ByteWidth = (UINT)(sizeof(unsigned int) * indices.size());

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.data();

	device->CreateBuffer(&bufferDesc, &indexData, indexBuffer.ReleaseAndGetAddressOf());
}

ID3D11InputLayout* Mesh::GetInputLayout() const { return inputLayout.Get(); }

ID3D11Buffer* Mesh::GetConstantBuffer() const { return constantBuffer.Get(); }

ID3D11SamplerState* Mesh::GetSamplerState() const { return samplerState.Get(); }

ID3D11Buffer* Mesh::GetVertexBuffer() const { return vertexBuffer.Get(); }

ID3D11Buffer* Mesh::GetIndexBuffer() const { return indexBuffer.Get(); }

const int Mesh::GetIndexCount() const { return indexCount; }