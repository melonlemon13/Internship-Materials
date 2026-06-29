#include	<iostream>
#include	"Texture.h"
#include	"stb_image.h"
#include	"renderer.h"

// テクスチャをロード
bool Texture::Load(const std::string& filename)
{
	bool sts = true;
	unsigned char* pixels;

	// 画像読み込み
	pixels = stbi_load(filename.c_str(), &m_width, &m_height, &m_bpp, 4);
	if (pixels == nullptr) {
		std::cout << filename.c_str() << " Load error " << std::endl;
		return false;
	}

	// テクスチャ2Dリソース生成
	ComPtr<ID3D11Texture2D> pTexture;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// RGBA
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA subResource{};
	subResource.pSysMem = pixels;
	subResource.SysMemPitch = desc.Width * 4;			// RGBA = 4 bytes per pixel
	subResource.SysMemSlicePitch = 0;

	ID3D11Device* device = Renderer::GetDevice();

	HRESULT hr = device->CreateTexture2D(&desc, &subResource, pTexture.GetAddressOf());
	if (FAILED(hr)) {
		stbi_image_free(pixels);
		return false;
	}

	// SRV生成
	hr = device->CreateShaderResourceView(pTexture.Get(), nullptr, m_srv.GetAddressOf());
	if (FAILED(hr)) {
		stbi_image_free(pixels);
		return false;
	}

	// ピクセルイメージ解放
	stbi_image_free(pixels);

	// テクスチャ解放
	pTexture->Release();

	return true;
}

// テクスチャをメモリからロード
bool Texture::LoadFromFemory(const unsigned char* Data,int len) {

	bool sts = true;
	unsigned char* pixels;

	// 画像読み込み
	pixels = stbi_load_from_memory(Data, 
		len, 
		&m_width, 
		&m_height, 
		&m_bpp, 
		STBI_rgb_alpha);

	// テクスチャ2Dリソース生成
	ComPtr<ID3D11Texture2D> pTexture;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// RGBA
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA subResource{};
	subResource.pSysMem = pixels;
	subResource.SysMemPitch = desc.Width * 4;			// RGBA = 4 bytes per pixel
	subResource.SysMemSlicePitch = 0;

	ID3D11Device* device = Renderer::GetDevice();

	HRESULT hr = device->CreateTexture2D(&desc, &subResource, pTexture.GetAddressOf());
	if (FAILED(hr)) {
		stbi_image_free(pixels);
		return false;
	}

	// SRV生成
	hr = device->CreateShaderResourceView(pTexture.Get(), nullptr, m_srv.GetAddressOf());
	if (FAILED(hr)) {
		stbi_image_free(pixels);
		return false;
	}

	// ピクセルイメージ解放
	stbi_image_free(pixels);

	return true;
}

bool Texture::CreateSolid1x1RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	ID3D11Device* device = Renderer::GetDevice();
	if (!device) return false;

	// 既存のSRVをリセット（ComPtrなので Reset）
	m_srv.Reset();

	// 1x1 RGBA
	const unsigned char pixel[4] = { r, g, b, a };

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;            // 変更しないのでIMMUTABLEが安全
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA subResource{};
	subResource.pSysMem = pixel;
	subResource.SysMemPitch = 4;                  // 1px * 4byte

	ComPtr<ID3D11Texture2D> pTexture;
	HRESULT hr = device->CreateTexture2D(&desc, &subResource, pTexture.GetAddressOf());
	if (FAILED(hr)) return false;

	hr = device->CreateShaderResourceView(pTexture.Get(), nullptr, m_srv.GetAddressOf());
	if (FAILED(hr)) return false;

	m_width = 1;
	m_height = 1;
	m_bpp = 4;
	return true;
}

Texture& Texture::White()
{
	static Texture s_white;
	static bool s_inited = false;

	if (!s_inited)
	{
		s_white.CreateSolid1x1RGBA(255, 255, 255, 255);
		s_inited = true;
	}
	return s_white;
}


// テクスチャをGPUにセット
void Texture::SetGPU()
{
	ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();

	//// テクスチャが未設定なら白を刺す
	//if (!m_srv)
	//{
	//	Texture& w = Texture::White();
	//	devicecontext->PSSetShaderResources(0, 1, w.m_srv.GetAddressOf());
	//	return;
	//}

	devicecontext->PSSetShaderResources(0, 1, m_srv.GetAddressOf());
}
