#pragma once
#include "../Framework/model.h"
#include "../Framework/character.h"
#include "../Framework/Input.h"
#include "../Framework/camera.h"
#include "../Framework/DebugRenderer.h"
#include "../Framework/collision.h"
#include "../Framework/Effect.h"
#include "../Framework/AudioSource.h"
#include "../Framework/Audio.h"
#include "../Framework/physic.h"

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
	std::unique_ptr<AudioSource> WalkSE;
};