#include "sceneTitle.h"
#include "sceneLoading.h"
#include "sceneGame.h"
#include "sceneManager.h"
#include "Input.h"

SceneTitle::SceneTitle(render* Render)
{
	this->Render = Render;
}

void SceneTitle::Init()
{
	Sprite = std::make_unique<sprite>(*Render, L".\\resources\\black-metal-texture.jpg", XMFLOAT2(0, 0), (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
	Fonts = std::make_unique<Font>(Render->get_device(), ".\\resources\\fonts\\bold\\bold.fnt", 1024);
	isInit = true;
}

void SceneTitle::Update(float elapsed_time)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() == gamePad.BTN_START)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(Render, new SceneGame(Render)));
	}
}

void SceneTitle::Draw(float elapsed_time)
{
	float clear_color[]{ 0.3f, 0.3f, 0.3f, 1.0f };
	Render->clear(clear_color);

	Sprite->renderTopLeft(*Render, 1, 1, 1, 1, 0);
	Fonts->Begin(Render->get_immediate_context());
	Fonts->Draw(400, 250, L"TITLE", 3, XMFLOAT4(1,0,0,1));
	Fonts->Draw(320, 500, L"PRESS ENTER to START", 1);
	Fonts->End(Render->get_immediate_context());
}

void SceneTitle::Uninit()
{
	Sprite.release();
	Fonts.release();
	if (Render != nullptr)
	{
		Render = nullptr;
	}
}
