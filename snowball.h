#pragma once
#include "model.h"
#include "DebugRenderer.h"
#include "collision.h"
#include "character.h"

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
