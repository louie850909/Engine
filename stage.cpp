#include "stage.h"
#include <comdef.h>
static STAGE* instance = nullptr;

STAGE::STAGE(render* Render)
{
	this->Render = Render;
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	instance = this;
}

STAGE::~STAGE()
{
}

STAGE& STAGE::Instance()
{
	return *instance;
}

void STAGE::initialize()
{
	//mesh = std::make_unique<static_mesh>(Render->get_device(), L".\\resources\\landscape.obj", position, scale, rotation, false);
	skinnedMesh = std::make_unique<skinned_mesh>(Render->get_device(), ".\\resources\\testStage\\ExampleStage.fbx", false, position, scale, rotation, 0.0f);
}

void STAGE::update(float elapsed_time)
{
}

void STAGE::draw(float elapsed_time)
{
	//mesh->draw(*Render);
	animation::keyframe keyframe;
	skinnedMesh->draw(*Render, XMFLOAT4(1, 1, 1, 1), skinned_mesh::LHS_YUP, &keyframe);
}

void STAGE::uninitialize()
{
}
