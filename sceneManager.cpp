#include "sceneManager.h"

void SceneManager::Update(float elapsed_time)
{
	if (next_scene != nullptr)
	{
		// 古いシーンの終了処理
		Clear();
		// 新しいシーンを設定
		current_scene = next_scene;
		next_scene = nullptr;
		//二重初期化を防ぐ
		if (current_scene->isInit == false)
		{
			current_scene->Init();
		}
	}

	if (current_scene != nullptr)
	{
		// シーンの更新処理
		current_scene->Update(elapsed_time);
	}
}

void SceneManager::Draw(float elapsed_time)
{
	if (current_scene != nullptr)
	{
		// シーンの描画処理
		current_scene->Draw(elapsed_time);
	}
}

void SceneManager::Clear()
{
	if (current_scene != nullptr)
	{
		// シーンの終了処理
		current_scene->Uninit();
		delete current_scene;
		current_scene = nullptr;
	}
}

void SceneManager::ChangeScene(Scene* scene)
{
	next_scene = scene;
}
