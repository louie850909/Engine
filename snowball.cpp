#include "snowball.h"
#include "modelManager.h"

Snowball::Snowball(render* Render)
{
	this->Render = Render;
}

Snowball::~Snowball()
{

}

void Snowball::initialize(XMFLOAT3 pos, float radius)
{
	position = XMFLOAT3(pos.x, pos.y + radius, pos.z);
	scale = XMFLOAT3(radius, radius, radius);
	this->radius = radius;

	mesh = ModelManager::getInstance().LoadSkinnedMesh(".\\resources\\snowball.fbx", Render, false, position, scale, rotation, 0.0f);
	collision_radius = radius;
	collision_height = 2 * radius;
}

void Snowball::update(float elapsed_time)
{
}

void Snowball::draw(float elapsed_time)
{
	animation::keyframe keyframe;
	mesh->draw(*Render, XMFLOAT4(1, 1, 1, 1), skinned_mesh::LHS_YUP, &keyframe);

#ifdef _DEBUG
	drawDebugPrimitive();
#endif // _DEBUG

}

void Snowball::uninitialize()
{
}

void Snowball::drawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Render->get_debug_renderer();

	// デバッグ用を描画する
	debugRenderer->DrawCylinder(XMFLOAT3(position.x, position.y - radius, position.z), collision_radius, collision_height, XMFLOAT4(0, 0, 0, 1));
}
