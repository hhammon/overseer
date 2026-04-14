#include "core.hpp"
#include "arena.hpp"
#include "polling.hpp"
#include "windows.hpp"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "implot.h"

global HWnd                    window              = NULL;
global IDXGISwapChain*         swap_chain          = NULL;
global ID3D11Device*           device              = NULL;
global ID3D11DeviceContext*    device_context      = NULL;
global ID3D11RenderTargetView* main_rtv            = NULL;
global bool                    swap_chain_occluded = false;
global u32                     resize_width        = 0;
global u32                     resize_height       = 0;

internal void create_render_device();
internal void create_render_target();
internal void destroy_render_device();
internal void destroy_render_target();

internal s64 WINCALLBACK wnd_proc(
	HWnd window,
	u32 msg,
	u64 w_param,
	s64 l_param
);

internal void do_ui() {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin(
		"MainWindow",
		NULL,
		ImGuiWindowFlags_NoTitleBar            |
		ImGuiWindowFlags_MenuBar               |
		ImGuiWindowFlags_NoResize              |
		ImGuiWindowFlags_NoMove                |
		ImGuiWindowFlags_NoCollapse            |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus
	);

	if (ImPlot::BeginPlot("CPU", ImVec2(-1,ImGui::GetTextLineHeight()*10))) {
		ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoTickLabels, 0);


		ImPlot::EndPlot();
	}

	ImGui::End();
	ImGui::PopStyleVar(3);
}

int WINAPI wWinMain(
	HInstance instance,
	HInstance prev_instance,
	wchar_t   cmd_line,
	int       cmd_show
) {
	unused_var(prev_instance);
	unused_var(cmd_line);
	unused_var(cmd_show);

	scratch_init();

	ImGui_ImplWin32_EnableDpiAwareness();
	f32 main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(
		MonitorFromPoint(Point{}, MONITOR_DEFAULTTOPRIMARY)
	);

	// Create window
	WndClassExW wnd_class = { };
	wnd_class.size = sizeof(wnd_class);
	wnd_class.style = CS_CLASSDC;
	wnd_class.wnd_proc = wnd_proc;
	wnd_class.instance = instance;
	wnd_class.class_name = L"Overseer Window";
	RegisterClassExW(&wnd_class);
	window = CreateWindowExW(
		0,
		wnd_class.class_name,
		L"Overseer",
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		(s32)(1280 * main_scale),
		(s32)( 800 * main_scale),
		NULL,
		NULL,
		instance,
		NULL
	);

	create_render_device();

	ShowWindow(window, ShowWindowCommand_SHOWDEFAULT);
	UpdateWindow(window);

	// Begin UI through Dear ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.0f);

	// UI Loop
	bool done = false;
	while (!done) {
		Msg msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
			if (msg.message == WM_QUIT) {
				done = true;
			}
		}
		if (done) break;

		if (
			swap_chain_occluded &&
			swap_chain->vtbl->present(swap_chain, 0, DxgiPresentFlag_TEST) == DxgiStatus_OCCLUDED
		) {
			Sleep(10);
			continue;
		}
		swap_chain_occluded = false;

		if (resize_width != 0 && resize_height != 0) {
			destroy_render_target();
			swap_chain->vtbl->resize_buffers(swap_chain, 0, resize_width, resize_height, DxgiFormat_UNKNOWN, 0);
			resize_width = resize_height = 0;
			create_render_target();
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		do_ui();

		ImGui::Render();
		f32 clear_color_with_alpha[] = {
			clear_color.x * clear_color.w,
			clear_color.y * clear_color.w,
			clear_color.z * clear_color.w,
			clear_color.w
		};
		device_context->vtbl->om_set_render_targets(device_context, 1, &main_rtv, NULL);
		device_context->vtbl->clear_render_target_view(device_context, main_rtv, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		HResult res = swap_chain->vtbl->present(swap_chain, 1, 0);
		swap_chain_occluded = res == DxgiStatus_OCCLUDED;
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	destroy_render_device();
	DestroyWindow(window);

	return 0;
}

internal void create_render_device() {
	DxgiSwapChainDesc swap_chain_desc = { };
	swap_chain_desc.buffer_count                         = 2;
	swap_chain_desc.buffer_desc.format                   = DxgiFormat_R8G8B8A8_UNORM;
	swap_chain_desc.buffer_desc.refresh_rate.numerator   = 60;
	swap_chain_desc.buffer_desc.refresh_rate.denominator = 1;
	swap_chain_desc.flags                                = DxgiSwapChainFlag_ALLOW_MODE_SWITCH;
	swap_chain_desc.buffer_usage                         = DxgiUsage_RENDER_TARGET_OUTPUT;
	swap_chain_desc.output_window                        = window;
	swap_chain_desc.sample_desc.count                    = 1;
	swap_chain_desc.sample_desc.quality                  = 0;
	swap_chain_desc.windowed                             = true;
	swap_chain_desc.swap_effect                          = DxgiSwapEffect_DISCARD;

	D3DFeatureLevel feature_levels[] = {D3DFeatureLevel_11_0, D3DFeatureLevel_10_0};
	HResult res = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3DDriverType_HARDWARE,
		NULL,
		0,
		feature_levels,
		arrlen(feature_levels),
		D3D11_SDK_VERSION,
		&swap_chain_desc,
		&swap_chain,
		&device,
		NULL,
		&device_context
	);

	if (res == DxgiError_UNSUPPORTED) {
		// Retry with WARP
		res = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3DDriverType_WARP,
			NULL,
			0,
			feature_levels,
			arrlen(feature_levels),
			D3D11_SDK_VERSION,
			&swap_chain_desc,
			&swap_chain,
			&device,
			NULL,
			&device_context
		);
	}

	if (FAILED(res)) {
		ExitProcess(1);
	}

	create_render_target();
}

