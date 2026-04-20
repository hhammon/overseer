#include "polling.hpp"

#include "arena.hpp"
#include "windows.hpp"

#include <unordered_map>

global bool polling_running = false;
global bool changes_ready   = false;

// Circular buffer that grows to a maximum
global CpuPercents cpu_history[POLLING_HISTORY_LENGTH];
global u64         cpu_history_start = 0;
global u64         cpu_history_count = 0;

// Circular buffer that grows to a maximum
global MemoryPercents memory_history[POLLING_HISTORY_LENGTH];
global u64            memory_history_start = 0;
global u64            memory_history_count = 0;

// Doubly-linked list of all processes
global ProcessList  processes         = { };
global ProcessData* process_free_list = NULL;
global ThreadData*  thread_free_list  = NULL;

global SystemInfo system_info = { };

internal u32 WINCALLBACK polling_thread(void* param);

api_method void polling_begin() {
	polling_running = true;

	Handle thread = CreateThread(NULL, 0, polling_thread, NULL, 0, NULL);

	SetThreadDescription(thread, L"PollingThread");
}

api_method void polling_end() {
	polling_running = false;
}

api_method bool polling_check_for_changes() {
	bool changes  = changes_ready;
	changes_ready = false;
	return changes;
}

api_method CpuHistory polling_get_cpu_history() {
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
	return (CpuHistory) {
		.base   = cpu_history,
		.length = cpu_history_count,
		.offset = cpu_history_start,
		.start  = last_time - (f64)(arrlen(cpu_history) - 1),
		.end    = last_time,
	};
}

api_method MemoryHistory polling_get_memory_history() {
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
	return (MemoryHistory) {
		.base   = memory_history,
		.length = memory_history_count,
		.offset = memory_history_start,
		.start  = last_time - (f64)(arrlen(memory_history) - 1),
		.end    = last_time,
	};
}

api_method View<ProcessData*> polling_collect_processes(Arena* arena) {
	View<ProcessData*> output = {.len = processes.count};
	alloc_array(arena, &output);

	u64 idx = 0;
	ProcessData* process = processes.head;
	while (idx < output.len && process) {
		output[idx++] = process;
		process       = process->next;
	}
	output.len = idx;

	return output;
}

api_method View<ThreadData*>  polling_collect_threads(Arena* arena, ProcessData* process) {
	View<ThreadData*> output = {.len = process->threads.count};
	alloc_array(arena, &output);

	u64 idx = 0;
	ThreadData* thread = process->threads.head;
	while (idx < output.len && thread) {
		output[idx++] = thread;
		thread        = thread->next;
	}
	output.len = idx;

	return output;
}

api_method SystemInfo* polling_get_system_info() {
	return &system_info;
}

