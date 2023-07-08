#pragma once
#include "scene.h"
#include "sprite.h"
#include "render.h"
#include "Font.h"
#include <thread>

class SceneLoading : public Scene
{
public:
	SceneLoading(render* Render, Scene* nextScene);
	~SceneLoading() override {}

	void Init() override;
	void Update(float elapsed_time) override;
	void Draw(float elapsed_time) override;
	void Uninit() override;

	// ���[�h�������������ǂ���
	bool IsReady() const { return ready; }
	// ���[�h�������ݒ�
	void SetReady() { this->ready = true; }
private:
	static void LoadingThread(SceneLoading* scene);
	
	render* Render;
	std::unique_ptr<sprite> Sprite;
	std::unique_ptr<Font> font;
	Scene* nextScene = nullptr;
	std::thread* thread = nullptr;
	float angle;
	bool ready;
};