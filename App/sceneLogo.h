#pragma once
#include "../Framework/scene.h"
#include "../Framework/sprite.h"
#include "../Framework/Font.h"
#include "../Framework/render.h"

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
