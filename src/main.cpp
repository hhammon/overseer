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

// NOTE(hhammon) Some sort of generational handle and linked list node
struct ProcessTab {
	ProcessTab*  prev;
	ProcessTab*  next;
	ProcessData* process;
	u64          pid;
	u64          create_time;
	bool         open;
};
ProcessData* opening_tab_process = NULL;

internal void create_render_device();
internal void create_render_target();
internal void destroy_render_device();
internal void destroy_render_target();

StringZ convert_wide_str(Arena* arena, wchar_t* str) {
	if (!str) return (StringZ){ };

	s32 buffer_size = WideCharToMultiByte(
		CP_UTF8,
		0,
		str,
		-1,
		NULL,
		0,
		NULL,
		NULL
	);

	if (buffer_size <= 0) return { };

	StringZ output = {
		.len = (u64)buffer_size,
	};
	alloc_array(arena, &output);

	s32 result = WideCharToMultiByte(
		CP_UTF8,
		0,
		str,
		-1,
		(char*)output.ptr,
		output.len,
		NULL,
		NULL
	);


	if (result <= 0) return { };

	if (output.len) output.len--; // Cut off the null terminator.
	return output;
}

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

internal void stat_table_row(String name, String value) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	imgui_string(name);
	ImGui::TableSetColumnIndex(1);
	imgui_string(value);
}

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

	SystemInfo* system_info = polling_get_system_info();
	if (ImGui::BeginTable("SystemInfo", 2, ImGuiTableFlags_SizingFixedFit)) {
		scratch_begin();

		stat_table_row(
			S("Logical Processors"),
			scratch_sprintf("%llu", system_info->cpu_count)
		);
		stat_table_row(
			S("Up Time"),
			scratch_format_timespan(system_info->uptime)
		);
		stat_table_row(
			S("CPU Time"),
			scratch_sprintf(
				"%s (%.2lf%%)",
				scratch_format_timespan(system_info->cpu_time).ptr,
				system_info->cpu_pct
			)
		);
		stat_table_row(
			S("User Time"),
			scratch_sprintf(
				"%s (%.2lf%%)",
				scratch_format_timespan(system_info->user_time).ptr,
				system_info->user_pct
			)
		);
		stat_table_row(
			S("Kernel Time"),
			scratch_sprintf(
				"%s (%.2lf%%)",
				scratch_format_timespan(system_info->kernel_time).ptr,
				system_info->kernel_pct
			)
		);
		stat_table_row(
			S("Interrupt Time"),
			scratch_sprintf(
				"%s (%.2lf%%)",
				scratch_format_timespan(system_info->interrupt_time).ptr,
				system_info->interrupt_pct
			)
		);
		stat_table_row(
			S("Deferred Procedure Call Time"),
			scratch_sprintf(
				"%s (%.2lf%%)",
				scratch_format_timespan(system_info->dpc_time).ptr,
				system_info->dpc_pct
			)
		);
		stat_table_row(
			S("Idle Time"),
			scratch_sprintf(
				"%s (%.2lf%%)",
				scratch_format_timespan(system_info->idle_time).ptr,
				system_info->idle_pct
			)
		);
		stat_table_row(
			S("Current CPU Utilization"),
			scratch_sprintf(
				"%.2lf%% (User: %.2lf%%, Kernel: %.2lf%%, Interrupt: %.2lf%%, DPC: %.2lf%%)",
				system_info->cpu_pct_tick,
				system_info->user_pct_tick,
				system_info->kernel_pct_tick,
				system_info->interrupt_pct_tick,
				system_info->dpc_pct_tick
			)
		);
		stat_table_row(
			S("RAM"),
			scratch_sprintf(
				"%_$$llu / %_$$llu (%.2lf%%)",
				system_info->ram_used,
				system_info->ram_size,
				system_info->ram_pct
			)
		);
		stat_table_row(
			S("Page File"),
			scratch_sprintf(
				"%_$$llu / %_$$llu (%.2lf%%)",
				system_info->page_file_used,
				system_info->page_file_size,
				system_info->page_file_pct
			)
		);
		stat_table_row(
			S("Commit Charge"),
			scratch_sprintf(
				"%_$$llu / %_$$llu (%.2lf%%)",
				system_info->commit_used,
				system_info->commit_limit,
				system_info->commit_pct
			)
		);
		// stat_table_row(
		// 	S("Page File Bounds (Min - Max)"),
		// 	scratch_sprintf(
		// 		"%_$$llu - %_$$llu",
		// 		system_info->page_file_min,
		// 		system_info->page_file_max
		// 	)
		// );
		stat_table_row(
			S("Processes"),
			scratch_sprintf("%llu", system_info->processes)
		);
		stat_table_row(
			S("Threads"),
			scratch_sprintf("%llu", system_info->threads)
		);
		stat_table_row(
			S("Handles"),
			scratch_sprintf("%llu", system_info->handles)
		);
		stat_table_row(
			S("System Calls"),
			scratch_sprintf(
				"%llu (+%llu)",
				system_info->system_calls,
				system_info->system_calls_tick
			)
		);
		stat_table_row(
			S("Context Switches"),
			scratch_sprintf(
				"%llu (+%llu)",
				system_info->context_switches,
				system_info->context_switches_tick
			)
		);

		scratch_end();
		ImGui::EndTable();
	}
}

enum ProcessTableColumn {
	ProcessTableColumn_NAME,
	ProcessTableColumn_PID,
	ProcessTableColumn_CPU,
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

enum ThreadTableColumn {
	ThreadTableColumn_TID,
	ThreadTableColumn_DESCRIPTION,
	ThreadTableColumn_CPU,
	ThreadTableColumn_UP_TIME,
	ThreadTableColumn_CPU_TIME,
	ThreadTableColumn_USER_TIME,
	ThreadTableColumn_KERNEL_TIME,
	ThreadTableColumn_CONTEXT_SWITCHES,

