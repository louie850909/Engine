#pragma once
#include <WICTextureLoader.h>
#include <wrl.h>
#include <map>
#include <string>
#include "misc.h"
#include "Logger.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* filename,
	ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);

void release_all_texture();

