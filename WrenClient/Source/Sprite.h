#pragma once

class Sprite
{
	ID3D11VertexShader* vertexShader{ nullptr };
	ID3D11PixelShader* pixelShader{ nullptr };
	ID3D11ShaderResourceView* texture{ nullptr };
	Sprite(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* texture);
public:
	void Draw(ID3D11DeviceContext* immediateContext, const XMMATRIX viewTransform, const XMMATRIX projectionTransform);
};