#include "core.hpp"
#include "arena.hpp"
#include "polling.hpp"
#include "windows.hpp"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "implot.h"

#include <stdlib.h>

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

internal void imgui_string(String str) {
	ImGui::TextUnformatted(str.ptr, str.ptr + str.len);
}
#define imgui_printf(fmt, ...) imgui_string(scratch_sprintf(fmt, ## __VA_ARGS__))

internal void imgui_string_right(String str) {
	f32 text_width = ImGui::CalcTextSize(str.ptr, str.ptr + str.len).x;
	f32 cell_width = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cell_width - text_width);
	ImGui::TextUnformatted(str.ptr, str.ptr + str.len);
}
#define imgui_printf_right(fmt, ...) imgui_string_right(scratch_sprintf(fmt, ## __VA_ARGS__))

internal StringZ format_timespan(f64 secs, Arena* arena) {
	bool negative = false;
	if (secs < 0) {
		negative = true;
		secs *= -1;
	}

	u32 seconds = (u32)(secs + 0.5);

	u32 minute = 60;
	u32 hour   = 60 * minute;
	u32 day    = 24 * hour;

	u32 days    = seconds / day;
	seconds    -= days * day;
	u32 hours   = seconds / hour;
	seconds    -= hours * hour;
	u32 minutes = seconds / minute;
	seconds    -= minutes * minute;

	if (days) {
		return arena_sprintf(arena, "%s%u:%02u:%02u:%02u", negative ? "-" : "", days, hours, minutes, seconds);
	} else {
		return arena_sprintf(arena, "%s%02u:%02u:%02u", negative ? "-" : "", hours, minutes, seconds);
	}
}
#define scratch_format_timespan(secs) format_timespan(secs, &scratch_arena)

internal void tab_performance() {
	if (ImPlot::BeginPlot("CPU Usage", ImVec2(-1, ImGui::GetTextLineHeight() * 15))) {
		CpuHistory   cpu_history = polling_get_cpu_history();
		CpuPercents* base        = cpu_history.base;
		u64          length      = cpu_history.length;

		ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoTickLabels, 0);
		ImPlot::SetupAxisLimits(ImAxis_X1, cpu_history.start, cpu_history.end, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);

		ImPlotSpec spec = {};
		spec.Offset = cpu_history.offset;
		spec.Stride = sizeof(cpu_history.base[0]);

		ImVec4 cpu_color    = ImVec4(0.2f, 0.8f, 1.0f, 1.0f);
		ImVec4 kernel_color = ImVec4(0.8f, 0.6f, 0.2f, 1.0f);

		spec.LineColor = cpu_color;
		ImPlot::PlotLine("All", &base->time, &base->cpu, length, spec);
		spec.LineColor = kernel_color;
		ImPlot::PlotLine("Kernel", &base->time, &base->kernel, length, spec);

		ImPlot::EndPlot();
	}

	if (ImPlot::BeginPlot("Memory Usage", ImVec2(-1, ImGui::GetTextLineHeight() * 15))) {
		MemoryHistory   memory_history = polling_get_memory_history();
		MemoryPercents* base           = memory_history.base;
		u64             length         = memory_history.length;

		ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoTickLabels, 0);
		ImPlot::SetupAxisLimits(ImAxis_X1, memory_history.start, memory_history.end, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);

		ImPlotSpec spec = {};
		spec.Offset = memory_history.offset;
		spec.Stride = sizeof(memory_history.base[0]);

		ImVec4 ram_color    = ImVec4(0.8f, 0.3f, 0.7f, 1.0f);
		ImVec4 swap_color   = ImVec4(0.8f, 0.5f, 0.1f, 1.0f);
		ImVec4 commit_color = ImVec4(0.1f, 0.9f, 0.1f, 1.0f);

		spec.LineColor = ram_color;
		ImPlot::PlotLine("RAM", &base->time, &base->ram, length, spec);
		spec.LineColor = swap_color;
		ImPlot::PlotLine("Swap", &base->time, &base->swap, length, spec);
		spec.LineColor = commit_color;
		ImPlot::PlotLine("Commit", &base->time, &base->commit, length, spec);

		ImPlot::EndPlot();
	}
}

enum ProcessTableColumn {
	ProcessTableColumn_NAME,
	ProcessTableColumn_PID,
	ProcessTableColumn_RAM,
	ProcessTableColumn_COMMIT,
	ProcessTableColumn_UP_TIME,
	ProcessTableColumn_CPU_TIME,
	ProcessTableColumn_USER_TIME,
	ProcessTableColumn_KERNEL_TIME,
	ProcessTableColumn_THREADS,
	ProcessTableColumn_HANDLES,
	ProcessTableColumn_HARD_FAULTS,

	ProcessTableColumn__COUNT,
};

internal int cmp_u64(u64 a, u64 b) {
	if (a < b) return -1;
	if (a > b) return 1;
	return 0;
}

