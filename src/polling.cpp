#include "polling.hpp"

#include "arena.hpp"
#include "windows.hpp"

global bool polling_running = false;

// Circular buffer that grows to a maximum
global CpuPercents cpu_history[300 + 1];
global u64         cpu_history_start = 0;
global u64         cpu_history_count = 0;

// Circular buffer that grows to a maximum
global MemoryPercents memory_history[300 + 1];
global u64            memory_history_start = 0;
global u64            memory_history_count = 0;

internal u32 WINCALLBACK polling_thread(void* param);

api_method void polling_begin() {
	polling_running = true;

	Handle thread = CreateThread(NULL, 0, polling_thread, NULL, 0, NULL);

	SetThreadDescription(thread, L"PollingThread");
}

api_method void polling_end() {
	polling_running = false;
}

api_method CpuHistoryCircularBuffer polling_get_cpu_history() {
	f64 last_time = 0.0;
	if (cpu_history_count) {
		if (cpu_history_count < arrlen(cpu_history)) {
			last_time = cpu_history[cpu_history_count - 1].time;
		} else {
			if (cpu_history_start > 0) {
				last_time = cpu_history[cpu_history_start - 1].time;
			} else {
				last_time = cpu_history[arrlen(cpu_history) - 1].time;
			}
		}
	}
	return (CpuHistoryCircularBuffer) {
		.base   = cpu_history,
		.length = cpu_history_count,
		.offset = cpu_history_start,
		.start  = last_time - (f64)(arrlen(cpu_history) - 1),
		.end    = last_time,
	};
}

api_method MemoryHistoryCircularBuffer polling_get_memory_history() {
	f64 last_time = 0.0;
	if (memory_history_count) {
		if (memory_history_count < arrlen(memory_history)) {
			last_time = memory_history[memory_history_count - 1].time;
		} else {
			if (memory_history_start > 0) {
				last_time = memory_history[memory_history_start - 1].time;
			} else {
				last_time = memory_history[arrlen(memory_history) - 1].time;
			}
		}
	}
	return (MemoryHistoryCircularBuffer) {
		.base   = memory_history,
		.length = memory_history_count,
		.offset = memory_history_start,
		.start  = last_time - (f64)(arrlen(memory_history) - 1),
		.end    = last_time,
	};
}

