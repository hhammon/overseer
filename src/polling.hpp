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
	f64 swap;
	f64 commit;
};

struct ProcessHistory {
	ProcessHistoryPoint buffer[POLLING_HISTORY_LENGTH];
	u64                 length;
	u64                 offset;
	f64                 start;
	f64                 end;
};

struct ThreadData {
	ThreadData*   prev;
	ThreadData*   next;
	u64           tid;
	u64           create_time;
	f64           uptime;
	f64           cpu_time;
	f64           user_time;
	f64           kernel_time;
	f64           cpu_pct_last;
	ThreadHistory history;
};

struct ProcessData {
	ProcessData*   prev;
	ProcessData*   next;
	u64            pid;
	u64            create_time;
	ThreadData*    threads_head_node;
	u32            thread_count;
	u32            handle_count;
	f64            uptime;
	f64            cpu_time;
	f64            user_time;
	f64            kernel_time;
	f64            cpu_pct_last;
	u64            ram;
	u64            swap;
	u64            va;
	u64            commit;
	u64            image_name_len;
	u8             image_name[256];
	ProcessHistory history;
};

const int x = sizeof(ProcessData);
const int y = sizeof(ThreadData);

api_method void polling_begin();
api_method void polling_end();

api_method CpuHistory    polling_get_cpu_history();
api_method MemoryHistory polling_get_memory_history();

#endif // __POLLING_HPP__
