#include "polling.hpp"

#include "arena.hpp"
#include "windows.hpp"

global bool polling_running = false;

global SystemBasicInformation polling_sys_basic_info = { };

internal u32 WINCALLBACK polling_thread(void* param);

api_method void polling_begin() {
	polling_running = true;

	CreateThread(NULL, 0, polling_thread, NULL, 0, NULL);
}

api_method void polling_end() {
	polling_running = false;
}

internal u32 WINCALLBACK polling_thread(void* param) {
	unused_var(param);

	scratch_init();

	u32 basic_info_return_size = 0;
	NtQuerySystemInformation(
		SysInfoClass_SYSTEM_BASIC_INFORMATION,
		&polling_sys_basic_info,
		sizeof(polling_sys_basic_info),
		&basic_info_return_size
	);
	assert(basic_info_return_size >= sizeof(polling_sys_basic_info));

	while (polling_running) {
		// Poll system CPU usage
		View<SystemProcessorPerformanceInformation> sys_cpu_stats = {
			.len = polling_sys_basic_info.number_of_processors
		};
		scratch_alloc_array(&sys_cpu_stats);

		u32 sys_cpu_stats_return_size = 0;
		NtQuerySystemInformation(
			SysInfoClass_SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION,
			sys_cpu_stats.ptr,
			sizeof_view(sys_cpu_stats),
			&sys_cpu_stats_return_size
		);



		// TODO(hhammon) Do actual timing
		Sleep(1000);
	}
}