internal u32 WINCALLBACK polling_thread(void* param) {
	unused_var(param);

	scratch_init();
	Arena arena = arena_create(1 * GIGABYTE);

	SystemBasicInformation sys_basic_info = { };
	u32 basic_info_return_size;
	NtQuerySystemInformation(
		SysInfoClass_SYSTEM_BASIC_INFORMATION,
		&sys_basic_info,
		sizeof(sys_basic_info),
		&basic_info_return_size
	);
	assert(basic_info_return_size >= sizeof(sys_basic_info));

	system_info.cpu_count = sys_basic_info.number_of_processors;
	system_info.ram_size  = (u64)sys_basic_info.number_of_physical_pages * sys_basic_info.page_size;

	View<SystemProcessorPerformanceInformation> cpu_stats = {
		.len = (u64)sys_basic_info.number_of_processors * 2 // Alloc last and current together
	};
	scratch_alloc_array(&cpu_stats);
	cpu_stats.len /= 2;
	View<SystemProcessorPerformanceInformation> cpu_stats_last = {
		.ptr = cpu_stats.ptr + cpu_stats.len,
		.len = cpu_stats.len,
	};

	std::unordered_map<u32, ProcessData*> process_map;
	std::unordered_map<u32, ThreadData*>  thread_map;

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

		{
			// System Info
			u64 user      = 0;
			u64 kernel    = 0;
			u64 idle      = 0;
			u64 interrupt = 0;
			u64 dpc       = 0;
			for (u64 i = 0; i < cpu_stats.len; i++) {
				user      += cpu_stats[i].user_time;
				kernel    += cpu_stats[i].kernel_time;
				idle      += cpu_stats[i].idle_time;
				interrupt += cpu_stats[i].interrupt_time;
				dpc       += cpu_stats[i].dpc_time;
			}
			u64 total = user + kernel; // kernel is all non-user time.
			kernel -= idle;
			u64 cpu = total - idle;

			u64 ticks_per_second = 10'000'000;

			system_info.uptime         = (f64)total     / ticks_per_second / system_info.cpu_count;
			system_info.cpu_time       = (f64)cpu       / ticks_per_second;
			system_info.user_time      = (f64)user      / ticks_per_second;
			system_info.kernel_time    = (f64)kernel    / ticks_per_second;
			system_info.interrupt_time = (f64)interrupt / ticks_per_second;
			system_info.dpc_time       = (f64)dpc       / ticks_per_second;
			system_info.idle_time      = (f64)idle      / ticks_per_second;
			system_info.cpu_pct        = (f64)cpu       / total * 100;
			system_info.user_pct       = (f64)user      / total * 100;
			system_info.kernel_pct     = (f64)kernel    / total * 100;
			system_info.interrupt_pct  = (f64)interrupt / total * 100;
			system_info.dpc_pct        = (f64)dpc       / total * 100;
			system_info.idle_pct       = (f64)idle      / total * 100;
		}

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
				.time      = system_info.uptime,
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

			system_info.cpu_pct_tick       = percents.cpu;
			system_info.user_pct_tick      = percents.user;
			system_info.kernel_pct_tick    = percents.kernel;
			system_info.interrupt_pct_tick = percents.interrupt;
			system_info.dpc_pct_tick       = percents.dpc;
			system_info.idle_pct_tick      = percents.idle;
		}

		swap_vars(cpu_stats, cpu_stats_last);

		SystemPageFileInformationEx  page_file_info;
		SystemPerformanceInformation sys_perf_info;
		{
			// Poll memory stats
			u32 page_file_info_return_size;
			NtQuerySystemInformation(
				SysInfoClass_SYSTEM_PAGE_FILE_INFORMATION_EX,
				&page_file_info,
				sizeof(page_file_info),
				&page_file_info_return_size
			);
			assert(page_file_info_return_size >= sizeof(page_file_info));

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
				.time   = system_info.uptime,
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

			system_info.ram_used       = (u64)ram_used_pages                   * sys_basic_info.page_size;
			system_info.page_file_min  = (u64)page_file_info.minimum_size      * sys_basic_info.page_size;
			system_info.page_file_max  = (u64)page_file_info.maximum_size      * sys_basic_info.page_size;
			system_info.page_file_size = (u64)page_file_info.info.total_size   * sys_basic_info.page_size;
			system_info.page_file_used = (u64)page_file_info.info.total_in_use * sys_basic_info.page_size;
			system_info.commit_limit   = (u64)sys_perf_info.commit_limit       * sys_basic_info.page_size;
			system_info.commit_used    = (u64)sys_perf_info.committed_pages    * sys_basic_info.page_size;

			system_info.ram_pct       = percents.ram;
			system_info.page_file_pct = percents.swap;
			system_info.commit_pct    = percents.commit;

			if (poll_count) {
				system_info.system_calls_tick     = sys_perf_info.system_calls     - system_info.system_calls;
				system_info.context_switches_tick = sys_perf_info.context_switches - system_info.context_switches;
			}
			system_info.system_calls     = sys_perf_info.system_calls;
			system_info.context_switches = sys_perf_info.context_switches;
		}

		{
			// Processes

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

			u64 system_time;
			GetSystemTimeAsFileTime(&system_time);

			processes.count = 0;
			system_info.threads = 0;
			system_info.handles = 0;
			for (;;) {
				processes.count++;
				system_info.threads += proc_info->number_of_threads;
				system_info.handles += proc_info->handle_count;

				ProcessData* process = process_map[proc_info->unique_process_id];
				bool init_process = false;
				if (!process) {
					init_process = true;

					if (process_free_list) {
						process = process_free_list;
						process_free_list = process->next;
					} else {
						process = alloc_item(&arena, ProcessData);
					}

					process->alive = true;
					process->prev = processes.tail;
					process->next = NULL;
					if (!processes.head) {
						processes.head = process;
					} else {
						processes.tail->next = process;
					}
					processes.tail = process;

					process_map[proc_info->unique_process_id] = process;
				} else if (process->create_time != proc_info->create_time) {
					init_process = true;

					for (ThreadData* thread = process->threads.head; thread; thread = thread->next) {
						if (thread_map[thread->tid] == thread) { // TID might already be reused in prior process.
							thread_map.erase(thread->tid);
						}
					}
					if (process->threads.tail) {
						process->threads.tail->next = thread_free_list;
						thread_free_list = process->threads.head;
					}
				}

				if (init_process) {
					process->pid         = proc_info->unique_process_id;
					process->create_time = proc_info->create_time;
					process->threads     = { };

					if (process->pid) {
						process->image_name_len = WideCharToMultiByte(
							CP_UTF8, 0,
							proc_info->image_name.buffer, proc_info->image_name.length / sizeof(wchar_t),
							process->image_name, sizeof(process->image_name) - 1,
							NULL, NULL
						);
					} else {
						StringZ name = S("System Idle Process");
						process->image_name_len = name.len;
						memcpy(process->image_name, name.ptr, min(name.len, sizeof(process->image_name)));
					}
					process->image_name[process->image_name_len] = 0;

					process->history.offset = 0;
					process->history.length  = 0;
				}

				process->threads.count = proc_info->number_of_threads;
				process->handle_count  = proc_info->handle_count;

				const f64 ticks_per_second = 10'000'000;
				process->uptime      = (f64)(system_time - proc_info->create_time)          / ticks_per_second;
				process->user_time   = (f64)(proc_info->user_time)                          / ticks_per_second;
				process->kernel_time = (f64)(proc_info->kernel_time)                        / ticks_per_second;
				process->cpu_time    = (f64)(proc_info->user_time + proc_info->kernel_time) / ticks_per_second;

				process->user_pct   = process->user_time   / process->uptime / system_info.cpu_count * 100;
				process->kernel_pct = process->kernel_time / process->uptime / system_info.cpu_count * 100;
				process->cpu_pct    = process->cpu_time    / process->uptime / system_info.cpu_count * 100;

				process->ram              = proc_info->working_set_private_size;
				process->commit           = proc_info->private_page_count;
				process->hard_fault_count = proc_info->hard_fault_count;

				if (!init_process) { // We have a previous tick for an interval
					u64 user_diff         = proc_info->user_time   - process->user_cpu_last;
					u64 kernel_diff       = proc_info->kernel_time - process->kernel_cpu_last;
					u64 system_time_diff  = system_time            - process->system_time_last;
					u64 cpu_diff          = user_diff + kernel_diff;
					u64 cpu_total         = system_time_diff * sys_basic_info.number_of_processors;

					f64 cpu_pct    = ((f64)cpu_diff    / cpu_total) * 100;
					f64 user_pct   = ((f64)user_diff   / cpu_total) * 100;
					f64 kernel_pct = ((f64)kernel_diff / cpu_total) * 100;

					u64 total_ram    = (u64)sys_basic_info.number_of_physical_pages * sys_basic_info.page_size;
					u64 commit_limit = (u64)sys_perf_info.commit_limit              * sys_basic_info.page_size;
					f64 ram_pct      = ((f64)process->ram    / total_ram)    * 100;
					f64 commit_pct   = ((f64)process->commit / commit_limit) * 100;

					process->cpu_pct_tick    = cpu_pct;
					process->user_pct_tick   = user_pct;
					process->kernel_pct_tick = kernel_pct;

					u64 index = (process->history.offset + process->history.length) % arrlen(process->history.buffer);
					process->history.buffer[index].time   = process->uptime;
					process->history.buffer[index].cpu    = cpu_pct;
					process->history.buffer[index].user   = user_pct;
					process->history.buffer[index].kernel = kernel_pct;
					process->history.buffer[index].ram    = ram_pct;
					process->history.buffer[index].commit = commit_pct;

					if (process->history.length < arrlen(process->history.buffer)) {
						process->history.length++;
					} else {
						process->history.offset = (process->history.offset + 1) % arrlen(process->history.buffer);
					}

					process->history.start = process->uptime - 300.0;
					process->history.end   = process->uptime;
				} else {
					process->cpu_pct = 0.0;
				}

				process->system_time_last = system_time;
				process->user_cpu_last    = proc_info->user_time;
				process->kernel_cpu_last  = proc_info->kernel_time;

				for (u32 thread_idx = 0; thread_idx < proc_info->number_of_threads; thread_idx++) {
					SystemThreadInformation* thread_info = &proc_info->threads[thread_idx];

					ThreadData* thread = thread_map[thread_info->client_id.unique_thread];
					bool init_thread = false;
					if (!thread) {
						init_thread = true;

						if (thread_free_list) {
							thread = thread_free_list;
							thread_free_list = thread->next;
						} else {
							thread = alloc_item(&arena, ThreadData);
						}

						thread_map[thread_info->client_id.unique_thread] = thread;
					} else if (
						thread->create_time  != thread_info->create_time &&
						thread->process->pid != thread_info->client_id.unique_process
					) {
						init_thread = true;
					}

					if (init_thread) {
						thread->prev = process->threads.tail;
						thread->next = NULL;
						if (!process->threads.head) {
							process->threads.head = thread;
						} else {
							process->threads.tail->next = thread;
						}
						process->threads.tail = thread;

						thread->process     = process;
						thread->tid         = thread_info->client_id.unique_thread;
						thread->create_time = thread_info->create_time;

						thread->history.offset = 0;
						thread->history.length = 0;
					}

					thread->uptime      = (f64)(system_time - thread_info->create_time)            / ticks_per_second;
					thread->user_time   = (f64)(thread_info->user_time)                            / ticks_per_second;
					thread->kernel_time = (f64)(thread_info->kernel_time)                          / ticks_per_second;
					thread->cpu_time    = (f64)(thread_info->user_time + thread_info->kernel_time) / ticks_per_second;

					thread->context_switches = thread_info->context_switches;

					if (!init_thread) { // We have a previous tick for an interval
						u64 user_diff        = thread_info->user_time   - thread->user_cpu_last;
						u64 kernel_diff      = thread_info->kernel_time - thread->kernel_cpu_last;
						u64 system_time_diff = system_time              - thread->system_time_last;
						u64 cpu_diff         = user_diff + kernel_diff;
						u64 cpu_total        = system_time_diff * sys_basic_info.number_of_processors;
						thread->cpu_pct     = ((f64)cpu_diff / cpu_total) * 100;

						f64 cpu_pct    = ((f64)cpu_diff    / cpu_total) * 100;
						f64 user_pct   = ((f64)user_diff   / cpu_total) * 100;
						f64 kernel_pct = ((f64)kernel_diff / cpu_total) * 100;

						u64 index = (thread->history.offset + thread->history.length) % arrlen(thread->history.buffer);
						thread->history.buffer[index].time   = process->uptime; // Still based on process time for plot
						thread->history.buffer[index].cpu    = cpu_pct;
						thread->history.buffer[index].user   = user_pct;
						thread->history.buffer[index].kernel = kernel_pct;

						if (thread->history.length < arrlen(thread->history.buffer)) {
							thread->history.length++;
						} else {
							thread->history.offset = (thread->history.offset + 1) % arrlen(thread->history.buffer);
						}

						thread->history.start = process->uptime - 300.0;
						thread->history.end   = process->uptime;
					}

					thread->system_time_last = system_time;
					thread->user_cpu_last    = thread_info->user_time;
					thread->kernel_cpu_last  = thread_info->kernel_time;

					thread->touched = true;
				}

				process->touched = true;

				if (!proc_info->next_entry_offset) {
					break;
				}
				proc_info = (SystemProcessInformation*)((u8*)proc_info + proc_info->next_entry_offset);
			}

			// Remove everything untouched
			ProcessData* process = processes.head;
			while (process) {
				ProcessData* next_process = process->next;

				ThreadData* thread = process->threads.head;
				while (thread) {
					ThreadData* next_thread = thread->next;

					if (!thread->touched) {
						thread_map.erase(thread->tid);

						if (thread->prev) {
							thread->prev->next = thread->next;
						} else {
							process->threads.head = thread->next;
						}

						if (thread->next) {
							thread->next->prev = thread->prev;
						} else {
							process->threads.tail = thread->prev;
						}

						thread->next     = thread_free_list;
						thread_free_list = thread;
					}

					thread->touched = false;
					thread = next_thread;
				}

				if (!process->touched) {
					process_map.erase(process->pid);

					if (process->prev) {
						process->prev->next = process->next;
					} else {
						processes.head = process->next;
					}

					if (process->next) {
						process->next->prev = process->prev;
					} else {
						processes.tail = process->prev;
					}

					process->next     = process_free_list;
					process_free_list = process;

					process->alive = false;
				}

				process->touched = false;
				process = next_process;
			}

			system_info.processes = processes.count;

			scratch_end();
		}

		poll_count++;

		changes_ready = true;
		// TODO(hhammon) Do actual timing
		Sleep(1000);
	}

	arena_destroy(&arena);
	scratch_destroy();
	return 0;
}
