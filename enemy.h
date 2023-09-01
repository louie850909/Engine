#pragma once

#include "character.h"

class ENEMY : public Character
{
public:
	ENEMY(render* Rneder);
	~ENEMY();

	void initialize() override;
	void update(float elapsed_time) override;
	void draw(float elapsed_time) override;
	void uninitialize() override;

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
