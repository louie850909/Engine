#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "texture.h"
#include <comdef.h>

static map<wstring, ComPtr<ID3D11ShaderResourceView>> resources;

HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* filename,
	ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{
	// シェーダーリソースビューオブジェクトの生成
	HRESULT hr{ S_OK };
	ComPtr<ID3D11Resource> resource;

	auto it = resources.find(filename);
	if (it != resources.end())
	{
		*shader_resource_view = it->second.Get();
		(*shader_resource_view)->AddRef();
		(*shader_resource_view)->GetResource(resource.GetAddressOf());
	}
	else
	{
		hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
		if (FAILED(hr))
		{
			// WICでサポートされていないフォーマットの場合（TGAなど）は
			// STBで画像読み込みをしてテクスチャを生成する
			int width, height, bpp;
			unsigned char* pixels = stbi_load(_bstr_t(filename), &width, &height, &bpp, STBI_rgb_alpha);
			if (pixels != nullptr)
			{
				D3D11_TEXTURE2D_DESC desc = { 0 };
				desc.Width = width;
				desc.Height = height;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
				D3D11_SUBRESOURCE_DATA data;
				::memset(&data, 0, sizeof(data));
				data.pSysMem = pixels;
				data.SysMemPitch = width * 4;

				Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
				HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
				resource = texture;
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

				hr = device->CreateShaderResourceView(texture.Get(), nullptr, shader_resource_view);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

				// 後始末
				stbi_image_free(pixels);
			}
			else
			{
				// 読み込み失敗したらダミーテクスチャを作る
				LOG("load failed : %s\n", filename);

				const int width = 8;
				const int height = 8;
				UINT pixels[width * height];
				::memset(pixels, 0xFF, sizeof(pixels));

				D3D11_TEXTURE2D_DESC desc = { 0 };
				desc.Width = width;
				desc.Height = height;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
				D3D11_SUBRESOURCE_DATA data;
				::memset(&data, 0, sizeof(data));
				data.pSysMem = pixels;
				data.SysMemPitch = width;

				Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
				HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
				resource = texture;
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

				hr = device->CreateShaderResourceView(texture.Get(), nullptr, shader_resource_view);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
		}
		resources.insert(make_pair(filename, *shader_resource_view));
	}

	// テクスチャの情報を取得
	ComPtr<ID3D11Texture2D> texture2d;
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	texture2d->GetDesc(texture2d_desc);

	return hr;
}

void release_all_texture()
{
	resources.clear();
}