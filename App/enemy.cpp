#include "enemy.h"
#include "../Framework/modelManager.h"

ENEMY::ENEMY(render* Rneder) : Character(Rneder)
{
	this->Render = Render;
	position = XMFLOAT3(100.0f, 10.0f, 100.0f);
	rotation = XMFLOAT3(0.0f, XM_PI, 0.0f);
	scale = XMFLOAT3(0.1f, 0.1f, 0.1f);
	clip_index = 0;
	frame_index = 0;
	animation_tick = 0.0f;
	blendSecond = 0.0f;
	blendTick = 0.0f;
	animation_loop = true;
	collision_radius = 3.0f;
	collision_height = 10.0f;
}

ENEMY::~ENEMY()
{
}

void ENEMY::initialize()
{
	mesh = ModelManager::getInstance().LoadSkinnedMesh(".\\resources\\Slime\\Slime.fbx", Render, false, position, scale, rotation);

	keyframe = mesh->animation_clips.at(clip_index).sequence.at(frame_index);

	physic = std::make_unique<Physic>();
}

void ENEMY::update(float elapsed_time)
{
	prePos = position;

	switch (state)
	{
	case State::Idle:
		updateIdle(elapsed_time);
		break;

	case State::Run:
		updateRun(elapsed_time);
		break;

	case State::Jump:
		updateJump(elapsed_time);
		break;
	}

	physic->updateVelocity(elapsed_time, isGround);
	position = physic->updateMove(position, elapsed_time);

	updateAnimation(elapsed_time);
}

void ENEMY::draw(float elapsed_time)
{
	mesh.get()->position = position;
	mesh.get()->rotation = rotation;
	mesh->rotation.y += XM_PI;
	mesh.get()->scale = scale;

	mesh->draw(*Render, XMFLOAT4(1, 1, 1, 1), skinned_mesh::RHS_YUP, &keyframe);
}

void ENEMY::uninitialize()
{
}

void ENEMY::toIdle()
{
}

void ENEMY::updateIdle(float elapsed_time)
{
}

void ENEMY::toRun()
{
}

void ENEMY::updateRun(float elapsed_time)
{
}

void ENEMY::toJump()
{
}

void ENEMY::updateJump(float elapsed_time)
{
}
