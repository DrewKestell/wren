#pragma once

#include "Mesh.h"

class Model
{
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	void ProcessNode(aiNode *node, const aiScene *scene);
	void LoadModel(const std::string& path);
public:
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform);
	void Translate(XMMATRIX matrix);
	XMFLOAT3 GetPosition() const;
	void SetPosition(const XMFLOAT3 pos);
};