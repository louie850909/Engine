#pragma once
#include "model.h"

class Character
{
public:
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	Character(render* Rneder);
	~Character();

	void drawDebugPrimitive();
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

	float collision_radius;
	float collision_height;

	std::shared_ptr<skinned_mesh> mesh;

	virtual void initialize();
	virtual void update(float elapsed_time);
	virtual void draw(float elapsed_time);
	virtual void uninitialize();

	void playAnimation(int clip_index, bool loop, float blendSec);
	bool isPlayAnimation();
	void updateAnimation(float elapsed_time);
};