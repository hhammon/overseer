#ifndef __POLLING_HPP__
#define __POLLING_HPP__

#include "core.hpp"

// TODO(hhammon) None of this is thread safe. It doesn't really matter for now since there's only one writer
// so, at worst, things will just look weird for a frame.

#define POLLING_HISTORY_LENGTH (5 * 60 + 1)

struct CpuPercents {
	f64 time;
	f64 cpu;
	f64 user;
	f64 kernel;
	f64 interrupt;
	f64 dpc;
	f64 idle;
};

struct MemoryPercents {
	f64 time;
	f64 ram;
	f64 swap;
	f64 commit;
};

struct CpuHistory {
	CpuPercents* base;
	u64          length;
	u64          offset;
	f64          start;
	f64          end;
};

struct MemoryHistory {
	MemoryPercents* base;
	u64             length;
	u64             offset;
	f64             start;
	f64             end;
};

struct ThreadHistoryPoint {
	f64 time;
	f64 cpu;
	f64 user;
	f64 kernel;
};

struct ThreadHistory {
	ThreadHistoryPoint buffer[POLLING_HISTORY_LENGTH];
	u64                length;
	u64                offset;
	f64                start;
	f64                end;
};

struct ProcessHistoryPoint {
	f64 time;
	f64 cpu;
	f64 user;
	f64 kernel;
	f64 ram;
	f64 commit;
};

struct ProcessHistory {
	ProcessHistoryPoint buffer[POLLING_HISTORY_LENGTH];
	u64                 length;
	u64                 offset;
	f64                 start;
	f64                 end;
};

struct ProcessData;

struct ThreadData {
	ThreadData*   prev;
	ThreadData*   next;
	ProcessData*  process;
	u64           tid;
	u64           create_time;
	f64           uptime;
	f64           cpu_time;
	f64           user_time;
	f64           kernel_time;
	f64           cpu_pct;
	u64           context_switches;
	ThreadHistory history;
	u64           system_time_last; // Internal
	u64           user_cpu_last;    // Internal
	u64           kernel_cpu_last;  // Internal

	b8            touched; // Internal flag
};

struct ThreadList {
	ThreadData* head;
	ThreadData* tail;
	u64         count;
};

struct ProcessData {
	ProcessData*   prev;
	ProcessData*   next;
	u64            pid;
	u64            create_time;
	ThreadList     threads;
	u64            handle_count;
	f64            uptime;
	f64            user_time;
	f64            kernel_time;
	f64            cpu_time;
	f64            cpu_pct;
	u64            ram;
	u64            commit;
	u64            hard_fault_count;
	u64            image_name_len;
	char           image_name[256];
	ProcessHistory history;
	u64            system_time_last; // Internal
	u64            user_cpu_last;    // Internal
	u64            kernel_cpu_last;  // Internal
	b8             alive;
	b8             touched; // Internal flag
};

struct ProcessList {
	ProcessData* head;
	ProcessData* tail;
	u64          count;
};

api_method void polling_begin();
api_method void polling_end();
api_method bool polling_check_for_changes();

api_method CpuHistory    polling_get_cpu_history();
api_method MemoryHistory polling_get_memory_history();

api_method View<ProcessData*> polling_collect_processes(Arena* arena);
api_method View<ThreadData*>  polling_collect_threads  (Arena* arena, ProcessData* process);

#endif // __POLLING_HPP__
