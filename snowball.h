#pragma once
#include "model.h"
#include "DebugRenderer.h"
#include "collision.h"

class Snowball
{
public:
	Snowball(render* Render);
	~Snowball();

	void initialize(XMFLOAT3 pos, float radius);
	void update(float elapsed_time);
	void draw(float elapsed_time);
	void uninitialize();

	void drawDebugPrimitive();

	int placeIndex = -1;
	XMFLOAT3 prePos;

	float radius;
	XMFLOAT3 position;
	XMFLOAT3 rotation = XMFLOAT3(0, 0, 0);
	XMFLOAT3 scale;

private:
	render* Render;
	std::shared_ptr<skinned_mesh> mesh;

	float collision_radius;
	float collision_height;
};
