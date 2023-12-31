#include "player.h"
#include <comdef.h>
#include "../Framework/Input.h"
#include "../Framework/camera.h"
#include "../Framework/stage.h"
#include "../Framework/modelManager.h"


PLAYER::PLAYER(render* Render) : Character(Render)
{
	this->Render = Render;
	position = XMFLOAT3(0.0f, 10.0f, 0.0f);
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

PLAYER::~PLAYER()
{
	delete effect;
}

void PLAYER::initialize()
{
	//mesh = std::make_unique<skinned_mesh>(Render->get_device(), _bstr_t(L".\\resources\\UnitychanSD\\Unity_Chan_generic.fbx"), true, position, scale, rotation, 0.0f);
	mesh = ModelManager::getInstance().LoadSkinnedMesh(".\\resources\\UnitychanSD\\Unity_Chan_generic.fbx", Render, true);

	mesh->append_animations(".\\resources\\UnitychanSD\\Animation\\idle.fbx", 0.0f, skinned_mesh::BINARY);
	mesh->append_animations(".\\resources\\UnitychanSD\\Animation\\run.fbx", 0.0f, skinned_mesh::BINARY);
	mesh->append_animations(".\\resources\\UnitychanSD\\Animation\\jump.fbx", 0.0f, skinned_mesh::BINARY);
	mesh->append_animations(".\\resources\\UnitychanSD\\Animation\\Joyful Jump.fbx", 0.0f, skinned_mesh::BINARY);
	mesh->append_animations(".\\resources\\UnitychanSD\\Animation\\Roll.fbx", 0.0f, skinned_mesh::BINARY);
	mesh->append_animations(".\\resources\\UnitychanSD\\Animation\\Hit.fbx", 0.0f, skinned_mesh::BINARY);
	mesh->append_animations(".\\resources\\UnitychanSD\\Animation\\Gett Up.fbx", 0.0f, skinned_mesh::BINARY);
	mesh->append_animations(".\\resources\\UnitychanSD\\Animation\\Death.fbx", 0.0f, skinned_mesh::BINARY);
	mesh->append_animations(".\\resources\\UnitychanSD\\Animation\\Fall Down.fbx", 0.0f, skinned_mesh::BINARY);
	keyframe = mesh->animation_clips.at(clip_index).sequence.at(frame_index);

	effect = new Effect(".\\resources\\Effect\\test3.efk", this->Render);
	WalkSE = Audio::Instance().LoadAudioSource(".\\resources\\SE\\walk.wav");

	physic = std::make_unique<Physic>();
}

void PLAYER::update(float elapsed_time)
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

#ifdef _DEBUG
	ImGui::Begin("PLAYER");
	ImGui::SliderFloat3("Position", &position.x, -1000.0f, 1000.0f, "%.1f");
	ImGui::SliderFloat3("Rotation", &rotation.x, -180.0f, 180.0f, "%.1f");
	ImGui::SliderFloat3("Scale", &scale.x, 0.0f, 10.0f, "%.1f");
	ImGui::SliderInt("Clip Index", &clip_index, 0, mesh->animation_clips.size() - 1);
	ImGui::SliderInt("Frame Index", &frame_index, 0, mesh->animation_clips.at(clip_index).sequence.size() - 1);
	ImGui::SliderFloat("Blend Second", &blendSecond, 0.0f, 10.0f, "%.1f", 0.5f);
	ImGui::SliderFloat("Blend Tick", &blendTick, 0.0f, blendSecond, "%.1f", 0.5f);
	ImGui::DragInt("Place Index", &placeIndex, 0.1f, 0, 5);
	ImGui::End();
#endif // _DEBUG

	updateAnimation(elapsed_time);
}

void PLAYER::draw(float elapsed_time)
{
	mesh.get()->position = position;
	mesh.get()->rotation = rotation;
	mesh->rotation.y += XM_PI;
	mesh.get()->scale = scale;

	mesh->draw(*Render, XMFLOAT4(1, 1, 1, 1), skinned_mesh::RHS_YUP, &keyframe);
}

void PLAYER::uninitialize()
{
}

bool PLAYER::InputMove(float elapsed_time)
{
	// キー入力を取得する
	GamePad& gamePad = Input::Instance().GetGamePad();                                                
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();
	float angle = atan2f(ax, ay);

	// 移動量を計算する
	float moveSpeed = 50.0f * elapsed_time;

	if (ax != 0 || ay != 0)
	{
		rotation.y = CAMERA::Instance().get_rotation().y + angle;
		position.x += sinf(rotation.y) * moveSpeed;
		position.z += cosf(rotation.y) * moveSpeed;
		return true;
	}
	return false;
}

bool PLAYER::InputJump(float elapsed_time)
{
	// キー入力を取得する
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() == gamePad.BTN_LEFT_SHOULDER)
	{
		effect->Play(position);
		return true;
	}
	return false;
}

void PLAYER::toIdle()
{
	state = State::Idle;
	blendframes[0] = &keyframe;
	blendframes[1] = &mesh->animation_clips.at((int)State::Idle).sequence.at(0);
	playAnimation((int)State::Idle, true, 0.05f);
	WalkSE->Stop();
}

void PLAYER::updateIdle(float elapsed_time)
{
	if(InputMove(elapsed_time) && blendSecond == 0)
		toRun();
	if(InputJump(elapsed_time) && blendSecond == 0)
		toJump();
}

void PLAYER::toRun()
{
	state = State::Run;
	blendframes[0] = &keyframe;
	blendframes[1] = &mesh->animation_clips.at((int)State::Run).sequence.at(0);
	playAnimation((int)State::Run, true, 0.05f);
}

void PLAYER::updateRun(float elapsed_time)
{
	WalkSE->Play(false);
	if (!InputMove(elapsed_time) && blendSecond == 0)
		toIdle();
	if (InputJump(elapsed_time) && blendSecond == 0)
		toJump();
}

void PLAYER::toJump()
{
	state = State::Jump;
	isGround = false;
	
	Physic::Force force;
	force.forward = XMFLOAT3(0,1,0);
	force.power = 100.0f;
	physic->addImpulse(force);

	blendframes[0] = &keyframe;
	blendframes[1] = &mesh->animation_clips.at((int)State::Jump).sequence.at(0);
	playAnimation((int)State::Jump, false, 0.05f);
	WalkSE->Stop();
}

void PLAYER::updateJump(float elapsed_time)
{
	if(frame_index <= 25)
		InputMove(elapsed_time);
	if(frame_index == mesh->animation_clips.at((int)State::Jump).sequence.size() - 1)
		toIdle();
}


