#pragma once
#include "../Framework/particle.h"

class Fountain : public Particle
{
public :
	Fountain(render r, float width, float height, int maxInstanceCount = 100);
	~Fountain() {}

	void update(float elapsed_time) override;

private:
	int maxInstanceCount;
};