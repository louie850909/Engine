#include "render.h"

render::render(HWND hwnd) : hwnd(hwnd)
{
}

render::~render()
{
}

void render::initialize()
{
	// デバイス・デバイスコンテキスト・スワップチェーンの作成
	HRESULT hr{ S_OK };

	UINT create_device_flags{ 0 };
#ifdef _DEBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG

	D3D_FEATURE_LEVEL feature_levels[]{ D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;
	swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = hwnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = !FULLSCREEN;

	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, create_device_flags, feature_levels,
		ARRAYSIZE(feature_levels), D3D11_SDK_VERSION,
		&swap_chain_desc, swap_chain.GetAddressOf(), device.GetAddressOf(),
		nullptr, immediate_context.GetAddressOf());

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// レンダーターゲットビューの作成
	ID3D11Texture2D* back_buffer{};
	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	back_buffer->Release();

	// 深度ステンシルビューの作成
	ID3D11Texture2D* depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = SCREEN_WIDTH;
	texture2d_desc.Height = SCREEN_HEIGHT;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;

	hr = device->CreateTexture2D(&texture2d_desc, nullptr, &depth_stencil_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	depth_stencil_view_desc.Format = texture2d_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Flags = 0;

	hr = device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depth_stencil_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	depth_stencil_buffer->Release();

	// 深度ステンシルステートオブジェクトを作成する
	// 深度テスト：オン、深度ライト：オン
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[0]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度テスト：オン、深度ライト：オフ
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[1]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度テスト：オフ、深度ライト：オン
	depth_stencil_desc.DepthEnable = false;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[2]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度テスト：オフ、深度ライト：オフ
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[3]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ブレンディングステートオブジェクトを作成する
	// ブレンディング：OFF
	D3D11_BLEND_DESC blend_desc{};
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;
	blend_desc.RenderTarget[0].BlendEnable = false;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = device->CreateBlendState(&blend_desc, &blend_states[0]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// αブレンド
	blend_desc.RenderTarget[0].BlendEnable = true;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	hr = device->CreateBlendState(&blend_desc, &blend_states[1]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 加算ブレンド
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	hr = device->CreateBlendState(&blend_desc, &blend_states[2]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 減算ブレンド
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;

	hr = device->CreateBlendState(&blend_desc, &blend_states[3]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ビューポートの設定
	D3D11_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(SCREEN_WIDTH);
	viewport.Height = static_cast<float>(SCREEN_HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	immediate_context->RSSetViewports(1, &viewport);

	// サンプラーステートオブジェクトを生成する
	D3D11_SAMPLER_DESC sampler_desc{};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampler_desc.MipLODBias = 0.0f;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.MinLOD = 0.0f;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	sampler_desc.BorderColor[0] = 0.0f;
	sampler_desc.BorderColor[1] = 0.0f;
	sampler_desc.BorderColor[2] = 0.0f;
	sampler_desc.BorderColor[3] = 0.0f;

	hr = device->CreateSamplerState(&sampler_desc, &sampler_states[0]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = device->CreateSamplerState(&sampler_desc, &sampler_states[1]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	hr = device->CreateSamplerState(&sampler_desc, &sampler_states[2]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ラスタライザステートオブジェクトを生成する
	D3D11_RASTERIZER_DESC rasterizer_desc{};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = false;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0.0f;
	rasterizer_desc.SlopeScaledDepthBias = 0.0f;
	rasterizer_desc.DepthClipEnable = true;
	rasterizer_desc.ScissorEnable = false;
	rasterizer_desc.MultisampleEnable = false;
	rasterizer_desc.AntialiasedLineEnable = false;

	hr = device->CreateRasterizerState(&rasterizer_desc, &rasterizer_states[0]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	hr = device->CreateRasterizerState(&rasterizer_desc, &rasterizer_states[1]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	hr = device->CreateRasterizerState(&rasterizer_desc, &rasterizer_states[2]);

	// シーン定数バッファオブジェクトを生成する
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(scene_constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	debug_renderer = new DebugRenderer(device.Get());

	this->mutex = new std::mutex();
}

void render::clear(const FLOAT color[4])
{
	// バックバッファのクリア
	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
}

void render::set()
{
	// サンプラーステートオブジェクトをバインドする
	set_sampler_state(SAMPLER_POINT, 0);
	set_sampler_state(SAMPLER_LINEAR, 1);
	set_sampler_state(SAMPLER_ANISOTROPIC, 2);

	// 深度ステンシルステートオブジェクトを設定する
	set_depth_stencil_state(DEPTH_TEST_ON_DEPTH_WRITE_ON);

	// ブレンドステートオブジェクトを設定する
	set_blend_state(BLEND_ALPHA);

	// ラスタライザステートオブジェクトを設定する
	set_rasterizer_state(RASTERIZER_NORMAL);
}

void render::present()
{
#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	// バックバッファへの描画
	UINT sync_interval{ 0 };
	swap_chain->Present(sync_interval, 0);
}

void render::set_depth_stencil_state(int index)
{
	immediate_context->OMSetDepthStencilState(depth_stencil_states[index].Get(), 0);
}

void render::set_blend_state(int index)
{
	immediate_context->OMSetBlendState(blend_states[index].Get(), nullptr, 0xffffffff);
}

void render::set_sampler_state(int index, int slot)
{
	immediate_context->PSSetSamplers(slot, 1, sampler_states[index].GetAddressOf());
}

void render::set_rasterizer_state(int index)
{
	immediate_context->RSSetState(rasterizer_states[index].Get());
}

void render::set_view_projection_matrix(CAMERA* camera, LIGHT* light)
{
	// ビュー・プロジェクション変換行列を計算し、定数バッファにセットする
	D3D11_VIEWPORT viewport;
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	float aspect_ratio{ viewport.Width / viewport.Height };
	XMMATRIX P{ XMMatrixPerspectiveFovLH(XMConvertToRadians(30), aspect_ratio, 0.01f, 10000.0f) };
	projectionMatrix = P;

	XMVECTOR eye{ XMVectorSet(camera->get_position().x, camera->get_position().y, camera->get_position().z, 1.0f) };
	XMVECTOR focus{ XMVectorSet(camera->get_target().x, camera->get_target().y, camera->get_target().z, 1.0f) };
	XMVECTOR up{ XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };

	XMMATRIX V{ XMMatrixLookAtLH(eye, focus, up) };
	viewMatrix = V;

	scene_constants data{};
	XMStoreFloat4x4(&data.view, V);
	XMStoreFloat4x4(&data.projection, P);
	data.light_direction = XMFLOAT4(light->get_direction().x, light->get_direction().y, light->get_direction().z, 0.0f);
	data.camera_position = XMFLOAT4(camera->get_position().x, camera->get_position().y, camera->get_position().z, 0.0f);
	immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
	immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
	immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
}

void render::set_2D_view_projection_matrix()
{
	// ビュー・プロジェクション変換行列を計算し、定数バッファにセットする
	D3D11_VIEWPORT viewport;
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	float aspect_ratio{ viewport.Width / viewport.Height };
	XMMATRIX P{ XMMatrixOrthographicOffCenterLH(0.0f, viewport.Width, viewport.Height, 0, 0, 1) };

	XMMATRIX V{ XMMatrixIdentity() };

	scene_constants data{};
	XMStoreFloat4x4(&data.view, V);
	XMStoreFloat4x4(&data.projection, P);
	data.light_direction = { 0, 0, 0, 0 };
	immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
	immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
}
