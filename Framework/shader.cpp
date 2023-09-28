#include "shader.h"

HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{
	char filepath[100] = ".\\Shader\\";
	strcat_s(filepath, cso_name);

	FILE* fp{ nullptr };
	fopen_s(&fp, filepath, "rb");
	_ASSERT_EXPR_A(fp, "CSO file not found");

	fseek(fp, 0, SEEK_END);
	long cso_size{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]> cso_data{ make_unique<unsigned char[]>(cso_size) };
	fread(cso_data.get(), cso_size, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreateVertexShader(cso_data.get(), cso_size, nullptr, vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (input_layout)
	{
		hr = device->CreateInputLayout(input_element_desc, num_elements,
			cso_data.get(), cso_size, input_layout);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	return hr;
}

HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
	char filepath[100] = ".\\Shader\\";
	strcat_s(filepath, cso_name);

	FILE* fp{ nullptr };
	fopen_s(&fp, filepath, "rb");
	_ASSERT_EXPR_A(fp, "CSO file not found");

	fseek(fp, 0, SEEK_END);
	long cso_size{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]> cso_data{ make_unique<unsigned char[]>(cso_size) };
	fread(cso_data.get(), cso_size, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreatePixelShader(cso_data.get(), cso_size, nullptr, pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}
