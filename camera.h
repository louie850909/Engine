#pragma once
#include <DirectXMath.h>
#include <wrl.h>
#include "DirectXTK\Inc\Keyboard.h"
using namespace DirectX;

#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif // USE_IMGUI


class CAMERA
{
public:

	CAMERA();
	~CAMERA();

	// インスタンス取得
	static CAMERA& Instance() { return *instance; }

	void initialize();
	void update(float elapsed_time);

	void SetCameraAT(XMFLOAT3 pos);

	XMFLOAT3 get_position() const{ return position; }
	XMFLOAT3 get_target() const{ return target; }
	XMFLOAT3 get_up() const{ return up; }
	XMFLOAT3 get_rotation() const{ return rotation; }
private:
	static CAMERA* instance;
	XMFLOAT3	position;
	XMFLOAT3	target;
	XMFLOAT3	up;
	XMFLOAT3	rotation;
	float		length;
};