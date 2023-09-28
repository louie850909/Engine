#pragma once
#include <DirectXMath.h>
#include <wrl.h>
using namespace DirectX;

#ifdef USE_IMGUI
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif // USE_IMGUI

class LIGHT
{
public:
	LIGHT();
	~LIGHT();

	void initialize();
	void update();

	XMFLOAT3 get_position() const{ return position; }
	XMFLOAT3 get_direction() const{ return direction; }
	XMFLOAT3 get_up() const{ return up; }
private:
	XMFLOAT3 position;
	XMFLOAT3 direction;
	XMFLOAT3 up;
};