	ThreadTableColumn__COUNT,
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

internal int __cdecl proc_cmp_cpu_asc(ProcessData** a, ProcessData** b) {
	return cmp_f64((*a)->cpu_pct_tick, (*b)->cpu_pct_tick);
}
internal int __cdecl proc_cmp_cpu_desc(ProcessData** a, ProcessData** b) {
	return -cmp_f64((*a)->cpu_pct_tick, (*b)->cpu_pct_tick);
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

internal void tab_processes(bool polling_changes) {
	local_persist Arena arena      = arena_create(1 * GIGABYTE);
	local_persist bool  arena_init =  false;
	if (!arena_init) {
		arena_frame_begin(&arena);
		arena_init = true;
	}

	local_persist View<ProcessData*> processes = { };

	scratch_begin();

	bool needs_sort = false;
	if (polling_changes) {
		arena_frame_end(&arena);
		arena_frame_begin(&arena);
		processes = polling_collect_processes(&arena);
		needs_sort = true;
	}

	CString column_names[ProcessTableColumn__COUNT] = {
		"Name",
		"PID",
		"CPU",
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
		(int (__cdecl*)(const void*, const void*))proc_cmp_cpu_asc,
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
		(int (__cdecl*)(const void*, const void*))proc_cmp_cpu_desc,
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
		ImGuiTableFlags_ScrollX                |
		ImGuiTableFlags_ScrollY                |
		ImGuiTableFlags_HighlightHoveredColumn |
		ImGuiTableFlags_Sortable               |
		0
	)) {
		for (u32 col_idx = 0; col_idx < ProcessTableColumn__COUNT; col_idx++) {
			ImGui::TableSetupColumn(
				column_names[col_idx],
				(
					col_idx == ProcessTableColumn_NAME ||
					col_idx == ProcessTableColumn_PID
				 ) ?
				 ImGuiTableColumnFlags_PreferSortAscending :
				 ImGuiTableColumnFlags_PreferSortDescending
			);
		}
		ImGui::TableSetupScrollFreeze(1, 1);
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
						// TODO(hhammon) This really is a terribly way to do this for a lot of reasons.
						local_persist s64 selected_pid = -1;
						if (ImGui::Selectable(
							scratch_sprintf("%s##%llu", process->image_name, process->pid).ptr,
							(s64)process->pid == selected_pid,
							ImGuiSelectableFlags_SpanAllColumns   |
							ImGuiSelectableFlags_AllowOverlap     |
							ImGuiSelectableFlags_AllowDoubleClick |
							0
						)) {
							selected_pid = process->pid;

							if (
								ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) ||
								ImGui::IsKeyDown(ImGuiKey_Enter)                   ||
								ImGui::IsKeyDown(ImGuiKey_KeypadEnter)             ||
								0
							) {
								opening_tab_process = process;
							}
						}
					} break;
					case ProcessTableColumn_PID: {
						imgui_printf_right("%llu", process->pid);
					} break;
					case ProcessTableColumn_CPU: {
						imgui_printf_right("%05.2lf%%", process->cpu_pct_tick);
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

internal int __cdecl thread_cmp_tid_asc(ThreadData** a, ThreadData** b) {
	return cmp_u64((*a)->tid, (*b)->tid);
}
internal int __cdecl thread_cmp_tid_desc(ThreadData** a, ThreadData** b) {
	return -cmp_u64((*a)->tid, (*b)->tid);
}

internal int thread_cmp_description_asc(ThreadData** a, ThreadData** b) {
	// Sort threads that have descriptions above those that don't.
	if ((*a)->description.len == 0 || (*b)->description.len == 0) {
		return cmp_u64((*b)->description.len, (*a)->description.len);
	}
	return _stricmp((*a)->description.ptr, (*b)->description.ptr);
}
internal int thread_cmp_description_desc(ThreadData** a, ThreadData** b) {
	// Sort threads that have descriptions above those that don't.
	if ((*a)->description.len == 0 || (*b)->description.len == 0) {
		return cmp_u64((*b)->description.len, (*a)->description.len);
	}
	return -_stricmp((*a)->description.ptr, (*b)->description.ptr);
}

internal int __cdecl thread_cmp_cpu_asc(ThreadData** a, ThreadData** b) {
	return cmp_f64((*a)->cpu_pct, (*b)->cpu_pct);
}
internal int __cdecl thread_cmp_cpu_desc(ThreadData** a, ThreadData** b) {
	return -cmp_f64((*a)->cpu_pct, (*b)->cpu_pct);
}

internal int __cdecl thread_cmp_uptime_asc(ThreadData** a, ThreadData** b) {
	return cmp_f64((*a)->uptime, (*b)->uptime);
}
internal int __cdecl thread_cmp_uptime_desc(ThreadData** a, ThreadData** b) {
	return -cmp_f64((*a)->uptime, (*b)->uptime);
}

internal int __cdecl thread_cmp_cpu_time_asc(ThreadData** a, ThreadData** b) {
	return cmp_f64((*a)->cpu_time, (*b)->cpu_time);
}
internal int __cdecl thread_cmp_cpu_time_desc(ThreadData** a, ThreadData** b) {
	return -cmp_f64((*a)->cpu_time, (*b)->cpu_time);
}

internal int __cdecl thread_cmp_user_time_asc(ThreadData** a, ThreadData** b) {
	return cmp_f64((*a)->user_time, (*b)->user_time);
}
internal int __cdecl thread_cmp_user_time_desc(ThreadData** a, ThreadData** b) {
	return -cmp_f64((*a)->user_time, (*b)->user_time);
}

internal int __cdecl thread_cmp_kernel_time_asc(ThreadData** a, ThreadData** b) {
	return cmp_f64((*a)->kernel_time, (*b)->kernel_time);
}
internal int __cdecl thread_cmp_kernel_time_desc(ThreadData** a, ThreadData** b) {
	return -cmp_f64((*a)->kernel_time, (*b)->kernel_time);
}

internal int thread_cmp_context_switches_asc(ThreadData** a, ThreadData** b) {
	return cmp_u64((*a)->context_switches, (*b)->context_switches);
}
internal int thread_cmp_context_switches_desc(ThreadData** a, ThreadData** b) {
	return -cmp_u64((*a)->context_switches, (*b)->context_switches);
}

internal ThreadData* thread_table(ProcessData* process, bool changes) {
	local_persist ThreadData* selected_thread     = NULL;
	local_persist s64         pid                 = -1;
	local_persist u64         process_create_time =  0;

	if ((s64)process->pid != pid || process->create_time != process_create_time) {
		// NOTE(hhammon) This prevents state from persisting if tabs switch... Okay for a jam.
		pid = process->pid;
		process_create_time = process->create_time;
		selected_thread     = NULL;
	}

	local_persist Arena arena      = arena_create(1 * MEGABYTE);
	local_persist bool  arena_init =  false;
	if (!arena_init) {
		arena_frame_begin(&arena);
		arena_init = true;
	}

	local_persist View<ThreadData*> threads = { };

	scratch_begin();

	bool needs_sort = false;
	if (changes) {
		arena_frame_end  (&arena);
		arena_frame_begin(&arena);
		threads = polling_collect_threads(&arena, process);
		needs_sort = true;
	}

	// Set thread descriptions
	for (u64 i = 0; i < threads.len; i++) {
		ThreadData* thread = threads[i];
		Handle thread_handle = OpenThread(
			ThreadAccessFlag_QUERY_LIMITED_INFORMATION,
			false,
			thread->tid
		);
		wchar_t* description_w;
		if (SUCCEEDED(GetThreadDescription(thread_handle, &description_w))) {
			thread->description = convert_wide_str(&arena, description_w);
			LocalFree(description_w);
		} else {
			thread->description = { };
		}
		CloseHandle(thread_handle);
	}


	CString column_names[ThreadTableColumn__COUNT] = {
		"TID",
		"Description",
		"CPU",
		"Up Time",
		"CPU Time",
		"User Time",
		"Kernel Time",
		"Context Switches",
	};

	int (__cdecl* process_comparers_asc[ThreadTableColumn__COUNT])(const void*, const void*) = {
		(int (__cdecl*)(const void*, const void*))thread_cmp_tid_asc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_description_asc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_cpu_asc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_uptime_asc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_cpu_time_asc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_user_time_asc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_kernel_time_asc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_context_switches_asc,
	};

	int (__cdecl* process_comparers_desc[ThreadTableColumn__COUNT])(const void*, const void*) = {
		(int (__cdecl*)(const void*, const void*))thread_cmp_tid_desc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_description_desc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_cpu_desc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_uptime_desc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_cpu_time_desc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_user_time_desc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_kernel_time_desc,
		(int (__cdecl*)(const void*, const void*))thread_cmp_context_switches_desc,
	};

	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 4.0f));
	if (ImGui::BeginTable(
		"ThreadTable",
		ThreadTableColumn__COUNT,
		ImGuiTableFlags_Resizable              |
		ImGuiTableFlags_Reorderable            |
		ImGuiTableFlags_Hideable               |
		ImGuiTableFlags_BordersOuter           |
		ImGuiTableFlags_BordersV               |
		ImGuiTableFlags_SizingFixedFit         |
		// ImGuiTableFlags_ScrollX                |
		ImGuiTableFlags_HighlightHoveredColumn |
		ImGuiTableFlags_Sortable               |
		0
	)) {
		for (u32 col_idx = 0; col_idx < ThreadTableColumn__COUNT; col_idx++) {
			ImGui::TableSetupColumn(
				column_names[col_idx],
				(
					col_idx == ThreadTableColumn_TID
				 ) ?
				 ImGuiTableColumnFlags_PreferSortAscending :
				 ImGuiTableColumnFlags_PreferSortDescending
			);
		}
		ImGui::TableSetupScrollFreeze(1, 1);
		ImGui::TableHeadersRow();

		if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs()) {
			if (sort_specs->SpecsDirty || needs_sort) {
				if (sort_specs->Specs[0].SortDirection == ImGuiSortDirection_Ascending) {
					qsort(
						threads.ptr, threads.len, sizeof(threads.ptr[0]),
						process_comparers_asc[sort_specs->Specs[0].ColumnIndex]
					);
				} else {
					qsort(
						threads.ptr, threads.len, sizeof(threads.ptr[0]),
						process_comparers_desc[sort_specs->Specs[0].ColumnIndex]
					);
				}

				sort_specs->SpecsDirty = false;
			}
		}

