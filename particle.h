#pragma once
#include "game_object.h"

class Particle : public GAME_OBJECT
{
	struct INSTANCE
	{
		XMFLOAT3	pos;
		XMFLOAT3	scl;
		XMFLOAT4	color;
		XMFLOAT3	move;
		float		life;
		int			use;
	};

public:
	Particle(render& r, float width, float height, int maxInstanceCount = 100, bool isAddBlend = false, bool isDepthTest = true);
	~Particle() {}

	void draw(render& r);
	virtual void update(float elapsed_time) {}
	void addParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 scl, XMFLOAT4 color, int life, int startidx);
	std::vector<INSTANCE> instances;

	float width;
	float height;

	bool isFull = false;
	int  startIndex = 0;
	bool isAddBlend = false;
	bool isDepthTest = true;

private:
	D3D11_TEXTURE2D_DESC texture2d_desc;
	int maxInstanceCount;
};
