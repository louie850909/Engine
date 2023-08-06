#pragma once
#include "model.h"
#include "character.h"
#include "Input.h"
#include "camera.h"
#include "DebugRenderer.h"
#include "collision.h"
#include "Effect.h"
#include "AudioSource.h"
#include "Audio.h"

class PLAYER : public Character
{
public:
	PLAYER(render* Rneder);
	~PLAYER();

	void initialize() override;
	void update(float elapsed_time) override;
	void draw(float elapsed_time) override;
	void uninitialize() override;

	bool InputMove(float elapsed_time);
	bool InputJump(float elapsed_time);

	int placeIndex = -1;
	XMFLOAT3 prePos;

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

	Effect* effect = nullptr;
	std::shared_ptr<AudioSource> WalkSE;
};