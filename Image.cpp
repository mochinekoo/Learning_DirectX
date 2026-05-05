#include "Image.h"
#include <wincodec.h>
#include <vector>

using namespace MyDirectX;

Image::Image(std::string path, float leftX, float leftY, float width, float height)
	: BaseObject("Image", false) {
	path_ = path;

	Vertex vertices_[6] = {
		{ leftX, leftY, 0, 1,1,1,1, 0, 1 }, // 右上 → 左下
		{ leftX, leftY + 1.0f, 0, 1,1,1,1, 0, 0 }, // 右下 → 左上
		{ leftX + 1.0f, leftY, 0, 1,1,1,1, 1, 1 }, // 左上 → 右下

		{ leftX + 1.0f, leftY, 0, 1,1,1,1, 1, 1 }, // 左上 → 右下
		{ leftX, leftY + 1.0f, 0, 1,1,1,1, 0, 0 }, // 右下 → 左上
		{ leftX + 1.0f, leftY + 1.0f, 0, 1,1,1,1, 1, 0 }, // 左下 → 右上
	};

	memcpy_s(vertices, sizeof(vertices), vertices_, sizeof(vertices_));
}

Image::~Image()
{
}

void Image::Initialize() {
	HRESULT result = {};

	IWICImagingFactory* factory = nullptr;
	IWICBitmapDecoder* decoder = nullptr;
	IWICBitmapFrameDecode* frame = nullptr;
	IWICFormatConverter* converter = nullptr;

	CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&factory)
	);

	result = factory->CreateDecoderFromFilename(
		std::wstring(path_.begin(), path_.end()).c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&decoder
	);
	assert(SUCCEEDED(result));
   result = decoder->GetFrame(0, &frame);
	assert(SUCCEEDED(result));

	result = factory->CreateFormatConverter(&converter);
	assert(SUCCEEDED(result));

	converter->Initialize(frame, 
		GUID_WICPixelFormat32bppBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		1.0f,
		WICBitmapPaletteTypeMedianCut
	);

	converter->GetSize(&width_, &height_);

	textureDesc.Width = width_;
	textureDesc.Height = height_;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	std::vector<BYTE> imageData;
	size_t rowBytes = static_cast<size_t>(width_) * 4;
	size_t imageSize = rowBytes * static_cast<size_t>(height_);
    imageData.resize(imageSize);
	result = converter->CopyPixels(nullptr, static_cast<UINT>(rowBytes), static_cast<UINT>(imageSize), imageData.data());

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = imageData.data();
	initData.SysMemPitch = static_cast<UINT>(rowBytes);

	result = MyDirectX::device_->CreateTexture2D(&textureDesc, &initData, &texture);
	assert(SUCCEEDED(result));

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	viewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;
	HRESULT hr = device_->CreateShaderResourceView(texture, &viewDesc, &textureView);
	assert(SUCCEEDED(hr) && textureView != nullptr);


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = MyDirectX::device_->CreateSamplerState(&samplerDesc, &samplerState);
	assert(SUCCEEDED(result));
	

	D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initDataa = {};
	initDataa.pSysMem = vertices;
	assert(textureView != nullptr);

	result = MyDirectX::device_->CreateBuffer(&bufferDesc, &initDataa, &vertexBuffer);
	assert(SUCCEEDED(result));
}

void Image::Update()
{
}

void Image::Draw() {
	float color[4] = { 1, 0, 0, 1.0f };

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context_->IASetInputLayout(inputLayout);
	context_->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context_->VSSetShader(vertexShader, nullptr, 0);
	context_->PSSetShader(imagePixelShader, nullptr, 0);
	context_->PSSetShaderResources(0, 1, &textureView);
	context_->PSSetSamplers(0, 1, &samplerState);

	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = FALSE;
	ID3D11RasterizerState* rastState = nullptr;
	MyDirectX::device_->CreateRasterizerState(&rastDesc, &rastState);
	MyDirectX::context_->RSSetState(rastState);

	context_->Draw(6, 0);

	MyDirectX::context_->RSSetState(nullptr);
}

void Image::Release()
{
}
