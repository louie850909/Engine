#pragma once
#include "model.h"
#include "stage.h"
#include "physic.h"

class Character
{
public:
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	Character(render* Rneder);
	~Character();

	void drawDebugPrimitive();
	std::shared_ptr<skinned_mesh> getMesh() { return mesh; }

	void updateplaceIndex(const STAGE* stage);
	int placeIndex = 0;
	XMFLOAT3 prePos;

	float collision_radius;
	float collision_height;
	std::unique_ptr<Physic> physic;

	bool isGround = false;

protected:
	render* Render;

	int clip_index;
	int frame_index;
	float animation_tick;
	bool animation_loop;
	animation::keyframe keyframe;
	const animation::keyframe* blendframes[2];
	float blendSecond;
	float blendTick;

	std::shared_ptr<skinned_mesh> mesh;

	virtual void initialize();
	virtual void update(float elapsed_time);
	virtual void draw(float elapsed_time);
	virtual void uninitialize();

	void playAnimation(int clip_index, bool loop, float blendSec);
	bool isPlayAnimation();
	void updateAnimation(float elapsed_time);
};