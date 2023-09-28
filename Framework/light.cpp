#include "light.h"

LIGHT::LIGHT()
{
}

LIGHT::~LIGHT()
{
}

void LIGHT::initialize()
{
	position = { 100.0f, 100.0f, 100.0f };
	direction = { -1.0f, -1.0f, 1.0f };
	up = { 0.0f, 1.0f, 0.0f };
}

void LIGHT::update()
{
	// Imgui‚ÅŒõü‚ÌˆÊ’u‚ğ•ÏX‚Å‚«‚é‚æ‚¤‚É‚·‚é
#ifdef USE_IMGUI
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Position", &position.x, -1000.0f, 1000.0f);
	ImGui::SliderFloat3("Direction", &direction.x, -1.0f, 1.0f);
	ImGui::SliderFloat3("Up", &up.x, -1000.0f, 1000.0f);
	ImGui::End();
#endif // USE_IMGUI
}
