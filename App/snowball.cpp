#include "snowball.h"
#include "../Framework/modelManager.h"

Snowball::Snowball(render* Render) : Character(Render)
{
	this->Render = Render;
}

Snowball::~Snowball()
{

}

void Snowball::initialize(XMFLOAT3 pos, float radius)
{
	position = pos;
	drawPos = XMFLOAT3(position.x, position.y + radius, position.z);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(radius, radius, radius);
	this->radius = radius;

	mesh = ModelManager::getInstance().LoadSkinnedMesh(".\\resources\\snowball.fbx", Render, false, drawPos, scale, rotation, 0.0f);
	collision_radius = radius;
	collision_height = 2 * radius;

	physic = std::make_unique<Physic>();
}

void Snowball::update(float elapsed_time)
{
	prePos = position;

	rotation.x = fabsf(physic->velocity.x) * -1;

	physic->updateVelocity(elapsed_time, isGround);
	position = physic->updateMove(position, elapsed_time);

#ifdef _DEBUG
	ImGui::Begin("Snowball");
	ImGui::Text("position: %f, %f, %f", position.x, position.y, position.z);
	ImGui::Text("velocity: %f, %f, %f", physic->velocity.x, physic->velocity.y, physic->velocity.z);
	ImGui::Text("rotation: %f, %f, %f", rotation.x, rotation.y, rotation.z);
	ImGui::End();
#endif // _DEBUG

}

void Snowball::draw(float elapsed_time)
{
	drawPos = XMFLOAT3(position.x, position.y + radius, position.z);
	mesh->position = drawPos;
	mesh->rotation = rotation;
	mesh->rotation.y += XM_PI;
	mesh->scale = scale;

	animation::keyframe keyframe;
	mesh->draw(*Render, XMFLOAT4(1, 1, 1, 1), skinned_mesh::LHS_YUP, &keyframe);
}

void Snowball::uninitialize()
{
}

void Snowball::drawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Render->get_debug_renderer();

	// デバッグ用を描画する
	debugRenderer->DrawCylinder(position, collision_radius, collision_height, XMFLOAT4(0, 0, 0, 1));
}
