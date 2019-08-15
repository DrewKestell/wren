#pragma once

std::ostream& operator<< (std::ostream& out, const DirectX::XMFLOAT3& vec);

DirectX::XMFLOAT3 operator+ (const DirectX::XMFLOAT3& l, const DirectX::XMFLOAT3& r);

DirectX::XMFLOAT2 operator+ (const DirectX::XMFLOAT2& l, const DirectX::XMFLOAT2& r);