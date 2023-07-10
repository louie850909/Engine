#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "misc.h"
#include "camera.h"
#include "light.h"
#include "DebugRenderer.h"
#include <mutex>

#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

using namespace DirectX;
using namespace std;

CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"ENGINE" };

class render
{
public:
	struct scene_constants
	{
		DirectX::XMFLOAT4X4 view_projection;      //�r���[�E�v���W�F�N�V�����ϊ��s��
		DirectX::XMFLOAT4 light_direction;        //���C�g�̌���
		DirectX::XMFLOAT4 camera_position;		  //�J�����̈ʒu
	};

	enum DepthMode
	{
		// �[�x�e�X�g�F�I���A�[�x���C�g�F�I��
		DEPTH_TEST_ON_DEPTH_WRITE_ON,
		// �[�x�e�X�g�F�I���A�[�x���C�g�F�I�t
		DEPTH_TEST_ON_DEPTH_WRITE_OFF,
		// �[�x�e�X�g�F�I�t�A�[�x���C�g�F�I��
		DEPTH_TEST_OFF_DEPTH_WRITE_ON,
		// �[�x�e�X�g�F�I�t�A�[�x���C�g�F�I�t
		DEPTH_TEST_OFF_DEPTH_WRITE_OFF,
	};

	enum BlendMode
	{
		// OFF
		BLEND_OFF,
		// ���u�����h
		BLEND_ALPHA,
		// ���Z�u�����h
		BLEND_ADD,
		// ���Z�u�����h
		BLEND_SUBTRACT,
	};

	enum SamplerMode
	{
		// �_�t�B���^�����O
		SAMPLER_POINT,
		// ���`�t�B���^�����O
		SAMPLER_LINEAR,
		// �ٕ����t�B���^�����O
		SAMPLER_ANISOTROPIC,
	};

	enum RasterizerMode
	{
		// �ʏ�
		RASTERIZER_NORMAL,
		// ���C���[�t���[��
		RASTERIZER_WIREFRAME,
		// �J�����O�Ȃ�
		RASTERIZER_CULL_NONE,
	};

	CONST HWND hwnd;
	render(HWND hwnd);
	~render();

	void initialize();

	void clear(const FLOAT color[4]);
	void set();
	void present();

	void set_depth_stencil_state(int index);
	void set_blend_state(int index);
	void set_sampler_state(int index, int slot);
	void set_rasterizer_state(int index);

	void set_view_projection_matrix(CAMERA* camera, LIGHT* light);
	void set_2D_view_projection_matrix();

	ID3D11Device* get_device() const { return device.Get(); }
	ID3D11DeviceContext* get_immediate_context() const { return immediate_context.Get(); }
	IDXGISwapChain* get_swap_chain() const { return swap_chain.Get(); }

	DebugRenderer* get_debug_renderer() const { return debug_renderer; }

	XMMATRIX get_view_matrix() const { return viewMatrix; }
	XMMATRIX get_projection_matrix() const { return projectionMatrix; }

	std::mutex& get_mutex() { return *mutex; }

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[4];
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[3];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[3];

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	DebugRenderer* debug_renderer;

	std::mutex* mutex = new std::mutex();
};