internal int cmp_f64(f64 a, f64 b) {
	if (a < b) return -1;
	if (a > b) return 1;
	return 0;
}

internal int __cdecl proc_cmp_image_name_asc(ProcessData** a, ProcessData** b) {
	return _stricmp((*a)->image_name, (*b)->image_name);
}
internal int __cdecl proc_cmp_image_name_desc(ProcessData** a, ProcessData** b) {
	return -_stricmp((*a)->image_name, (*b)->image_name);
}

internal int __cdecl proc_cmp_pid_asc(ProcessData** a, ProcessData** b) {
	return cmp_u64((*a)->pid, (*b)->pid);
}
internal int __cdecl proc_cmp_pid_desc(ProcessData** a, ProcessData** b) {
	return -cmp_u64((*a)->pid, (*b)->pid);
}

internal int __cdecl proc_cmp_ram_asc(ProcessData** a, ProcessData** b) {
	return cmp_u64((*a)->ram, (*b)->ram);
}
internal int __cdecl proc_cmp_ram_desc(ProcessData** a, ProcessData** b) {
	return -cmp_u64((*a)->ram, (*b)->ram);
}

internal int __cdecl proc_cmp_commit_asc(ProcessData** a, ProcessData** b) {
	return cmp_u64((*a)->commit, (*b)->commit);
}
internal int __cdecl proc_cmp_commit_desc(ProcessData** a, ProcessData** b) {
	return -cmp_u64((*a)->commit, (*b)->commit);
}

internal int __cdecl proc_cmp_uptime_asc(ProcessData** a, ProcessData** b) {
	return cmp_f64((*a)->uptime, (*b)->uptime);
}
internal int __cdecl proc_cmp_uptime_desc(ProcessData** a, ProcessData** b) {
	return -cmp_f64((*a)->uptime, (*b)->uptime);
}

internal int __cdecl proc_cmp_cpu_time_asc(ProcessData** a, ProcessData** b) {
	return cmp_f64((*a)->cpu_time, (*b)->cpu_time);
}
internal int __cdecl proc_cmp_cpu_time_desc(ProcessData** a, ProcessData** b) {
	return -cmp_f64((*a)->cpu_time, (*b)->cpu_time);
}

internal int __cdecl proc_cmp_user_time_asc(ProcessData** a, ProcessData** b) {
	return cmp_f64((*a)->user_time, (*b)->user_time);
}
internal int __cdecl proc_cmp_user_time_desc(ProcessData** a, ProcessData** b) {
	return -cmp_f64((*a)->user_time, (*b)->user_time);
}

internal int __cdecl proc_cmp_kernel_time_asc(ProcessData** a, ProcessData** b) {
	return cmp_f64((*a)->kernel_time, (*b)->kernel_time);
}
internal int __cdecl proc_cmp_kernel_time_desc(ProcessData** a, ProcessData** b) {
	return -cmp_f64((*a)->kernel_time, (*b)->kernel_time);
}

internal int __cdecl proc_cmp_threads_asc(ProcessData** a, ProcessData** b) {
	return cmp_u64((*a)->threads.count, (*b)->threads.count);
}
internal int __cdecl proc_cmp_threads_desc(ProcessData** a, ProcessData** b) {
	return -cmp_u64((*a)->threads.count, (*b)->threads.count);
}

internal int __cdecl proc_cmp_handles_asc(ProcessData** a, ProcessData** b) {
	return cmp_u64((*a)->handle_count, (*b)->handle_count);
}
internal int __cdecl proc_cmp_handles_desc(ProcessData** a, ProcessData** b) {
	return -cmp_u64((*a)->handle_count, (*b)->handle_count);
}

internal int __cdecl proc_cmp_hard_faults_asc(ProcessData** a, ProcessData** b) {
	return cmp_u64((*a)->hard_fault_count, (*b)->hard_fault_count);
}
internal int __cdecl proc_cmp_hard_faults_desc(ProcessData** a, ProcessData** b) {
	return -cmp_u64((*a)->hard_fault_count, (*b)->hard_fault_count);
}