internal void create_render_target() {
	ID3D11Texture2D* back_buffer;
	swap_chain->vtbl->get_buffer(swap_chain, 0, COM_INTERFACE_OUT(ID3D11Texture2D, &back_buffer));
	device->vtbl->create_render_target_view(device, (ID3D11Resource*)back_buffer, NULL, &main_rtv);
	back_buffer->vtbl->_._._.release(back_buffer);
}

internal void destroy_render_device() {
	destroy_render_target();

	if (swap_chain) {
		swap_chain->vtbl->_._._.release(swap_chain);
		swap_chain = NULL;
	}

	if (device_context) {
		device_context->vtbl->_._.release(device_context);
		device_context = NULL;
	}

	if (device) {
		device->vtbl->_.release(device);
		device = NULL;
	}
}

internal void destroy_render_target() {
	if (main_rtv) {
		main_rtv->vtbl->_._._.release(main_rtv);
		main_rtv = NULL;
	}
}
extern s64 WINCALLBACK ImGui_ImplWin32_WndProcHandler(HWnd window, u32 msg, u64 w_param, s64 l_param);
internal s64 WINCALLBACK wnd_proc(
	HWnd window,
	u32 msg,
	u64 w_param,
	s64 l_param
) {
	if (ImGui_ImplWin32_WndProcHandler(window, msg, w_param, l_param)) return 1;

	switch (msg) {
	case WM_SIZE:
		if (w_param == SIZE_MINIMIZED) return 0;
		resize_width  = (u32)((l_param >>  0) & 0xffff);
		resize_height = (u32)((l_param >> 16) & 0xffff);
		return 0;
	case WM_SYSCOMMAND:
		if ((w_param & 0xfff0) == SC_KEYMENU) return false;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProcW(window, msg, w_param, l_param);
}

// Defined in Language Server config to prevent namespace pollution
#ifndef UNITY_BUILD_NO_IMPL
#include "arena.cpp"
#include "core.cpp"
#include "polling.cpp"
#endif