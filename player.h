#pragma once
#include "model.h"
#include "Input.h"
#include "camera.h"
#include "DebugRenderer.h"
#include "collision.h"

class PLAYER
{
public:
	render* Render;

	int clip_index;
	int frame_index;
	float animation_tick;
	bool animation_loop;
	animation::keyframe keyframe;
	const animation::keyframe* blendframes[2];
	float blendSecond;
	float blendTick;

	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	float collision_radius;
	float collision_height;

	std::unique_ptr<skinned_mesh> mesh;

	PLAYER(render* Rneder);
	~PLAYER();

	void initialize();
	void update(float elapsed_time);
	void draw(float elapsed_time);
	void uninitialize();

	void playAnimation(int clip_index, bool loop, float blendSec);
	bool isPlayAnimation();
	void updateAnimation(float elapsed_time);

	bool InputMove(float elapsed_time);
	bool InputJump(float elapsed_time);

	void drawDebugPrimitive();
private:
	enum class State
	{
		Idle,
		Run,
		Jump,
	};

	State state = State::Idle;

	void toIdle();
	void updateIdle(float elapsed_time);

	void toRun();
	void updateRun(float elapsed_time);

	void toJump();
	void updateJump(float elapsed_time);
};