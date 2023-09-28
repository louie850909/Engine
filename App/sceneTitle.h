#pragma once
#include "../Framework/scene.h"
#include "../Framework/sprite.h"
#include "../Framework/Font.h"
#include "../Framework/render.h"
#include "../Framework/AudioSource.h"
#include "../Framework/Audio.h"

class SceneTitle : public Scene
{
public:
	SceneTitle(render* Render);
	~SceneTitle() override {}

	void Init() override;
	void Update(float elapsed_time) override;
	void Draw(float elapsed_time) override;
	void Uninit() override;

private:
	std::unique_ptr<sprite> Sprite;
	std::unique_ptr<Font> Fonts;
	render* Render;

	std::unique_ptr<AudioSource> BGM;
};