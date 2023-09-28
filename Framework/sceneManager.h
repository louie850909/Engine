#pragma once
#include "scene.h"

class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	void Update(float elapsed_time);
	void Draw(float elapsed_time);
	void Clear();
	void ChangeScene(Scene* scene);

private:
	Scene* current_scene = nullptr;
	Scene* next_scene = nullptr;
};