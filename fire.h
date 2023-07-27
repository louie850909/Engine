#pragma once
#include "particle.h"

class Fire : public Particle
{
public:
	Fire(render r, float width, float height, XMFLOAT3 pos, int maxInstanceCount = 100);
	~Fire() {}

	void update(float elapsed_time) override;

private:
	int maxInstanceCount;
	XMFLOAT3 pos;
};