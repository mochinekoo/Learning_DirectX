#pragma once
#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

struct Vertex {
    float x, y, z;
    float r, g, b, a;
};

namespace MyDirectX {
    inline ID3D11Device* device_ = nullptr;
    inline ID3D11DeviceContext* context_ = nullptr;
    inline IDXGISwapChain* swapChain = nullptr;
    inline ID3D11RenderTargetView* renderTargetView = nullptr;
    inline ID3D11Texture2D* texture2D = nullptr;

    inline ID2D1Factory* d2dFactory = nullptr;
    inline IDWriteFactory* dwriteFactory = nullptr;
    inline ID2D1RenderTarget* d2dRenderTarget = nullptr;
    inline ID2D1SolidColorBrush* brush = nullptr;
    inline IDWriteTextFormat* textFormat = nullptr;

	inline ID3D11VertexShader* vertexShader = nullptr;
	inline ID3D11PixelShader* pixelShader = nullptr;
	inline ID3D11InputLayout* inputLayout = nullptr;

    inline void initialize() {
		ID3DBlob* vsBlob = nullptr; 
		ID3DBlob* psBlob = nullptr;
		ID3DBlob* vsBlobError = nullptr;
		ID3DBlob* psBlobError = nullptr;

		D3DCompileFromFile(
			L"PixelShader.hlsl", 
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", 
			"ps_5_0", 
			D3DCOMPILE_ENABLE_STRICTNESS, 
			0,
			&psBlob,
			&psBlobError
		);

		D3DCompileFromFile(
			L"VertexShader.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"vs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&vsBlob,
			&vsBlobError
		);

		device_->CreateVertexShader(
			vsBlob->GetBufferPointer(), 
			vsBlob->GetBufferSize(),
			NULL,
			&vertexShader);

		device_->CreatePixelShader(
			psBlob->GetBufferPointer(), 
			psBlob->GetBufferSize(),
			NULL,
			&pixelShader);

		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                          D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, UINT(3 * sizeof(float)),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		device_->CreateInputLayout(
			layout,
			ARRAYSIZE(layout),
			vsBlob->GetBufferPointer(),
			vsBlob->GetBufferSize(),
			&inputLayout
		);
    }
}