		ImGuiListClipper clipper;
		clipper.Begin(threads.len);
		while (clipper.Step()) {
			for (int thread_idx = clipper.DisplayStart; thread_idx < clipper.DisplayEnd; thread_idx++) {
				ThreadData* thread = threads[thread_idx];

				ImGui::TableNextRow();
				for (u32 col_idx = 0; col_idx < ThreadTableColumn__COUNT; col_idx++) {
					ImGui::TableSetColumnIndex(col_idx);
					switch (col_idx) {
					case ThreadTableColumn_TID: {
						// TODO(hhammon) This really is a terribly way to do this for a lot of reasons.
						if (ImGui::Selectable(
							scratch_sprintf("%llu", thread->tid).ptr,
							thread == selected_thread,
							ImGuiSelectableFlags_SpanAllColumns   |
							ImGuiSelectableFlags_AllowOverlap     |
							ImGuiSelectableFlags_AllowDoubleClick |
							0
						)) {
							selected_thread = thread;
						}
					} break;
					case ThreadTableColumn_DESCRIPTION: {
						imgui_string(thread->description);
					} break;
					case ThreadTableColumn_CPU: {
						imgui_printf_right("%05.2lf%%", thread->cpu_pct);
					} break;
					case ThreadTableColumn_UP_TIME: {
						imgui_string_right(scratch_format_timespan(thread->uptime));
					} break;
					case ThreadTableColumn_CPU_TIME: {
						imgui_string_right(scratch_format_timespan(thread->cpu_time));
					} break;
					case ThreadTableColumn_USER_TIME: {
						imgui_string_right(scratch_format_timespan(thread->user_time));
					} break;
					case ThreadTableColumn_KERNEL_TIME: {
						imgui_string_right(scratch_format_timespan(thread->kernel_time));
					} break;
					case ThreadTableColumn_CONTEXT_SWITCHES: {
						imgui_printf_right("%llu", thread->context_switches);
					} break;
					}
				}
			}
		}

