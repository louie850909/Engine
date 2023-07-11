#pragma once
#include "scene.h"
#include "sprite.h"
#include "Font.h"
#include "render.h"

class SceneLogo : public Scene
{
public:
	SceneLogo(render* Render);
	~SceneLogo() override {}

	void Init() override;
	void Update(float elapsed_time) override;
	void Draw(float elapsed_time) override;
	void Uninit() override;

private:
	std::unique_ptr<sprite> Logo;
	std::unique_ptr<sprite> Black;
	std::unique_ptr<Font> Fonts;
	render* Render;
	float Time;
};
