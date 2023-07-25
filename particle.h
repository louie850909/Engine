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
	Particle(render r, float width, float height, int maxInstanceCount = 100);
	~Particle() {}

	void draw(render r);
	virtual void update(float elapsed_time) {}
	void addParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 color, int life);
	std::vector<INSTANCE> instances;

	float width;
	float height;

private:
	D3D11_TEXTURE2D_DESC texture2d_desc;
	int maxInstanceCount;
};
