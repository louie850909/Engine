#include "geometric_primitive.h"
#define	SIZE_WH			(10.0f)

geometric_primitive::geometric_primitive(ID3D11Device* device, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot)
{
	position = pos;
	scale = scl;
	rotation = rot;

	// 重心が原点の立方体を作成
	vertex vertices[24]
	{
		//下
		{XMFLOAT3(-SIZE_WH, -SIZE_WH,  -SIZE_WH), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(SIZE_WH, -SIZE_WH,  -SIZE_WH), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(-SIZE_WH, -SIZE_WH,   SIZE_WH), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(SIZE_WH, -SIZE_WH,   SIZE_WH), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

		//上
		{XMFLOAT3(-SIZE_WH, SIZE_WH,  SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(SIZE_WH, SIZE_WH,  SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(-SIZE_WH, SIZE_WH, -SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(SIZE_WH, SIZE_WH, -SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

		//前
		{XMFLOAT3(-SIZE_WH,  SIZE_WH,  -SIZE_WH), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(SIZE_WH,  SIZE_WH,  -SIZE_WH), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(-SIZE_WH, -SIZE_WH,  -SIZE_WH), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(SIZE_WH, -SIZE_WH,  -SIZE_WH), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

		//奥
		{XMFLOAT3(SIZE_WH,  SIZE_WH,  SIZE_WH), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(-SIZE_WH,  SIZE_WH,  SIZE_WH), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(SIZE_WH, -SIZE_WH,  SIZE_WH), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(-SIZE_WH, -SIZE_WH,  SIZE_WH), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

		//左
		{XMFLOAT3(-SIZE_WH,  SIZE_WH,  SIZE_WH), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(-SIZE_WH,  SIZE_WH, -SIZE_WH), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(-SIZE_WH, -SIZE_WH,  SIZE_WH), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(-SIZE_WH, -SIZE_WH, -SIZE_WH), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

		//右
		{XMFLOAT3(SIZE_WH,  SIZE_WH,  -SIZE_WH), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(SIZE_WH,  SIZE_WH,   SIZE_WH), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(SIZE_WH, -SIZE_WH,  -SIZE_WH), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(SIZE_WH, -SIZE_WH,   SIZE_WH), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

	};

	uint32_t indices[36]
	{
		0, 1, 2, 2, 1, 3,
		4, 5, 6, 6, 5, 7,
		8, 9, 10, 10, 9, 11,
		12, 13, 14, 14, 13, 15,
		16, 17, 18, 18, 17, 19,
		20, 21, 22, 22, 21, 23,
	};

	create_com_buffers(device, vertices, 24, indices, 36);

	HRESULT hr = { S_OK };

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA , 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA , 0},
	};

	create_vs_from_cso(device, "geometric_primitive_vs.cso", vertex_shader.GetAddressOf(),
		input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
	create_ps_from_cso(device, "geometric_primitive_ps.cso", pixel_shader.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void geometric_primitive::draw(render Render)
{
	XMMATRIX world, scl, rot, trans;
	world = XMMatrixIdentity();
	scl = XMMatrixScaling(scale.x, scale.y, scale.z);
	world = scl * world;
	rot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y + XM_PI, rotation.z);
	world = rot * world;
	trans = XMMatrixTranslation(position.x, position.y, position.z);
	world = trans * world;

	uint32_t stride{ sizeof(vertex) };
	uint32_t offset{ 0 };
	Render.get_immediate_context()->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	Render.get_immediate_context()->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	Render.get_immediate_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Render.get_immediate_context()->IASetInputLayout(input_layout.Get());

	Render.get_immediate_context()->VSSetShader(vertex_shader.Get(), nullptr, 0);
	Render.get_immediate_context()->PSSetShader(pixel_shader.Get(), nullptr, 0);

	XMStoreFloat4x4(&constant_data.world, world);
	constant_data.material_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Render.get_immediate_context()->UpdateSubresource(constant_buffer.Get(), 0, 0, &constant_data, 0, 0);
	Render.get_immediate_context()->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);
	Render.get_immediate_context()->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
}

void geometric_primitive::update()
{
#ifdef USE_IMGUI
	ImGui::Begin("Geometric Primitive");
	ImGui::SliderFloat3("Position", &position.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("Rotation", &rotation.x, -XM_PI, XM_PI);
	ImGui::SliderFloat3("Scale", &scale.x, 0.0f, 10.0f);
	ImGui::End();
#endif // USE_IMGUI

}

void geometric_primitive::create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count)
{
	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};
	D3D11_SUBRESOURCE_DATA subresource_data{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	subresource_data.pSysMem = indices;
	
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, &index_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
