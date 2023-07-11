#include "sceneLogo.h"
#include "sceneTitle.h"
#include "sceneManager.h"
#include "Input.h"

SceneLogo::SceneLogo(render* Render)
{
	this->Render = Render;
}

void SceneLogo::Init()
{
	Logo = std::make_unique<sprite>(*Render, L".\\resources\\crescendo mark.png", XMFLOAT2(512, 232), 256.0f, 256.0f);
	Black = std::make_unique<sprite>(*Render, L".\\resources\\black.png", XMFLOAT2(0,0), (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
	Fonts = std::make_unique<Font>(Render->get_device(), ".\\resources\\fonts\\stand\\stand.fnt", 1024);
	Time = 0.0f;
	isInit = true;
}

void SceneLogo::Update(float elapsed_time)
{
	if (Time >= 5.0f)
	{
		SceneManager::Instance().ChangeScene(new SceneTitle(Render));
	}

	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() == gamePad.BTN_START)
	{
		SceneManager::Instance().ChangeScene(new SceneTitle(Render));
	}
	Time += elapsed_time;
}

void SceneLogo::Draw(float elapsed_time)
{
	float clear_color[]{ 0.0f, 0.0f, 0.0f, 1.0f };
	Render->clear(clear_color);

	// 最初の2.5秒はフェードイン
	if (Time <= 2.5f)
	{
		Logo->renderTopLeft(*Render, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
		Fonts->Begin(Render->get_immediate_context());
		Fonts->Draw(450, 520, L"Powered by Cresc. Engine", 1);
		Fonts->End(Render->get_immediate_context());
		Black->renderTopLeft(*Render, 0.0f, 0.0f, 0.0f, 1.0f - Time / 2.5f, 0.0f);
	}

	// 2.5秒から5秒までフェードアウト
	else if (Time <= 5.0f)
	{
		Logo->renderTopLeft(*Render, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
		Fonts->Begin(Render->get_immediate_context());
		Fonts->Draw(450, 520, L"Powered by Cresc. Engine", 1);
		Fonts->End(Render->get_immediate_context());
		Black->renderTopLeft(*Render, 0.0f, 0.0f, 0.0f, (Time - 2.5f) / 2.5f, 0.0f);
	}
}

void SceneLogo::Uninit()
{
	Logo.release();
	Black.release();
	Fonts.release();
}
