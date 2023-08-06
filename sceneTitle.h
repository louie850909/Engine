#pragma once
#include "scene.h"
#include "sprite.h"
#include "Font.h"
#include "render.h"
#include "AudioSource.h"
#include "Audio.h"

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

	std::shared_ptr<AudioSource> BGM;
};