		ImGui::EndTable();
	}
	ImGui::PopStyleVar();

	scratch_end();

	return selected_thread;
}

internal void process_vm(ProcessData* process, bool changes, bool is_tab = false) {
	local_persist s64         pid                 = -1;
	local_persist u64         process_create_time =  0;

	if ((s64)process->pid != pid || process->create_time != process_create_time) {
		// NOTE(hhammon) This prevents state from persisting if tabs switch... Okay for a jam.
		pid = process->pid;
		process_create_time = process->create_time;
	}

	local_persist Arena arena      = arena_create(1 * MEGABYTE);
	local_persist bool  arena_init =  false;
	if (!arena_init) {
		arena_frame_begin(&arena);
		arena_init = true;
	}

	enum VMType {
		VMType_NONE,
		VMType_PRIVATE,
		VMType_IMAGE,
		VMType_SECTION,
	};

	enum VMState {
		VMState_FREE,
		VMState_RESERVED,
		VMState_COMMITTED,
	};

	enum_flags(VMFlags, u32) {
		VMFlags_READ  = 1 << 0,
		VMFlags_WRITE = 1 << 1,
		VMFlags_EXEC  = 1 << 2,
		VMFlags_COW   = 1 << 3,
		VMFlags_GUARD = 1 << 4,
	};

	struct VMAllocation;

	struct VMRegion {
		VMAllocation* allocation;
		uptr          allocation_base;
		uptr          base;
		uptr          top;
		u64           size;
		VMType        type;
		VMState       state;
		VMFlags       flags;
	};

	struct VMAllocation {
		uptr           base;
		uptr           top;
		u64            size;
		View<VMRegion> regions;
		VMType         type;
	};

	local_persist View<VMRegion>     regions     = { };
	local_persist View<VMAllocation> allocations = { };

	if (changes) {
		arena_frame_end  (&arena);
		arena_frame_begin(&arena);

		if (process->pid == GetCurrentProcessId()) {
			// TODO(hhammon) This is possible to do, but we would need to suspend all threads but this one
			// instead of just calling NtSuspendProcess.
			ImGui::Text("Cannot yet get memory mappings for the current process. (Can be implemented)");
			return;
		}

		Handle process_handle = OpenProcess(
			ProcessAccessFlag_SUSPEND_RESUME    |
			ProcessAccessFlag_QUERY_INFORMATION |
			ProcessAccessFlag_VM_READ           |
			0,
			false,
			process->pid
		);

		if (!process_handle) {
			// TODO(hhammon) :BetterErrorMessage
			ImGui::Text("Could not open the process.");
			return;
		}

		regions     = { };
		allocations = { };

		if (FAILED(NtSuspendProcess(process_handle))) {
			CloseHandle(process_handle);
			ImGui::Text("Could not suspend the process to query its virtual memory.");
			return;
		}

		MemoryBasicInformation region_info;
		uptr address = NULL;
		uptr last_allocation_base = (uptr)-1;
		while (VirtualQueryEx(
			process_handle,
			(void*)address,
			&region_info,
			sizeof(region_info
		)) == sizeof(region_info)) {
			VMRegion* region = alloc_item(&arena, VMRegion);

			if (!region) break;

			*region = { };

			if (!regions.ptr) {
				regions.ptr = region;
			}
			regions.len++;

			region->allocation_base = (uptr)region_info.allocation_base;
			region->base            = (uptr)region_info.base_address;
			region->top             = region->base + region_info.region_size;
			region->size            = region_info.region_size;

			if (region->allocation_base != last_allocation_base) {
				allocations.len++;
				last_allocation_base = region->allocation_base;
			}

			switch (region_info.type) {
			case 0: {
				region->type = VMType_NONE;
			} break;
			case MEM_PRIVATE: {
				region->type = VMType_PRIVATE;
			} break;
			case MEM_MAPPED: {
				region->type = VMType_SECTION;
			} break;
			case MEM_IMAGE: {
				region->type = VMType_IMAGE;
			} break;
			}

			switch (region_info.state) {
			case MEM_FREE: {
				region->state = VMState_FREE;
			} break;
			case MEM_RESERVE: {
				region->state = VMState_RESERVED;
			} break;
			case MEM_COMMIT: {
				region->state = VMState_COMMITTED;
			} break;
			}

			switch (region_info.protect & 0xff) {
			case 0:
			case PAGE_NOACCESS: {
				region->flags = 0;
			} break;
			case PAGE_READONLY: {
				region->flags = VMFlags_READ;
			} break;
			case PAGE_READWRITE: {
				region->flags = VMFlags_READ | VMFlags_WRITE;
			} break;
			case PAGE_WRITECOPY: {
				region->flags = VMFlags_READ | VMFlags_WRITE | VMFlags_COW;
			} break;
			case PAGE_EXECUTE_READ:
			case PAGE_EXECUTE: {
				region->flags = VMFlags_READ | VMFlags_EXEC;
			} break;
			case PAGE_EXECUTE_READWRITE: {
				region->flags = VMFlags_READ | VMFlags_WRITE | VMFlags_EXEC;
			} break;
			case PAGE_EXECUTE_WRITECOPY: {
				region->flags = VMFlags_READ | VMFlags_WRITE | VMFlags_EXEC | VMFlags_COW;
			} break;
			}

			if (region_info.protect & PAGE_GUARD) {
				region->flags |= VMFlags_GUARD;
			}

			address = region->top;
		}

		NtResumeProcess(process_handle);
		CloseHandle    (process_handle);

		alloc_array(&arena, &allocations);

		last_allocation_base = (uptr)-1;
		s32 allocation_idx   = -1;
		for (u32 region_idx = 0; region_idx < regions.len; region_idx++) {
			VMRegion*     region     = &regions[region_idx];
			VMAllocation* allocation = NULL;

			if (region->allocation_base != last_allocation_base) {
				if ((u64)(++allocation_idx) >= allocations.len) break;
				allocation = &allocations[allocation_idx];

				// Free regions will have an allocation base of 0 from Windows, but we don't want that.
				allocation->base    = region->allocation_base ? region->allocation_base : region->base,
				allocation->regions = {
					.ptr = region,
				};
			} else {
				allocation = &allocations[allocation_idx];
			}

			region->allocation = allocation;

			allocation->top  = region->top;
			allocation->size = allocation->top - allocation->base;
			allocation->regions.len++;

			if (region->type == VMType_PRIVATE && allocation->type == VMType_NONE) {
				allocation->type = VMType_PRIVATE;
			}

			if (region->type == VMType_SECTION || region->type == VMType_IMAGE) {
				allocation->type = region->type;
			}
		}
	}

	if (!is_tab) {
		u32 count_allocations            = 0;
		u32 count_allocations_private    = 0;
		u32 count_allocations_section    = 0;
		u32 count_allocations_image      = 0;

		u64 size_vm                      = 0;
		u64 size_allocations             = 0;
		u64 size_private                 = 0;
		u64 size_private_reserved        = 0;
		u64 size_private_committed       = 0;
		u64 size_section                 = 0;
		u64 size_section_reserved        = 0;
		u64 size_section_committed       = 0;
		u64 size_image                   = 0;
		u64 size_image_reserved          = 0;
		u64 size_image_committed         = 0;
		u64 size_free                    = 0;
		u64 size_reserved                = 0;
		u64 size_committed               = 0;
		u64 size_read_only               = 0;
		u64 size_read_write              = 0;
		u64 size_read_exec               = 0;
		u64 size_read_write_exec         = 0;
		u64 size_private_read_only       = 0;
		u64 size_private_read_write      = 0;
		u64 size_private_read_exec       = 0;
		u64 size_private_read_write_exec = 0;
		u64 size_section_read_only       = 0;
		u64 size_section_read_write      = 0;
		u64 size_section_read_exec       = 0;
		u64 size_section_read_write_exec = 0;
		u64 size_image_read_only         = 0;
		u64 size_image_read_write        = 0;
		u64 size_image_read_exec         = 0;
		u64 size_image_read_write_exec   = 0;
		u64 size_mapped_private          = 0;
		u64 size_section_private         = 0;
		u64 size_image_private           = 0;

		if (allocations.len) {
			size_vm = allocations[allocations.len - 1].top;
		}

		for (u32 allocation_idx = 0; allocation_idx < allocations.len; allocation_idx++) {
			VMAllocation allocation = allocations[allocation_idx];

			if (allocation.type != VMType_NONE) {
				count_allocations++;
				size_allocations += allocation.size;
			}

			switch (allocation.type) {
			case VMType_PRIVATE: {
				count_allocations_private++;
			};
			case VMType_IMAGE: {
				count_allocations_image++;
			};
			case VMType_SECTION: {
				count_allocations_section++;
			};
			}
		}

		for (u32 region_idx = 0; region_idx < regions.len; region_idx++) {
			VMRegion     region     = regions[region_idx];
			VMAllocation allocation = region.allocation ? *region.allocation : (VMAllocation){ };

			VMFlags protect_flags = (
				VMFlags_READ  |
				VMFlags_WRITE |
				VMFlags_EXEC  |
				0
			);

			bool is_private         = (region.type == VMType_PRIVATE);
			bool is_section         = (region.type == VMType_SECTION);
			bool is_image           = (region.type == VMType_IMAGE);

			bool is_section_alloc   = (allocation.type == VMType_SECTION);
			bool is_image_alloc     = (allocation.type == VMType_SECTION);
			bool is_mapped_alloc    = (is_section_alloc | is_image_alloc);

			bool is_free            = (region.state == VMState_FREE || region.type == VMType_NONE);
			bool is_reserved        = (region.state == VMState_RESERVED);
			bool is_committed       = (region.state == VMState_COMMITTED);

			bool is_read_only       = (region.flags & protect_flags) == (VMFlags_READ);
			bool is_read_write      = (region.flags & protect_flags) == (VMFlags_READ | VMFlags_WRITE);
			bool is_read_exec       = (region.flags & protect_flags) == (VMFlags_READ | VMFlags_EXEC);
			bool is_read_write_exec = (region.flags & protect_flags) == (protect_flags);

			size_vm += region.size;
			if (is_private         & true              ) size_private                 += region.size;
			if (is_private         & is_reserved       ) size_private_reserved        += region.size;
			if (is_private         & is_committed      ) size_private_committed       += region.size;
			if (is_section         & true              ) size_section                 += region.size;
			if (is_section         & is_reserved       ) size_section_reserved        += region.size;
			if (is_section         & is_committed      ) size_section_committed       += region.size;
			if (is_image           & true              ) size_image                   += region.size;
			if (is_image           & is_reserved       ) size_image_reserved          += region.size;
			if (is_image           & is_committed      ) size_image_committed         += region.size;
			if (is_free            & true              ) size_free                    += region.size;
			if (is_reserved        & true              ) size_reserved                += region.size;
			if (is_committed       & true              ) size_committed               += region.size;
			if (is_read_only       & true              ) size_read_only               += region.size;
			if (is_read_write      & true              ) size_read_write              += region.size;
			if (is_read_exec       & true              ) size_read_exec               += region.size;
			if (is_read_write_exec & true              ) size_read_write_exec         += region.size;
			if (is_private         & is_read_only      ) size_private_read_only       += region.size;
			if (is_private         & is_read_write     ) size_private_read_write      += region.size;
			if (is_private         & is_read_exec      ) size_private_read_exec       += region.size;
			if (is_private         & is_read_write_exec) size_private_read_write_exec += region.size;
			if (is_section         & is_read_only      ) size_section_read_only       += region.size;
			if (is_section         & is_read_write     ) size_section_read_write      += region.size;
			if (is_section         & is_read_exec      ) size_section_read_exec       += region.size;
			if (is_section         & is_read_write_exec) size_section_read_write_exec += region.size;
			if (is_image           & is_read_only      ) size_image_read_only         += region.size;
			if (is_image           & is_read_write     ) size_image_read_write        += region.size;
			if (is_image           & is_read_exec      ) size_image_read_exec         += region.size;
			if (is_image           & is_read_write_exec) size_image_read_write_exec   += region.size;
			if (is_mapped_alloc    & is_private        ) size_mapped_private          += region.size;
			if (is_section_alloc   & is_private        ) size_section_private         += region.size;
			if (is_image_alloc     & is_private        ) size_image_private           += region.size;
		}

		if (ImGui::BeginTable("VMStats", 2, ImGuiTableFlags_SizingFixedFit)) {
			scratch_begin();

			stat_table_row(
				S("count_allocations"),
				scratch_sprintf("%lu", count_allocations)
			);
			stat_table_row(
				S("count_allocations_private"),
				scratch_sprintf("%lu", count_allocations_private)
			);
			stat_table_row(
				S("count_allocations_section"),
				scratch_sprintf("%lu", count_allocations_section)
			);
			stat_table_row(
				S("count_allocations_image"),
				scratch_sprintf("%lu", count_allocations_image)
			);
			stat_table_row(
				S("total_size_vm"),
				scratch_sprintf("%_$$llu", size_vm)
			);
			stat_table_row(
				S("total_size_allocations"),
				scratch_sprintf("%_$$llu", size_allocations)
			);
			stat_table_row(
				S("total_size_private"),
				scratch_sprintf("%_$$llu", size_private)
			);
			stat_table_row(
				S("total_size_private_reserved"),
				scratch_sprintf("%_$$llu", size_private_reserved)
			);
			stat_table_row(
				S("total_size_private_committed"),
				scratch_sprintf("%_$$llu", size_private_committed)
			);
			stat_table_row(
				S("total_size_section"),
				scratch_sprintf("%_$$llu", size_section)
			);
			stat_table_row(
				S("total_size_section_reserved"),
				scratch_sprintf("%_$$llu", size_section_reserved)
			);
			stat_table_row(
				S("total_size_section_committed"),
				scratch_sprintf("%_$$llu", size_section_committed)
			);
			stat_table_row(
				S("total_size_image"),
				scratch_sprintf("%_$$llu", size_image)
			);
			stat_table_row(
				S("total_size_image_reserved"),
				scratch_sprintf("%_$$llu", size_image_reserved)
			);
			stat_table_row(
				S("total_size_image_committed"),
				scratch_sprintf("%_$$llu", size_image_committed)
			);
			stat_table_row(
				S("total_size_free"),
				scratch_sprintf("%_$$llu", size_free)
			);
			stat_table_row(
				S("total_size_reserved"),
				scratch_sprintf("%_$$llu", size_reserved)
			);
			stat_table_row(
				S("total_size_committed"),
				scratch_sprintf("%_$$llu", size_committed)
			);
			stat_table_row(
				S("total_size_read_only"),
				scratch_sprintf("%_$$llu", size_read_only)
			);
			stat_table_row(
				S("total_size_read_write"),
				scratch_sprintf("%_$$llu", size_read_write)
			);
			stat_table_row(
				S("total_size_read_exec"),
				scratch_sprintf("%_$$llu", size_read_exec)
			);
			stat_table_row(
				S("total_size_read_write_exec"),
				scratch_sprintf("%_$$llu", size_read_write_exec)
			);
			stat_table_row(
				S("total_size_private_read_only"),
				scratch_sprintf("%_$$llu", size_private_read_only)
			);
			stat_table_row(
				S("total_size_private_read_write"),
				scratch_sprintf("%_$$llu", size_private_read_write)
			);
			stat_table_row(
				S("total_size_private_read_exec"),
				scratch_sprintf("%_$$llu", size_private_read_exec)
			);
			stat_table_row(
				S("total_size_private_read_write_exec"),
				scratch_sprintf("%_$$llu", size_private_read_write_exec)
			);
			stat_table_row(
				S("total_size_section_read_only"),
				scratch_sprintf("%_$$llu", size_section_read_only)
			);
			stat_table_row(
				S("total_size_section_read_write"),
				scratch_sprintf("%_$$llu", size_section_read_write)
			);
			stat_table_row(
				S("total_size_section_read_exec"),
				scratch_sprintf("%_$$llu", size_section_read_exec)
			);
			stat_table_row(
				S("total_size_section_read_write_exec"),
				scratch_sprintf("%_$$llu", size_section_read_write_exec)
			);
			stat_table_row(
				S("total_size_image_read_only"),
				scratch_sprintf("%_$$llu", size_image_read_only)
			);
			stat_table_row(
				S("total_size_image_read_write"),
				scratch_sprintf("%_$$llu", size_image_read_write)
			);
			stat_table_row(
				S("total_size_image_read_exec"),
				scratch_sprintf("%_$$llu", size_image_read_exec)
			);
			stat_table_row(
				S("total_size_image_read_write_exec"),
				scratch_sprintf("%_$$llu", size_image_read_write_exec)
			);
			stat_table_row(
				S("total_size_mapped_private"),
				scratch_sprintf("%_$$llu", size_mapped_private)
			);
			stat_table_row(
				S("total_size_section_private"),
				scratch_sprintf("%_$$llu", size_section_private)
			);
			stat_table_row(
				S("total_size_image_private"),
				scratch_sprintf("%_$$llu", size_image_private)
			);

			scratch_end();
			ImGui::EndTable();
		}
	} else {

	}
}

