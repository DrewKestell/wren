#pragma once

#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Position;
	//XMFLOAT4 Color;
	//XMFLOAT3 Normal;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
		: vertices{ vertices },
		  indices{ indices }
	{
	}
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};