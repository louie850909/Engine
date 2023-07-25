#include "particle.h"

Particle::Particle(render r, float width, float height, int maxInstanceCount)
{
	this->position = { 0,0,0 };
	this->scale = { 1,1,1 };
	this->rotation = { 0,0,0 };
	this->width = width;
	this->height = height;
	this->maxInstanceCount = maxInstanceCount;

	// 頂点情報のセット
	vertex vertices[]
	{
		{{-width / 2.0f, height, 0.0f}, {0,0,1}, {1, 1, 1, 1}, {0, 0}},
		{{+width / 2.0f, height, 0.0f}, {0,0,1}, {1, 1, 1, 1}, {1, 0}},
		{{-width / 2.0f, 0.0f,   0.0f}, {0,0,1}, {1, 1, 1, 1}, {0, 1}},
		{{+width / 2.0f, 0.0f,   0.0f}, {0,0,1}, {1, 1, 1, 1}, {1, 1}},
	};

	// 頂点バッファオブジェクトの生成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(vertices);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	HRESULT hr = r.get_device()->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 画像ファイルのロード
	hr = load_texture_from_file(r.get_device(), L".\\resources\\Texture\\particle.jpg", &shader_resource_view, &texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 頂点シェーダーオブジェクトの生成
	const char* cso_name{ "particle_vs.cso" };
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"INSTPOS",		0, DXGI_FORMAT_R32G32B32_FLOAT,		1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1},
		{"INSTSCL",     0, DXGI_FORMAT_R32G32B32_FLOAT,     1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1},
		{"INSTCOLOR",   0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1},
		{"INSTMOVE",    0, DXGI_FORMAT_R32G32B32_FLOAT,     1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1},
		{"INSTLIFE",    0, DXGI_FORMAT_R32_FLOAT,            1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1},
		{"INSTUSE",     0, DXGI_FORMAT_R32_SINT,            1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1},
	};

	hr = create_vs_from_cso(r.get_device(), cso_name, &vertex_shader, &input_layout, input_element_desc, _countof(input_element_desc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ピクセルシェーダーオブジェクトの生成
	cso_name = "particle_ps.cso";

	hr = create_ps_from_cso(r.get_device(), cso_name, &pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 定数バッファオブジェクトの生成
	D3D11_BUFFER_DESC cbuffer_desc{};
	cbuffer_desc.ByteWidth = sizeof(constants);
	cbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	cbuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = r.get_device()->CreateBuffer(&cbuffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// インスタンスデータのセット
	INSTANCE ins;
	ins.pos = XMFLOAT3(0, 0, 0);
	ins.scl = XMFLOAT3(1, 1, 1);
	ins.color = XMFLOAT4(1, 1, 1, 1);
	ins.move = XMFLOAT3(0, 0, 0);
	ins.life = 0.0f;
	ins.use = false;

	for (int i = 0; i < maxInstanceCount; ++i)
	{
		instances.push_back(ins);
	}

	// インスタンスバッファオブジェクトの生成
	D3D11_BUFFER_DESC instance_buffer_desc{};
	instance_buffer_desc.ByteWidth = sizeof(INSTANCE) * maxInstanceCount;
	instance_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	instance_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instance_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instance_buffer_desc.MiscFlags = 0;
	instance_buffer_desc.StructureByteStride = 0;

	subresource_data.pSysMem = instances.data();
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	hr = r.get_device()->CreateBuffer(&instance_buffer_desc, &subresource_data, &instance_buffer);
}

void Particle::draw(render r)
{
	// 加算合成に設定
	r.set_blend_state(render::BLEND_ADD);

	// シェーダー リソースのバインド
	r.get_immediate_context()->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

	// プリミティブタイプおよびデータの順序に関する情報のバインド
	r.get_immediate_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 入力レイアウトオブジェクトのバインド
	r.get_immediate_context()->IASetInputLayout(input_layout.Get());

	// シェーダーのバインド
	r.get_immediate_context()->VSSetShader(vertex_shader.Get(), nullptr, 0);
	r.get_immediate_context()->PSSetShader(pixel_shader.Get(), nullptr, 0);

	// インスタンスバッファーの更新
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	HRESULT hr = r.get_immediate_context()->Map(instance_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	memcpy(mapped_subresource.pData, instances.data(), sizeof(INSTANCE) * instances.size());
	r.get_immediate_context()->Unmap(instance_buffer.Get(), 0);

	// 頂点バッファーのバインド
	UINT strides[2];
	UINT offsets[2];
	ID3D11Buffer* bufferPointer[2];

	strides[0] = sizeof(vertex);
	strides[1] = sizeof(INSTANCE);
	offsets[0] = 0;
	offsets[1] = 0;
	bufferPointer[0] = vertex_buffer.Get();
	bufferPointer[1] = instance_buffer.Get();

	r.get_immediate_context()->IASetVertexBuffers(0, 2, bufferPointer, strides, offsets);

	// 描画
	r.get_immediate_context()->DrawInstanced(4, maxInstanceCount, 0, 0);

	// ブレンドステートの解除
	r.set_blend_state(render::BLEND_ALPHA);
}

void Particle::addParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 color, int life)
{
	// 未使用のパーティクルを探す
	for (int i = 0; i < maxInstanceCount; ++i)
	{
		if (!instances[i].use)
		{
			instances[i].pos = pos;
			instances[i].move = move;
			instances[i].color = color;
			instances[i].life = life;
			instances[i].use = true;
			break;
		}
	}
}