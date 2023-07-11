#include "sceneGame.h"
#include "sceneManager.h"
#include "sceneLogo.h"
#include "EffectManager.h"

SceneGame::SceneGame(render* Render)
{
	this->Render = Render;
}

void SceneGame::Init()
{
	sprites[0] = std::make_unique<sprite>(*Render, L".\\resources\\screenshot.jpg", XMFLOAT2(0, 0), (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);

	light = std::make_unique<LIGHT>();
	light->initialize();

	player = std::make_unique<PLAYER>(Render);
	player->initialize();

	stage = std::make_unique<STAGE>(Render);
	stage->initialize();

	fonts[0] = std::make_unique<Font>(Render->get_device(), ".\\resources\\fonts\\test\\test.fnt", 1024);
	
	isInit = true;
}

void SceneGame::Update(float elapsed_time)
{
	light->update();
	player->update(elapsed_time);
	EffectManager::Instance().Update(elapsed_time);

	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() == gamePad.BTN_START)
	{
		SceneManager::Instance().ChangeScene(new SceneLogo(Render));
	}
}

void SceneGame::Draw(float elapsed_time)
{
	float clear_color[]{ 0.3f, 0.3f, 0.3f, 1.0f };
	Render->clear(clear_color);
	/*�w�i�`��*/
	sprites[0].get()->renderTopLeft(*Render, 1, 1, 1, 1, 0);

	/*3D�`��*/
	CAMERA::Instance().SetCameraAT(XMFLOAT3(player->position.x, player->position.y + 10.0f, player->position.z));
	Render->set_view_projection_matrix(&CAMERA::Instance(), light.get());

	player->draw(elapsed_time);
	stage->draw(elapsed_time);

	/*�G�t�F�N�g�`��*/
	XMFLOAT4X4 v, p;
	XMStoreFloat4x4(&v, Render->get_view_matrix());
	XMStoreFloat4x4(&p, Render->get_projection_matrix());
	EffectManager::Instance().Draw(v,p);

#ifdef _DEBUG
	player->drawDebugPrimitive();
	Render->get_debug_renderer()->Render(Render->get_immediate_context(), v, p);
#endif // DEBUG


	/*UI�`��*/
	fonts[0]->Begin(Render->get_immediate_context());
	fonts[0]->Draw(0, 0, L"HELLO WORLD!", 3.0f);
	fonts[0]->Draw(800, 0, L"����FWASD\n�J�����F�����L�[\n�W�����v�F��Shift\n�J����Zoom in/out�F��control/�X�y�[�X", 1.0f);
	fonts[0]->End(Render->get_immediate_context());
}

void SceneGame::Uninit()
{
	light.release();
	player.release();
	stage.release();
	for (int i = 0; i < 8; i++)
	{
		sprites[i].release();
		fonts[i].release();
	}
	if (Render != nullptr)
	{
		Render = nullptr;
	}	
}
