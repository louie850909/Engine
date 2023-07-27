#pragma once
#include "particle.h"

class Rain : public Particle
{
public:
	Rain(render r, float width, float height, int maxInstanceCount = 1000);
	~Rain() {}

	void update(float elapsed_time) override;

private:
	int maxInstanceCount;
};