internal void tab_processes() {
	local_persist Arena arena      = arena_create(1 * GIGABYTE);
	local_persist bool  arena_init =  false;
	if (!arena_init) {
		arena_frame_begin(&arena);
		arena_init = true;
	}

	local_persist View<ProcessData*> processes = { };

	scratch_begin();

	bool needs_sort = false;
	if (polling_check_for_changes()) {
		arena_frame_end(&arena);
		arena_frame_begin(&arena);
		processes = polling_collect_processes(&arena);
		needs_sort = true;
	}

	CString column_names[ProcessTableColumn__COUNT] = {
		"Name",
		"PID",
		"RAM",
		"Commit",
		"Up Time",
		"CPU Time",
		"User Time",
		"Kernel Time",
		"Threads",
		"Handles",
		"Hard Faults",
	};

	int (__cdecl* process_comparers_asc[ProcessTableColumn__COUNT])(const void*, const void*) = {
		(int (__cdecl*)(const void*, const void*))proc_cmp_image_name_asc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_pid_asc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_ram_asc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_commit_asc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_uptime_asc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_cpu_time_asc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_user_time_asc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_kernel_time_asc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_threads_asc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_handles_asc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_hard_faults_asc,
	};

	int (__cdecl* process_comparers_desc[ProcessTableColumn__COUNT])(const void*, const void*) = {
		(int (__cdecl*)(const void*, const void*))proc_cmp_image_name_desc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_pid_desc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_ram_desc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_commit_desc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_uptime_desc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_cpu_time_desc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_user_time_desc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_kernel_time_desc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_threads_desc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_handles_desc,
		(int (__cdecl*)(const void*, const void*))proc_cmp_hard_faults_desc,
	};

	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 4.0f));
	if (ImGui::BeginTable(
		"ProcessTable",
		ProcessTableColumn__COUNT,
		ImGuiTableFlags_Resizable              |
		ImGuiTableFlags_Reorderable            |
		ImGuiTableFlags_Hideable               |
		ImGuiTableFlags_BordersOuter           |
		ImGuiTableFlags_BordersV               |
		ImGuiTableFlags_SizingFixedFit         |
		ImGuiTableFlags_ScrollY                |
		ImGuiTableFlags_HighlightHoveredColumn |
		ImGuiTableFlags_Sortable               |
		0
	)) {
		for (u32 col_idx = 0; col_idx < ProcessTableColumn__COUNT; col_idx++) {
			ImGui::TableSetupColumn(column_names[col_idx], 0);
		}
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableHeadersRow();

		if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs()) {
			if (sort_specs->SpecsDirty || needs_sort) {
				if (sort_specs->Specs[0].SortDirection == ImGuiSortDirection_Ascending) {
					qsort(
						processes.ptr, processes.len, sizeof(processes.ptr[0]),
						process_comparers_asc[sort_specs->Specs[0].ColumnIndex]
					);
				} else {
					qsort(
						processes.ptr, processes.len, sizeof(processes.ptr[0]),
						process_comparers_desc[sort_specs->Specs[0].ColumnIndex]
					);
				}

				sort_specs->SpecsDirty = false;
			}
		}

		ImGuiListClipper clipper;
		clipper.Begin(processes.len);
		while (clipper.Step()) {
			for (int proc_idx = clipper.DisplayStart; proc_idx < clipper.DisplayEnd; proc_idx++) {
				ProcessData* process = processes[proc_idx];

				ImGui::TableNextRow();
				for (u32 col_idx = 0; col_idx < ProcessTableColumn__COUNT; col_idx++) {
					ImGui::TableSetColumnIndex(col_idx);
					switch (col_idx) {
					case ProcessTableColumn_NAME: {
						imgui_string((String){
							.ptr = process->image_name,
							.len = process->image_name_len
						});
					} break;
					case ProcessTableColumn_PID: {
						imgui_printf_right("%llu", process->pid);
					} break;
					case ProcessTableColumn_RAM: {
						imgui_printf_right("%_$$llu", process->ram);
					} break;
					case ProcessTableColumn_COMMIT: {
						imgui_printf_right("%_$$llu", process->commit);
					} break;
					case ProcessTableColumn_UP_TIME: {
						imgui_string_right(scratch_format_timespan(process->uptime));
					} break;
					case ProcessTableColumn_CPU_TIME: {
						imgui_string_right(scratch_format_timespan(process->cpu_time));
					} break;
					case ProcessTableColumn_USER_TIME: {
						imgui_string_right(scratch_format_timespan(process->user_time));
					} break;
					case ProcessTableColumn_KERNEL_TIME: {
						imgui_string_right(scratch_format_timespan(process->kernel_time));
					} break;
					case ProcessTableColumn_THREADS: {
						imgui_printf_right("%llu", process->threads.count);
					} break;
					case ProcessTableColumn_HANDLES: {
						imgui_printf_right("%llu", process->handle_count);
					} break;
					case ProcessTableColumn_HARD_FAULTS: {
						imgui_printf_right("%llu", process->hard_fault_count);
					} break;
					}
				}
			}
		}

		ImGui::EndTable();
	}
	ImGui::PopStyleVar();

	scratch_end();
}

internal void do_ui() {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushFont(NULL, 16);

	ImGui::Begin(
		"MainWindow",
		NULL,
		ImGuiWindowFlags_NoTitleBar            |
		// ImGuiWindowFlags_MenuBar               |
		ImGuiWindowFlags_NoResize              |
		ImGuiWindowFlags_NoMove                |
		ImGuiWindowFlags_NoCollapse            |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus
	);

	if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_Reorderable)) {
		if (ImGui::BeginTabItem("Processes")) {
			tab_processes();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Performance")) {
			tab_performance();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
	ImGui::PopFont();
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

	polling_begin();

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

	polling_end();

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

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#endif