internal void tab_process(ProcessData* process, bool polling_changes) {
	local_persist ProcessData* last_process;
	bool changes = polling_changes || (process != last_process);
	last_process = process;

	local_persist ThreadData* selected_thread = NULL;

	if (ImPlot::BeginPlot("CPU Usage", ImVec2(-1, ImGui::GetTextLineHeight() * 15))) {
		ProcessHistoryPoint* base   = process->history.buffer;
		u64                  length = process->history.length;

		ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoTickLabels, 0);
		ImPlot::SetupAxisLimits(ImAxis_X1, process->history.start, process->history.end, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);

		ImPlotSpec spec = {};
		spec.Offset = process->history.offset;
		spec.Stride = sizeof(base[0]);

		ImVec4 cpu_color           = ImVec4(0.2f, 0.8f, 1.0f, 1.0f);
		ImVec4 kernel_color        = ImVec4(0.8f, 0.6f, 0.2f, 1.0f);
		ImVec4 thread_cpu_color    = ImVec4(0.1f, 0.4f, 0.5f, 1.0f);
		ImVec4 thread_kernel_color = ImVec4(0.4f, 0.3f, 0.1f, 1.0f);

		spec.LineColor = cpu_color;
		ImPlot::PlotLine("All", &base->time, &base->cpu, length, spec);
		spec.LineColor = kernel_color;
		ImPlot::PlotLine("Kernel", &base->time, &base->kernel, length, spec);

		if (selected_thread) {
			scratch_begin();
			ThreadHistoryPoint* thread_base   = selected_thread->history.buffer;
			u64                 thread_length = selected_thread->history.length;

			CString thread_cpu_label    = scratch_sprintf("Thread %llu CPU",    selected_thread->tid).ptr;
			CString thread_kernel_label = scratch_sprintf("Thread %llu Kernel", selected_thread->tid).ptr;

			spec.Offset = selected_thread->history.offset;
			spec.Stride = sizeof(thread_base[0]);

			spec.LineColor = thread_cpu_color;
			ImPlot::PlotLine(thread_cpu_label, &thread_base->time, &thread_base->cpu, thread_length, spec);
			spec.LineColor = thread_kernel_color;
			ImPlot::PlotLine(thread_kernel_label, &thread_base->time, &thread_base->kernel, thread_length, spec);

			scratch_end();
		}

		ImPlot::EndPlot();
	}

	if (ImPlot::BeginPlot("Memory Usage", ImVec2(-1, ImGui::GetTextLineHeight() * 15))) {
		ProcessHistoryPoint* base   = process->history.buffer;
		u64                  length = process->history.length;

		ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoTickLabels, 0);
		ImPlot::SetupAxisLimits(ImAxis_X1, process->history.start, process->history.end, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);

		ImPlotSpec spec = {};
		spec.Offset = process->history.offset;
		spec.Stride = sizeof(base[0]);

		ImVec4 ram_color    = ImVec4(0.8f, 0.3f, 0.7f, 1.0f);
		ImVec4 commit_color = ImVec4(0.1f, 0.9f, 0.1f, 1.0f);

		spec.LineColor = ram_color;
		ImPlot::PlotLine("RAM", &base->time, &base->ram, length, spec);
		spec.LineColor = commit_color;
		ImPlot::PlotLine("Commit", &base->time, &base->commit, length, spec);

		ImPlot::EndPlot();
	}

	if (ImGui::Button("Kill")) {
		debug_log("Killing PID %llu", process->pid);

		Handle process_handle = OpenProcess(ProcessAccessFlag_TERMINATE, false, process->pid);
		if (process_handle) {
			b32 terminate_result = TerminateProcess(process_handle, 1);

			if (!terminate_result) {
				debug_log("Failed to kill process");
			}
		} else {
			debug_log("Failed to open handle");
		}
	}

	SystemInfo* system_info = polling_get_system_info();
	if (ImGui::BeginTable("SystemInfo", 2, ImGuiTableFlags_SizingFixedFit)) {
		scratch_begin();

		stat_table_row(
			S("Process"),
			scratch_sprintf("%s (PID: %llu)", process->image_name, process->pid)
		);
		stat_table_row(
			S("Up Time"),
			scratch_format_timespan(process->uptime)
		);
		stat_table_row(
			S("CPU Time"),
			scratch_sprintf(
				"%s (%.2lf%%)",
				scratch_format_timespan(process->cpu_time).ptr,
				process->cpu_pct
			)
		);
		stat_table_row(
			S("User Time"),
			scratch_sprintf(
				"%s (%.2lf%%)",
				scratch_format_timespan(process->user_time).ptr,
				process->user_pct
			)
		);
		stat_table_row(
			S("Kernel Time"),
			scratch_sprintf(
				"%s (%.2lf%%)",
				scratch_format_timespan(process->kernel_time).ptr,
				process->kernel_pct
			)
		);
		stat_table_row(
			S("Current CPU Utilization"),
			scratch_sprintf(
				"%.2lf%% (User: %.2lf%%, Kernel: %.2lf%%)",
				process->cpu_pct_tick,
				process->user_pct_tick,
				process->kernel_pct_tick
			)
		);
		stat_table_row(
			S("RAM"),
			scratch_sprintf(
				"%_$$llu / %_$$llu (%.2lf%%)",
				process->ram,
				system_info->ram_size,
				(f64)process->ram / system_info->ram_size * 100
			)
		);
		stat_table_row(
			S("Commit Charge"),
			scratch_sprintf(
				"%_$$llu / %_$$llu (%.2lf%%)",
				process->commit,
				system_info->commit_limit,
				(f64)process->commit / system_info->commit_limit * 100
			)
		);
		stat_table_row(
			S("Threads"),
			scratch_sprintf("%llu", process->threads.count)
		);
		stat_table_row(
			S("Handles"),
			scratch_sprintf("%llu", process->handle_count)
		);

		ImGui::EndTable();
		scratch_end();
	}

	local_persist bool update_thread_table = true;
	if (changes) update_thread_table       = true;
	if (ImGui::CollapsingHeader("Threads")) {
		selected_thread = thread_table(process, update_thread_table);
	} else {
		update_thread_table = true;
	}

	local_persist bool update_process_vm = true;
	if (changes) update_process_vm       = true;
	if (ImGui::CollapsingHeader("Virtual Memory")) {
		process_vm(process, update_process_vm);
	} else {
		update_process_vm = true;
	}
}

