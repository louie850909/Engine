#include "sceneGame.h"
#include "sceneManager.h"
#include "sceneLogo.h"
#include "EffectManager.h"
#include "collision.h"

SceneGame::SceneGame(render* Render)
{
	this->Render = Render;
}

void SceneGame::Init()
{
	sprites[0] = std::make_unique<sprite>(*Render, L".\\resources\\screenshot.jpg");
	billboards[0] = std::make_unique<Billboard>(*Render, L".\\resources\\screenshot.jpg", XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), 1.0f, 1.0f, true, 1000);
	for (int i = 0; i < billboards[0]->instances.size(); i++)
	{
		billboards[0]->instances[i].pos.x = i * 1;
	}

	Fountains[0] = std::make_unique<Fountain>(*Render, 1.0f, 1.0f, 1000);
	rains[0] = std::make_unique<Rain>(*Render, 0.05f, 1.0f, 4000);
	fires[0] = std::make_unique<Fire>(*Render, 1.0f, 1.0f, XMFLOAT3(110, 0, 10), 500);
	fires[1] = std::make_unique<Fire>(*Render, 1.0f, 1.0f, XMFLOAT3(110, 0, -30), 500);

	light = std::make_unique<LIGHT>();
	light->initialize();

	player = std::make_unique<PLAYER>(Render);
	player->initialize();

	stage = std::make_unique<STAGE>(Render);
	stage->initialize();

	fonts[0] = std::make_unique<Font>(Render->get_device(), ".\\resources\\fonts\\test\\test.fnt", 1024);

	BGM = Audio::Instance().LoadAudioSource(".\\resources\\BGM\\BGM_Game.wav");
	BGM->Play(true);
	
	isInit = true;
}

void SceneGame::Update(float elapsed_time)
{
	std::thread threads[8];

	light->update();
	player->update(elapsed_time);
	Fountains[0]->update(elapsed_time);
	rains[0]->update(elapsed_time);
	fires[0]->update(elapsed_time);
	fires[1]->update(elapsed_time);
	EffectManager::Instance().Update(elapsed_time);

	CollosionUpdate(elapsed_time);

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
	/*背景描画*/
	sprites[0].get()->renderTopLeft(*Render, XMFLOAT2(0, 0), (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 1, 1, 1, 1, 0);

	/*3D描画*/
	CAMERA::Instance().SetCameraAT(XMFLOAT3(player->position.x, player->position.y + 10.0f, player->position.z));
	Render->set_view_projection_matrix(&CAMERA::Instance(), light.get());

	player->draw(elapsed_time);
	stage->draw(elapsed_time);
	billboards[0]->draw(*Render);
	Fountains[0]->draw(*Render);
	rains[0]->draw(*Render);
	fires[0]->draw(*Render);
	fires[1]->draw(*Render);

	/*エフェクト描画*/
	XMFLOAT4X4 v, p;
	XMStoreFloat4x4(&v, Render->get_view_matrix());
	XMStoreFloat4x4(&p, Render->get_projection_matrix());
	EffectManager::Instance().Draw(v,p);

#ifdef _DEBUG
	player->drawDebugPrimitive();
	Render->get_debug_renderer()->Render(Render->get_immediate_context(), v, p);
#endif // DEBUG


	/*UI描画*/
	fonts[0]->Begin(Render->get_immediate_context());
	fonts[0]->Draw(0, 0, L"HELLO WORLD!", 3.0f);
	fonts[0]->Draw(800, 0, L"操作：WASD\nカメラ：方向キー\nジャンプ：左Shift\nカメラZoom in/out：左control/スペース", 1.0f);
	fonts[0]->End(Render->get_immediate_context());
}

void SceneGame::Uninit()
{
	BGM->Stop();
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

void SceneGame::CollosionUpdate(float elapsed_time)
{
	// プレイヤーとステージの当たり判定
	{
		HitResult player_hitResult;

		// プレイヤーの位置判定
		if(player->position.x < stage->center.x && player->position.y < stage->center.y && player->position.z < stage->center.z)
			player->placeIndex = 0;
		else if (player->position.x > stage->center.x && player->position.y < stage->center.y && player->position.z < stage->center.z)
			player->placeIndex = 1;
		else if (player->position.x < stage->center.x && player->position.y > stage->center.y && player->position.z < stage->center.z)
			player->placeIndex = 2;
		else if (player->position.x > stage->center.x && player->position.y > stage->center.y && player->position.z < stage->center.z)
			player->placeIndex = 3;
		else if (player->position.x < stage->center.x && player->position.y < stage->center.y && player->position.z > stage->center.z)
			player->placeIndex = 4;
		else if (player->position.x > stage->center.x && player->position.y < stage->center.y && player->position.z > stage->center.z)
			player->placeIndex = 5;
		else if (player->position.x < stage->center.x && player->position.y > stage->center.y && player->position.z > stage->center.z)
			player->placeIndex = 6;
		else if (player->position.x > stage->center.x && player->position.y > stage->center.y && player->position.z > stage->center.z)
			player->placeIndex = 7;

		// 床判定
		if (Collision::VsStage(XMFLOAT3(player->position.x, player->position.y + 15.0f, player->position.z),
			XMFLOAT3(player->position.x, player->position.y - 1.0f, player->position.z), stage->subDivisions[player->placeIndex], player_hitResult))
		{
			player->position.y = player_hitResult.Pos.y;
		}
		else
		{
			player->position.y -= 9.8f * 10 * elapsed_time;
		}

		// 壁判定
		if (Collision::VsStage(XMFLOAT3(player->position.x, player->position.y + 10.0f, player->position.z),
			XMFLOAT3(player->position.x + sinf(player->rotation.y) * 3, player->position.y + 5.0f, player->position.z + cosf(player->rotation.y) * 3),
			stage->subDivisions[player->placeIndex], player_hitResult))
		{
			player->position = player->prePos;
		}
	}
}
