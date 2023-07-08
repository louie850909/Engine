#include "sceneLoading.h"
#include "sceneManager.h"
#include "imgui/imgui.h"
#include "sceneGame.h"

SceneLoading::SceneLoading(render* Render, Scene* nextScene)
{
	this->Render = Render;
	this->nextScene = nextScene;
}

void SceneLoading::Init()
{
	Sprite = std::make_unique<sprite>(*Render, L".\\resources\\LoadingIcon.png", XMFLOAT2(890, 630), 80.0f, 80.0f);
	font = std::make_unique<Font>(Render->get_device(), ".\\resources\\fonts\\bold\\bold.fnt", 1024);
	angle = 0.0f;
	ready = false;

	// ロードスレッドを開始
	thread = new std::thread(LoadingThread, this);
	this->isInit = true;
}

void SceneLoading::Update(float elapsed_time)
{
	constexpr float speed = 180;
	angle += speed * elapsed_time;

	// ロードが完了したら次のシーンへ
	if (IsReady())
	{
		SceneManager::Instance().ChangeScene(nextScene);
	}
}

void SceneLoading::Draw(float elapsed_time)
{
	float clear_color[]{ 0.0f, 0.0f, 1.0f, 1.0f };
	Render->clear(clear_color);

	Sprite->renderTopLeft(*Render, 1, 0, 1, 1, angle);
	font->Begin(Render->get_immediate_context());
	font->Draw(970, 640, L"LOADING...", 1);
	font->End(Render->get_immediate_context());
}

void SceneLoading::Uninit()
{
	// ロードスレッド終了化
	thread->join();
	delete thread;
	thread = nullptr;

	Sprite.release();
	font.release();
	if (Render != nullptr)
	{
		Render = nullptr;
	}
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	// COM関連の初期化であるCoInitializeを呼び出す
	CoInitialize(nullptr);

	// 次のシーンの初期化
	scene->nextScene->Init();

	// COM関連の終了処理であるCoUninitializeを呼び出す
	CoUninitialize();

	// ロード完了を設定
	scene->SetReady();
}