internal void process_tabs(bool polling_changes) {
	local_persist Arena arena = arena_create(1 * GIGABYTE);
	local_persist ProcessTab* tabs_head = NULL;
	local_persist ProcessTab* tabs_tail = NULL;
	local_persist ProcessTab* free_list = NULL;

	// Remove any that are now invalid and check if an open process is being opened again
	ProcessTab* tab = tabs_head;
	ProcessTab* opening = NULL;
	while (tab) {
		ProcessTab* next_tab = tab->next;

		if (
			!(tab->process->alive)                           ||
			!(tab->process->pid         == tab->pid)         ||
			!(tab->process->create_time == tab->create_time) ||
			!(tab->open)
		) {
			if (tab->prev) {
				tab->prev->next = tab->next;
			} else {
				tabs_head = tab->next;
			}

			if (tab->next) {
				tab->next->prev = tab->prev;
			} else {
				tabs_tail = tab->prev;
			}

			tab->next = free_list;
			free_list = tab;
		} else if (tab->process == opening_tab_process) {
			opening = tab;
		}

		tab = next_tab;
	}

	if (opening_tab_process && !opening) {
		if (free_list) {
			opening   = free_list;
			free_list = opening->next;
		} else {
			opening = alloc_item(&arena, ProcessTab);
		}

		opening->prev = tabs_tail;
		opening->next = NULL;
		if (tabs_tail) {
			tabs_tail->next = opening;
			tabs_tail = opening;
		} else {
			tabs_head = opening;
			tabs_tail = opening;
		}

		opening->process     = opening_tab_process;
		opening->pid         = opening_tab_process->pid;
		opening->create_time = opening_tab_process->create_time;
		opening->open        = true;
	}

	opening_tab_process = NULL;

	scratch_begin();

	tab = tabs_head;
	while (tab) {
		ProcessTab* next_tab = tab->next;

		ProcessData* process = tab->process;
		if (ImGui::BeginTabItem(
			scratch_sprintf("%s (%llu)", process->image_name, process->pid).ptr,
			&tab->open,
			(tab == opening) ? ImGuiTabItemFlags_SetSelected : 0
		)) {
			if (ImGui::BeginChild("ProcessesBody")) {
				tab_process(process, polling_changes);
			}
			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		tab = next_tab;
	}

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
		ImGuiWindowFlags_NoNavFocus            |
		0
	);

	bool polling_changes = polling_check_for_changes();

	if (ImGui::BeginTabBar(
		"Tabs",
		ImGuiTabBarFlags_Reorderable         |
		ImGuiTabBarFlags_FittingPolicyScroll |
		ImGuiTabBarFlags_TabListPopupButton
	)) {
		if (ImGui::BeginTabItem("Processes")) {
			if (ImGui::BeginChild("ProcessesBody")) {
				tab_processes(polling_changes);
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Performance")) {
			if (ImGui::BeginChild("ProcessesBody")) {
				tab_performance();
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		process_tabs(polling_changes);
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

	SetThreadDescription(GetCurrentThread(), L"MainThread");

	scratch_init();

	polling_begin();

	ImGui_ImplWin32_EnableDpiAwareness();
	f32 main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(
		MonitorFromPoint(Point{}, MONITOR_DEFAULTTOPRIMARY)
	);

	// Create window
	WndClassExW wnd_class = { };
	wnd_class.size        = sizeof(wnd_class);
	wnd_class.style       = CS_CLASSDC;
	wnd_class.wnd_proc    = wnd_proc;
	wnd_class.instance    = instance;
	wnd_class.class_name  = L"Overseer Window";
	wnd_class.icon        = LoadIconW(instance, L"MAINICON");
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