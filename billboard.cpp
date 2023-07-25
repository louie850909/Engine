#include "billboard.h"

Billboard::Billboard(render r, const wchar_t* filename, XMFLOAT3 pos, XMFLOAT3 scl, float width, float height, bool instancing, int maxInstanceCount)
{
	this->position = pos;
	this->scale = scl;
	this->rotation = { 0,0,0 };
	this->maxInstanceCount = maxInstanceCount;
	this->instancing = instancing;

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
	hr = load_texture_from_file(r.get_device(), filename, &shader_resource_view, &texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (this->instancing)
	{
		// 頂点シェーダーオブジェクトの生成
		const char* cso_name{ "instancingBillboard_vs.cso" };
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"INSTPOS",		0, DXGI_FORMAT_R32G32B32_FLOAT,		1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"INSTSCL",     0, DXGI_FORMAT_R32G32B32_FLOAT,     1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"INSTROT",     0, DXGI_FORMAT_R32G32B32_FLOAT,	    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1},
		};

		hr = create_vs_from_cso(r.get_device(), cso_name, &vertex_shader, &input_layout, input_element_desc, _countof(input_element_desc));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// ピクセルシェーダーオブジェクトの生成
		cso_name = "instancingBillboard_ps.cso";

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
		INSTANCE ins{ {0,0,0},{1,1,1},{0,0,0} };
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
	else
	{
		// 頂点シェーダーオブジェクトの生成
		const char* cso_name{ "Billboard_vs.cso" };
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		hr = create_vs_from_cso(r.get_device(), cso_name, &vertex_shader, &input_layout, input_element_desc, _countof(input_element_desc));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// ピクセルシェーダーオブジェクトの生成
		cso_name = "Billboard_ps.cso";

		hr = create_ps_from_cso(r.get_device(), cso_name, &pixel_shader);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// 定数バッファオブジェクトの生成
		D3D11_BUFFER_DESC cbuffer_desc{};
		cbuffer_desc.ByteWidth = sizeof(constants);
		cbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
		cbuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		hr = r.get_device()->CreateBuffer(&cbuffer_desc, nullptr, constant_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void Billboard::draw(render r)
{
	// シェーダー リソースのバインド
	r.get_immediate_context()->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

	// プリミティブタイプおよびデータの順序に関する情報のバインド
	r.get_immediate_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 入力レイアウトオブジェクトのバインド
	r.get_immediate_context()->IASetInputLayout(input_layout.Get());

	// シェーダーのバインド
	r.get_immediate_context()->VSSetShader(vertex_shader.Get(), nullptr, 0);
	r.get_immediate_context()->PSSetShader(pixel_shader.Get(), nullptr, 0);

	if (this->instancing)
	{
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
	}
	else
	{
		// 頂点バッファーのバインド
		UINT stride{ sizeof(vertex) };
		UINT offset{ 0 };

		r.get_immediate_context()->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

		// 定数バッファの更新
		XMMATRIX w, scl, trs, view;
		w = XMMatrixIdentity();

		// ビルドボード処理
		view = r.get_view_matrix();
		for (int k = 0; k < 3; k++)
		{
			for (int l = 0; l < 3; l++)
			{
				w.r[k].m128_f32[l] = view.r[l].m128_f32[k];
			}
		}

		scl = XMMatrixScaling(scale.x, scale.y, scale.z);
		w = scl * w;
		trs = XMMatrixTranslation(position.x, position.y, position.z);
		w = trs * w;
		XMStoreFloat4x4(&constant_data.world, w);
		constant_data.material_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		r.get_immediate_context()->UpdateSubresource(constant_buffer.Get(), 0, 0, &constant_data, 0, 0);
		r.get_immediate_context()->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

		// 描画
		r.get_immediate_context()->Draw(4, 0);
	}
}
