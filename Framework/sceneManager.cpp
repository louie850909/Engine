#include "sceneManager.h"

void SceneManager::Update(float elapsed_time)
{
	if (next_scene != nullptr)
	{
		// �Â��V�[���̏I������
		Clear();
		// �V�����V�[����ݒ�
		current_scene = next_scene;
		next_scene = nullptr;
		//��d��������h��
		if (current_scene->isInit == false)
		{
			current_scene->Init();
		}
	}

	if (current_scene != nullptr)
	{
		// �V�[���̍X�V����
		current_scene->Update(elapsed_time);
	}
}

void SceneManager::Draw(float elapsed_time)
{
	if (current_scene != nullptr)
	{
		// �V�[���̕`�揈��
		current_scene->Draw(elapsed_time);
	}
}

void SceneManager::Clear()
{
	if (current_scene != nullptr)
	{
		// �V�[���̏I������
		current_scene->Uninit();
		delete current_scene;
		current_scene = nullptr;
	}
}

void SceneManager::ChangeScene(Scene* scene)
{
	next_scene = scene;
}
