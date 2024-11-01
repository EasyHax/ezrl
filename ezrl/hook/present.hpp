#pragma once

#include "windows.h"

#include "d3d11.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

ID3D11Device*           g_device    = NULL;
ID3D11DeviceContext*    g_context   = NULL;
IDXGISwapChain*         g_swapchain = NULL;
ID3D11RenderTargetView* g_render    = NULL;

bool g_drawing           = false;
bool g_show_menu         = false;
bool g_imgui_initialized = false;

HWND    g_window   = NULL;
WNDPROC g_o_window = NULL;

typedef HRESULT( __stdcall* t_Present )( IDXGISwapChain*, UINT, UINT );
t_Present o_present;
t_Present o_present_addr;

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND, UINT, WPARAM, LPARAM );
extern HRESULT __fastcall hk_present( IDXGISwapChain* g_pSwapChain, UINT syncInterval, UINT flags );

LRESULT CALLBACK handle_wdn_proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	ImGuiIO& io = ImGui::GetIO();
	POINT mPos;

	GetCursorPos( &mPos );
	ScreenToClient( g_window, &mPos );

	ImGui::GetIO().MousePos.x = mPos.x;
	ImGui::GetIO().MousePos.y = mPos.y;

	if ( uMsg == WM_KEYUP ) {
		if ( wParam == VK_INSERT ) {
			g_show_menu = !g_show_menu;
		}
		if ( wParam == VK_HOME ) {
			g_drawing = !g_drawing;
		}
	}

	if ( g_show_menu ) {
		ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam );
		return TRUE;
	}

	return CallWindowProc( g_o_window, hWnd, uMsg, wParam, lParam );
}

void imgui_release( void ) {

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr( g_window, GWLP_WNDPROC, (LONG_PTR)g_o_window );

	g_swapchain->Release();
	g_device->Release();
	g_context->Release();
}

bool imgui_initialize( IDXGISwapChain* g_pSwapChain ) {

	if ( FAILED( g_pSwapChain->GetDevice( __uuidof( ID3D11Device ), (void**)&g_device ) ) ) {
		return false;
	}

	DXGI_SWAP_CHAIN_DESC sd;
	ID3D11Texture2D* pBackBuffer;

	g_device->GetImmediateContext( &g_context );
	g_pSwapChain->GetDesc( &sd );
	g_window = sd.OutputWindow;
	g_o_window = (WNDPROC)SetWindowLongPtr( g_window, GWLP_WNDPROC, (LONG_PTR)handle_wdn_proc );

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
	g_device->CreateRenderTargetView( pBackBuffer, NULL, &g_render );
	pBackBuffer->Release();

	ImGui_ImplWin32_Init( g_window );
	ImGui_ImplDX11_Init( g_device, g_context );
	ImGui::GetIO().ImeWindowHandle = g_window;

	g_imgui_initialized = true;
	return true;
}

t_Present get_present( void ) {

	auto ft_lvl = D3D_FEATURE_LEVEL{};
	auto sc_dsc = DXGI_SWAP_CHAIN_DESC{ 0 };

	sc_dsc.BufferCount = 1;
	sc_dsc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_dsc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sc_dsc.BufferDesc.Height = 800;
	sc_dsc.BufferDesc.Width = 600;
	sc_dsc.BufferDesc.RefreshRate = { 60, 1 };
	sc_dsc.OutputWindow = GetForegroundWindow();
	sc_dsc.Windowed = TRUE;
	sc_dsc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sc_dsc.SampleDesc.Count = 1;
	sc_dsc.SampleDesc.Quality = 0;

	if ( FAILED( D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &sc_dsc, &g_swapchain, &g_device, &ft_lvl, NULL ) ) ) {
		return NULL;
	}

	return reinterpret_cast<t_Present>( ( *(void***)g_swapchain )[8] );
}

bool hook_present( void ) {

	o_present_addr = get_present();

	if ( MH_CreateHook( o_present_addr, (void*)hk_present, (void**)&o_present ) != MH_OK ) {
		return false;
	}

	return MH_EnableHook( o_present_addr ) == MH_OK;
}

bool unhook_present( void ) {

	if ( MH_DisableHook( o_present_addr ) != MH_OK ) {
		return false;
	}

	imgui_release();

	return MH_RemoveHook( o_present_addr ) == MH_OK;
}