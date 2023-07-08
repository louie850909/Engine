#include "sprite_batch.h"
#include "misc.h"
#include <sstream>
#include <WICTextureLoader.h>

inline void rotate(float& x, float& y, float center_x, float center_y, float cos, float sin)
{
	x -= center_x;
	y -= center_y;

	float tx{ x };
	float ty{ y };
	x = tx * cos - ty * sin;
	y = tx * sin + ty * cos;

	x += center_x;
	y += center_y;
}

sprite_batch::sprite_batch(render Render, const wchar_t* filename, size_t max_sprites) : max_vertices(max_sprites * 6)
{
	HRESULT hr = { S_OK };

	std::unique_ptr<vertex[]> vertices{ std::make_unique<vertex[]>(max_vertices) };

	// 頂点バッファオブジェクトの生成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(vertex) * max_vertices;
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = vertices.get();
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	hr = Render.get_device()->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 画像ファイルのロード
	hr = load_texture_from_file(Render.get_device(), filename, &shader_resource_view, &texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 頂点シェーダーオブジェクトの生成
	const char* cso_name{ "sprite_vs.cso" };

	// 入力レイアウトオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = create_vs_from_cso(Render.get_device(), cso_name, &vertex_shader, &input_layout, input_element_desc, _countof(input_element_desc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ピクセルシェーダーオブジェクトの生成
	cso_name = "sprite_ps.cso";

	hr = create_ps_from_cso(Render.get_device(), cso_name, &pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 定数バッファオブジェクトの生成
	D3D11_BUFFER_DESC cbuffer_desc{};
	cbuffer_desc.ByteWidth = sizeof(constants);
	cbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	cbuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = Render.get_device()->CreateBuffer(&cbuffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

sprite_batch::~sprite_batch()
{
}

void sprite_batch::renderTopLeft(render Render, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float degree)
{
	renderTopLeft(Render, dx, dy, dw, dh, r, g, b, a, degree, 0, 0, 1, 1);
}

void sprite_batch::renderTopLeft(render Render, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float degree, float tx, float ty, float tw, float th)
{
	// スクリーン（ビューポート）のサイズを取得する
	D3D11_VIEWPORT viewport{};
	UINT viewport_count{ 1 };
	Render.get_immediate_context()->RSGetViewports(&viewport_count, &viewport);

	// renderメンバ関数の引数（dx, dy, dw, dh）から矩形の各頂点の位置（スクリーン座標系）を計算する
	float x0{ dx }; // 左上の頂点のx座標
	float y0{ dy }; // 左上の頂点のy座標
	float x1{ dx + dw }; // 右上の頂点のx座標
	float y1{ dy }; // 右上の頂点のy座標
	float x2{ dx }; // 左下の頂点のx座標
	float y2{ dy + dh }; // 左下の頂点のy座標
	float x3{ dx + dw }; // 右下の頂点のx座標
	float y3{ dy + dh }; // 右下の頂点のy座標
	float center_x{ dx + dw / 2.0f }; // 回転の中心のx座標
	float center_y{ dy + dh / 2.0f }; // 回転の中心のy座標

	float radian = DirectX::XMConvertToRadians(degree);
	float cos = cosf(radian);
	float sin = sinf(radian);

	rotate(x0, y0, center_x, center_y, cos, sin);
	rotate(x1, y1, center_x, center_y, cos, sin);
	rotate(x2, y2, center_x, center_y, cos, sin);
	rotate(x3, y3, center_x, center_y, cos, sin);

	vertices.push_back({ {x0, y0, 0.0f}, {0, 0, 1}, {r, g, b, a}, {tx, ty} });
	vertices.push_back({{x1, y1, 0.0f},  {0, 0, 1}, {r, g, b, a}, {tx + tw, ty}});
	vertices.push_back({{x2, y2, 0.0f},  {0, 0, 1}, {r, g, b, a}, {tx, ty + th}});
	vertices.push_back({{x2, y2, 0.0f},  {0, 0, 1}, {r, g, b, a}, {tx, ty + th}});
	vertices.push_back({{x1, y1, 0.0f},  {0, 0, 1}, {r, g, b, a}, {tx + tw, ty}});
	vertices.push_back({{x3, y3, 0.0f},  {0, 0, 1}, {r, g, b, a}, {tx + tw, ty + th}});
}

void sprite_batch::begin(render Render)
{
	vertices.clear();

	// シェーダー リソースのバインド
	Render.get_immediate_context()->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

	// シェーダーのバインド
	Render.get_immediate_context()->VSSetShader(vertex_shader.Get(), nullptr, 0);
	Render.get_immediate_context()->PSSetShader(pixel_shader.Get(), nullptr, 0);
}

void sprite_batch::end(render Render)
{
	// 計算結果で頂点バッファオブジェクトを更新する
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = Render.get_immediate_context()->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
	vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	if (data != nullptr)
	{
		const vertex* p = vertices.data();
		memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
	}
	Render.get_immediate_context()->Unmap(vertex_buffer.Get(), 0);

	// 頂点バッファーのバインド
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	Render.get_immediate_context()->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	// プリミティブタイプおよびデータの順序に関する情報のバインド
	Render.get_immediate_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 入力レイアウトオブジェクトのバインド
	Render.get_immediate_context()->IASetInputLayout(input_layout.Get());

	XMMATRIX world;
	world = XMMatrixIdentity();
	XMStoreFloat4x4(&constant_data.world, world);
	constant_data.material_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Render.get_immediate_context()->UpdateSubresource(constant_buffer.Get(), 0, 0, &constant_data, 0, 0);
	Render.get_immediate_context()->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	Render.set_2D_view_projection_matrix();

	Render.set_depth_stencil_state(render::DEPTH_TEST_OFF_DEPTH_WRITE_OFF);

	// プリミティブの描画
	Render.get_immediate_context()->Draw(static_cast<UINT>(vertex_count), 0);

	Render.set_depth_stencil_state(render::DEPTH_TEST_ON_DEPTH_WRITE_ON);
}
