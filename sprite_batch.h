#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>
#include "texture.h"
#include "shader.h"
#include "game_object.h"

class sprite_batch : public GAME_OBJECT
{
public:
	sprite_batch(render& Render, const wchar_t* filename, size_t& max_sprites);
	~sprite_batch();

	D3D11_TEXTURE2D_DESC texture2d_desc;

	const size_t max_vertices;
	std::vector<vertex> vertices;

	void renderTopLeft(render& Render,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a,
		float angle/*degree*/);

	void renderTopLeft(render& Render,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a,
		float angle/*degree*/,
		float tx, float ty, float tw, float th);

	void begin(render Render);
	void end(render Render);

private:

};
