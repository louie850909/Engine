#pragma once
#include "scene.h"
#include "render.h"
#include "Font.h"
#include "player.h"
#include "stage.h"
#include "sprite.h"
#include "billboard.h"
#include "fountain.h"
#include "rain.h"
#include "fire.h"
#include "Audio.h"
#include "AudioResource.h"

class SceneGame : public Scene
{
public:
	SceneGame(render* Render);
	~SceneGame() override {}

	void Init() override;
	void Update(float elapsed_time) override;
	void Draw(float elapsed_time) override;
	void Uninit() override;

	void CollosionUpdate(float elapsed_time);

private:
	render* Render;

	std::unique_ptr<LIGHT> light;
	std::unique_ptr<sprite> sprites[8];
	std::unique_ptr<Billboard> billboards[8];
	std::unique_ptr<Fountain> Fountains[8];
	std::unique_ptr<Rain> rains[8];
	std::unique_ptr<Fire> fires[8];
	std::unique_ptr<Font> fonts[8];
	std::unique_ptr<PLAYER> player;
	std::unique_ptr<STAGE> stage;
	std::unique_ptr<AudioSource> BGM;
};