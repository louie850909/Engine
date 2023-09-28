#pragma once

#include "misc.h"
#include "shader.h"
#include "game_object.h"

class geometric_primitive : public GAME_OBJECT
{
public:
	geometric_primitive(ID3D11Device* device, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot);
	virtual ~geometric_primitive() = default;

	void draw(render Render);

	virtual void update() override;

protected:
	void create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
		uint32_t * indices, size_t index_count);

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
};
