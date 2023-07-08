#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "texture.h"
#include "shader.h"
#include "game_object.h"
#include "render.h"

class sprite : public GAME_OBJECT
{
public:
	float width;
	float height;

	D3D11_TEXTURE2D_DESC texture2d_desc;

	sprite(render r, const wchar_t* filename, XMFLOAT2 pos, float width, float height);
	~sprite();

	void renderTopLeft(render Render,
		float r, float g, float b, float a,
		float angle/*degree*/);

	void renderTopLeft(render RenderR,
		float r, float g, float b, float a,
		float angle/*degree*/,
		float tx, float ty, float tw, float th);

private:
};
