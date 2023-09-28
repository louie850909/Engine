#include "sprite.h"
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

sprite::sprite(render& r, const wchar_t* filename)
{
	// ���_���̃Z�b�g
	vertex vertices[]
	{
		{{-0.5, +0.5, 0}, {0, 0, 1}, {1, 1, 1, 1}, {0, 0}},
		{{+0.5, +0.5, 0}, {0, 0, 1},{1, 0, 0, 1}, {1, 0}},
		{{-0.5, -0.5, 0}, {0, 0, 1},{0, 1, 0, 1}, {0, 1}},
		{{+0.5, -0.5, 0}, {0, 0, 1},{0, 0, 1, 1}, {1, 1}},
	};

	// ���_�o�b�t�@�I�u�W�F�N�g�̐���
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

	// �摜�t�@�C���̃��[�h
	hr = load_texture_from_file(r.get_device(), filename, &shader_resource_view, &texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
	const char* cso_name{ "sprite_vs.cso" };

	// ���̓��C�A�E�g�I�u�W�F�N�g�̐���
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = create_vs_from_cso(r.get_device(), cso_name, &vertex_shader, &input_layout, input_element_desc, _countof(input_element_desc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
	cso_name = "sprite_ps.cso";

	hr = create_ps_from_cso(r.get_device(), cso_name, &pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �萔�o�b�t�@�I�u�W�F�N�g�̐���
	D3D11_BUFFER_DESC cbuffer_desc{};
	cbuffer_desc.ByteWidth = sizeof(constants);
	cbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	cbuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = r.get_device()->CreateBuffer(&cbuffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

sprite::~sprite()
{
	vertex_shader.Get()->Release();
	pixel_shader.Get()->Release();
	input_layout.Get()->Release();
	vertex_buffer.Get()->Release();
	index_buffer.Get()->Release();
	constant_buffer.Get()->Release();
	shader_resource_view.Get()->Release();
}

void sprite::renderTopLeft(render& Render, XMFLOAT2 pos, float width, float height, float r, float g, float b, float a, float degree)
{
	renderTopLeft(Render, pos, width, height, r, g, b, a, degree, 0, 0, 1, 1);
}

void sprite::renderTopLeft(render& Render, XMFLOAT2 pos, float width, float height, float r, float g, float b, float a, float degree, float tx, float ty, float tw, float th)
{
	this->width = width;
	this->height = height;

	Render.set_depth_stencil_state(render::DEPTH_TEST_OFF_DEPTH_WRITE_OFF);

	position = XMFLOAT3(pos.x, pos.y, 0.0f);

	// render�����o�֐��̈����idx, dy, dw, dh�j�����`�̊e���_�̈ʒu�i�X�N���[�����W�n�j���v�Z����
	float x0{ position.x }; // ����̒��_��x���W
	float y0{ position.y }; // ����̒��_��y���W
	float x1{ position.x + width }; // �E��̒��_��x���W
	float y1{ position.y }; // �E��̒��_��y���W
	float x2{ position.x }; // �����̒��_��x���W
	float y2{ position.y + height }; // �����̒��_��y���W
	float x3{ position.x + width }; // �E���̒��_��x���W
	float y3{ position.y + height }; // �E���̒��_��y���W
	float center_x{ position.x + width / 2.0f }; // ��]�̒��S��x���W
	float center_y{ position.y + height / 2.0f }; // ��]�̒��S��y���W

	float radian = DirectX::XMConvertToRadians(degree);
	float cos = cosf(radian);
	float sin = sinf(radian);

	rotate(x0, y0, center_x, center_y, cos, sin);
	rotate(x1, y1, center_x, center_y, cos, sin);
	rotate(x2, y2, center_x, center_y, cos, sin);
	rotate(x3, y3, center_x, center_y, cos, sin);

	// �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = Render.get_immediate_context()->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0, 0.0f };
		vertices[1].position = { x1, y1, 0.0f };
		vertices[2].position = { x2, y2, 0.0f };
		vertices[3].position = { x3, y3, 0.0f };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };

		// �e�N�X�`�����W�𒸓_�o�b�t�@�ɃZ�b�g����
		vertices[0].texcoord = { tx, ty };
		vertices[1].texcoord = { tx + tw, ty };
		vertices[2].texcoord = { tx, ty + th };
		vertices[3].texcoord = { tx + tw, ty + th };
	}
	Render.get_immediate_context()->Unmap(vertex_buffer.Get(), 0);

	// �V�F�[�_�[ ���\�[�X�̃o�C���h
	Render.get_immediate_context()->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());


	// ���_�o�b�t�@�[�̃o�C���h
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	Render.get_immediate_context()->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	// �v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
	Render.get_immediate_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	Render.get_immediate_context()->IASetInputLayout(input_layout.Get());

	// �V�F�[�_�[�̃o�C���h
	Render.get_immediate_context()->VSSetShader(vertex_shader.Get(), nullptr, 0);
	Render.get_immediate_context()->PSSetShader(pixel_shader.Get(), nullptr, 0);

	
	XMMATRIX world;
	world = XMMatrixIdentity();
	XMStoreFloat4x4(&constant_data.world, world);
	constant_data.material_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Render.get_immediate_context()->UpdateSubresource(constant_buffer.Get(), 0, 0, &constant_data, 0, 0);
	Render.get_immediate_context()->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	Render.set_2D_view_projection_matrix();

	// �v���~�e�B�u�̕`��
	Render.get_immediate_context()->Draw(4, 0);

	Render.set_depth_stencil_state(render::DEPTH_TEST_ON_DEPTH_WRITE_ON);
}