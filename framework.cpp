#include "framework.h"
#include "EffectManager.h"
#include "sceneManager.h"
#include "sceneLogo.h"

framework::framework(HWND hwnd, HINSTANCE hinstance) : 
	hwnd(hwnd) , 
	hinstance(hinstance),
	input(hwnd)
{
}

bool framework::initialize()
{
	HRESULT hr{ S_OK };

	camera = std::make_unique<CAMERA>();
	camera->initialize();

	Render.initialize();
	Render.set();

	Audio::Instance();

	Physic::getInstance();
	
	EffectManager::Instance().Init(&Render);
	
	SceneManager::Instance().ChangeScene(new SceneLogo(&Render));

	return true;
}

void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
	input.Update();
	camera->update(elapsed_time);

	SceneManager::Instance().Update(elapsed_time);
}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	// 別のスレッド中に同時にDevice contextをアクセスしないようにする
	std::lock_guard<std::mutex> lock(Render.get_mutex());
	SceneManager::Instance().Draw(elapsed_time);
	Render.present();
}

bool framework::uninitialize()
{
	EffectManager::Instance().Uninit();
	return true;
}

framework::~framework()
{
}