internal u32 WINCALLBACK polling_thread(void* param) {
	unused_var(param);

	scratch_init();

	SystemBasicInformation sys_basic_info = { };
	u32 basic_info_return_size;
	NtQuerySystemInformation(
		SysInfoClass_SYSTEM_BASIC_INFORMATION,
		&sys_basic_info,
		sizeof(sys_basic_info),
		&basic_info_return_size
	);
	assert(basic_info_return_size >= sizeof(sys_basic_info));

	View<SystemProcessorPerformanceInformation> cpu_stats = {
		.len = (u64)sys_basic_info.number_of_processors * 2 // Alloc last and current together
	};
	scratch_alloc_array(&cpu_stats);
	cpu_stats.len /= 2;
	View<SystemProcessorPerformanceInformation> cpu_stats_last = {
		.ptr = cpu_stats.ptr + cpu_stats.len,
		.len = cpu_stats.len,
	};

	u64 poll_count = 0;
	while (polling_running) {
		// Poll system CPU usage
		u32 sys_cpu_stats_return_size;
		NtQuerySystemInformation(
			SysInfoClass_SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION,
			cpu_stats.ptr,
			sizeof_view(cpu_stats),
			&sys_cpu_stats_return_size
		);

		if (poll_count) {
			// We have stats for before and can get an interval of data.
			u64 user      = 0;
			u64 kernel    = 0;
			u64 idle      = 0;
			u64 interrupt = 0;
			u64 dpc       = 0;
			for (u64 i = 0; i < cpu_stats.len; i++) {
				user      += cpu_stats[i].user_time      - cpu_stats_last[i].user_time;
				kernel    += cpu_stats[i].kernel_time    - cpu_stats_last[i].kernel_time;
				idle      += cpu_stats[i].idle_time      - cpu_stats_last[i].idle_time;
				interrupt += cpu_stats[i].interrupt_time - cpu_stats_last[i].interrupt_time;
				dpc       += cpu_stats[i].dpc_time       - cpu_stats_last[i].dpc_time;
			}
			u64 total = user + kernel; // kernel is all non-user time.
			kernel -= idle;
			u64 cpu = total - idle;

			CpuPercents percents = {
				.time      = (f64)(poll_count - 1),
				.cpu       = ((f64)cpu       / total) * 100.0f,
				.user      = ((f64)user      / total) * 100.0f,
				.kernel    = ((f64)kernel    / total) * 100.0f,
				.interrupt = ((f64)interrupt / total) * 100.0f,
				.dpc       = ((f64)dpc       / total) * 100.0f,
				.idle      = ((f64)idle      / total) * 100.0f,
			};

			u64 index = (cpu_history_start + cpu_history_count) % arrlen(cpu_history);
			cpu_history[index] = percents;
			if (cpu_history_count < arrlen(cpu_history)) {
				cpu_history_count++;
			} else {
				cpu_history_start = (cpu_history_start + 1) % arrlen(cpu_history);
			}
		}

		swap_vars(cpu_stats, cpu_stats_last);

		{
			// Poll memory stats
			SystemPageFileInformationEx page_file_info;
			u32 page_file_info_return_size;
			NtQuerySystemInformation(
				SysInfoClass_SYSTEM_PAGE_FILE_INFORMATION_EX,
				&page_file_info,
				sizeof(page_file_info),
				&page_file_info_return_size
			);
			assert(page_file_info_return_size >= sizeof(page_file_info));

			SystemPerformanceInformation sys_perf_info;
			u32 sys_perf_info_return_size;
			NtQuerySystemInformation(
				SysInfoClass_SYSTEM_PERFORMANCE_INFORMATION,
				&sys_perf_info,
				sizeof(sys_perf_info),
				&sys_perf_info_return_size
			);
			assert(sys_perf_info_return_size >= sizeof(sys_perf_info));

			u32 ram_used_pages = sys_basic_info.number_of_physical_pages - sys_perf_info.available_pages;

			MemoryPercents percents = {
				.time   = (f64)(poll_count),
				.ram    = ((f64)ram_used_pages                   / sys_basic_info.number_of_physical_pages) * 100.0,
				.swap   = ((f64)page_file_info.info.total_in_use / page_file_info.info.total_size)          * 100.0,
				.commit = ((f64)sys_perf_info.committed_pages    / sys_perf_info.commit_limit)              * 100.0,
			};

			u64 index = (memory_history_start + memory_history_count) % arrlen(memory_history);
			memory_history[index] = percents;
			if (memory_history_count < arrlen(memory_history)) {
				memory_history_count++;
			} else {
				memory_history_start = (memory_history_start + 1) % arrlen(memory_history);
			}
		}

		{
			// Processes... Is this the place to do it?

			SystemProcessInformation* proc_info      = NULL;
			u32                       proc_info_size = 0;

			scratch_begin();

			for (;;) {
				NtStatus proc_info_status = NtQuerySystemInformation(
					SysInfoClass_SYSTEM_PROCESS_INFORMATION,
					proc_info,
					proc_info_size,
					&proc_info_size
				);

				if (proc_info_status == NT_STATUS_INFO_LENGTH_MISMATCH) {
					scratch_end();
					scratch_begin();

					proc_info = (SystemProcessInformation*)arena_alloc_item(
						&scratch_arena,
						alignof(SystemProcessInformation),
						proc_info_size
					);

					if (!proc_info) {
						break;
					}
				} else {
					break;
				}
			}

			u32 thread_count = 0;
			u32 handle_count = 0;
			for (;;) {
				thread_count += proc_info->number_of_threads;
				handle_count += proc_info->handle_count;

				if (!proc_info->next_entry_offset) {
					break;
				}

				proc_info = (SystemProcessInformation*)((u8*)proc_info + proc_info->next_entry_offset);
			}

			debug_log("Threads: %u, Handles: %u", thread_count, handle_count);

			scratch_end();
		}

		poll_count++;

		// TODO(hhammon) Do actual timing
		Sleep(1000);
	}

	scratch_destroy();
	return 0;
}
