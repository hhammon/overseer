#ifndef __POLLING_HPP__
#define __POLLING_HPP__

#include "core.hpp"

// TODO(hhammon) None of this is thread safe. It doesn't really matter for now since there's only one writer
// so, at worst, things will just look weird for a frame.

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

struct CpuHistoryCircularBuffer {
	CpuPercents* base;
	u64          length;
	u64          offset;
	f64          start;
	f64          end;
};

struct MemoryHistoryCircularBuffer {
	MemoryPercents* base;
	u64             length;
	u64             offset;
	f64             start;
	f64             end;
};

api_method void polling_begin();
api_method void polling_end();

api_method CpuHistoryCircularBuffer    polling_get_cpu_history();
api_method MemoryHistoryCircularBuffer polling_get_memory_history();

#endif // __POLLING_HPP__
