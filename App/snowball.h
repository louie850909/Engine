#pragma once
#include "../Framework/model.h"
#include "../Framework/DebugRenderer.h"
#include "../Framework/collision.h"
#include "../Framework/character.h"

class Snowball : public Character
{
public:
	Snowball(render* Render);
	~Snowball();

	void initialize(XMFLOAT3 pos, float radius);
	void update(float elapsed_time) ;
	void draw(float elapsed_time);
	void uninitialize();

	void drawDebugPrimitive();

	float radius;
	XMFLOAT3 drawPos;
};
