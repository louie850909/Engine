#pragma once
#include "game_object.h"

class Billboard : public GAME_OBJECT
{
	struct INSTANCE
	{
		XMFLOAT3 pos;
		XMFLOAT3 scl;
		XMFLOAT3 rot;
	};

public:
	Billboard(render& r, const wchar_t* filename, XMFLOAT3 pos, XMFLOAT3 scl, float width, float height, bool instancing = false, int maxInstanceCount = 0);
	~Billboard(){}

	void draw(render& r);
	std::vector<INSTANCE> instances;

private:
	D3D11_TEXTURE2D_DESC texture2d_desc;
	int maxInstanceCount;
	bool instancing;
};
