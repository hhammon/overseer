#ifndef __WINDOWS_HPP__
#define __WINDOWS_HPP__

#pragma comment(lib, "libcmt.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "d3d11.lib")

#include "core.hpp"

#define WINAPI      __stdcall
#define NTAPI       __stdcall
#define WINCALLBACK __stdcall
#define DLLIMPORT no_mangle __declspec(dllimport)

typedef struct HWND__ {
	int unused;
}* Handle;
typedef Handle HModule;
typedef Handle HInstance;
typedef Handle HWnd;
typedef Handle HMonitor;
typedef Handle HIcon;
typedef Handle HCursor;
typedef Handle HBrush;
typedef Handle HMenu;

typedef s32 NtStatus;

typedef s32 HResult;
#define SUCCEEDED(result) ((bool)((HResult)(result) >= 0))
#define FAILED(   result) ((bool)((HResult)(result) <  0))

/**
 * ==========
 * WINTERNALS
 * ==========
 *
 * These are APIs that are close to the kernel and aren't really designed for regular linking. Much of this isn't
 * even properly documented or included in Windows header files and so is needed here. For this jam project, I'm
 * not going to be thinking too hard about whether APIs are stable and for which versions of Windows things will
 * work, but that is a concern to have. -hhammon
 *
 * Much thanks to [Geoff Chappell](geoffchappell.com) and others for their reverse-engineering work.
 */

// NOTE(hhammon) @NtStatus If the value in NtStatus ever becomes relevant, locate it here:
// #include <ntstatus.h>

#define NT_STATUS_INFO_LENGTH_MISMATCH ((NtStatus)0xc0000004)

/**
 * https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/ntexapi/system_information_class.htm
 * https://ntdoc.m417z.com/system_information_class
 */
enum SysInfoClass {
	SysInfoClass_SYSTEM_BASIC_INFORMATION                                    = 0x00, //all
	SysInfoClass_SYSTEM_PROCESSOR_INFORMATION                                = 0x01, //all
	SysInfoClass_SYSTEM_PERFORMANCE_INFORMATION                              = 0x02, //all
	SysInfoClass_SYSTEM_TIME_OF_DAY_INFORMATION                              = 0x03, //all
	SysInfoClass_SYSTEM_PATH_INFORMATION                                     = 0x04, //all
	SysInfoClass_SYSTEM_PROCESS_INFORMATION                                  = 0x05, //all
	SysInfoClass_SYSTEM_CALL_COUNT_INFORMATION                               = 0x06, //all
	SysInfoClass_SYSTEM_DEVICE_INFORMATION                                   = 0x07, //all
	SysInfoClass_SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION                    = 0x08, //all
	SysInfoClass_SYSTEM_FLAGS_INFORMATION                                    = 0x09, //all
	SysInfoClass_SYSTEM_CALL_TIME_INFORMATION                                = 0x0A, //all
	SysInfoClass_SYSTEM_MODULE_INFORMATION                                   = 0x0B, //all
	SysInfoClass_SYSTEM_LOCKS_INFORMATION                                    = 0x0C, //all
	SysInfoClass_SYSTEM_STACK_TRACE_INFORMATION                              = 0x0D, //all
	SysInfoClass_SYSTEM_PAGED_POOL_INFORMATION                               = 0x0E, //all
	SysInfoClass_SYSTEM_NON_PAGED_POOL_INFORMATION                           = 0x0F, //all
	SysInfoClass_SYSTEM_HANDLE_INFORMATION                                   = 0x10, //all
	SysInfoClass_SYSTEM_OBJECT_INFORMATION                                   = 0x11, //all
	SysInfoClass_SYSTEM_PAGE_FILE_INFORMATION                                = 0x12, //all
	SysInfoClass_SYSTEM_VDM_INSTEMUL_INFORMATION                             = 0x13, //all
	SysInfoClass_SYSTEM_VDM_BOP_INFORMATION                                  = 0x14, //all
	SysInfoClass_SYSTEM_FILE_CACHE_INFORMATION                               = 0x15, //all
	SysInfoClass_SYSTEM_POOL_TAG_INFORMATION                                 = 0x16, //3.50 and higher
	SysInfoClass_SYSTEM_INTERRUPT_INFORMATION                                = 0x17, //3.51 and higher
	SysInfoClass_SYSTEM_DPC_BEHAVIOR_INFORMATION                             = 0x18, //3.51 and higher
	SysInfoClass_SYSTEM_FULL_MEMORY_INFORMATION                              = 0x19, //4.0 and higher
	SysInfoClass_SYSTEM_LOAD_GDI_DRIVER_INFORMATION                          = 0x1A, //3.51 and higher
	SysInfoClass_SYSTEM_UNLOAD_GDI_DRIVER_INFORMATION                        = 0x1B, //3.51 and higher
	SysInfoClass_SYSTEM_TIME_ADJUSTMENT_INFORMATION                          = 0x1C, //3.50 and higher
	SysInfoClass_SYSTEM_SUMMARY_MEMORY_INFORMATION                           = 0x1D, //4.0 and higher
	SysInfoClass_SYSTEM_MIRROR_MEMORY_INFORMATION                            = 0x1E, //5.1 and higher
	SysInfoClass_SYSTEM_PERFORMANCE_TRACE_INFORMATION                        = 0x1F, //6.0 and higher
	SysInfoClass_SYSTEM_OBSOLETE0                                            = 0x20, //5.1 and higher
	SysInfoClass_SYSTEM_EXCEPTION_INFORMATION                                = 0x21, //3.50 and higher
	SysInfoClass_SYSTEM_CRASH_DUMP_STATE_INFORMATION                         = 0x22, //3.50 and higher
	SysInfoClass_SYSTEM_KERNEL_DEBUGGER_INFORMATION                          = 0x23, //3.50 and higher
	SysInfoClass_SYSTEM_CONTEXT_SWITCH_INFORMATION                           = 0x24, //3.50 and higher
	SysInfoClass_SYSTEM_REGISTRY_QUOTA_INFORMATION                           = 0x25, //3.51 and higher
	SysInfoClass_SYSTEM_EXTEND_SERVICE_TABLE_INFORMATION                     = 0x26, //3.51 and higher
	SysInfoClass_SYSTEM_PRIORITY_SEPERATION                                  = 0x27, //3.51 and higher
	SysInfoClass_SYSTEM_VERIFIER_ADD_DRIVER_INFORMATION                      = 0x28, //5.1 and higher
	SysInfoClass_SYSTEM_VERIFIER_REMOVE_DRIVER_INFORMATION                   = 0x29, //5.1 and higher
	SysInfoClass_SYSTEM_PROCESSOR_IDLE_INFORMATION                           = 0x2A, //5.1 and higher
	SysInfoClass_SYSTEM_LEGACY_DRIVER_INFORMATION                            = 0x2B, //5.0 and higher
	SysInfoClass_SYSTEM_CURRENT_TIME_ZONE_INFORMATION                        = 0x2C, //4.0 and higher
	SysInfoClass_SYSTEM_LOOKASIDE_INFORMATION                                = 0x2D, //4.0 and higher
	SysInfoClass_SYSTEM_TIME_SLIP_NOTIFICATION                               = 0x2E, //5.0 and higher
	SysInfoClass_SYSTEM_SESSION_CREATE                                       = 0x2F, //5.0 and higher
	SysInfoClass_SYSTEM_SESSION_DETACH                                       = 0x30, //5.0 and higher
	SysInfoClass_SYSTEM_SESSION_INFORMATION                                  = 0x31, //5.0 and higher
	SysInfoClass_SYSTEM_RANGE_START_INFORMATION                              = 0x32, //5.0 and higher
	SysInfoClass_SYSTEM_VERIFIER_INFORMATION                                 = 0x33, //5.0 and higher
	SysInfoClass_SYSTEM_VERIFIER_THUNK_EXTEND                                = 0x34, //5.0 and higher
	SysInfoClass_SYSTEM_SESSION_PROCESS_INFORMATION                          = 0x35, //5.0 and higher
	SysInfoClass_SYSTEM_LOAD_GDI_DRIVER_IN_SYSTEM_SPACE                      = 0x36, //5.1 and higher
	SysInfoClass_SYSTEM_NUMA_PROCESSOR_MAP                                   = 0x37, //5.1 and higher
	SysInfoClass_SYSTEM_PREFETCHER_INFORMATION                               = 0x38, //5.1 and higher
	SysInfoClass_SYSTEM_EXTENDED_PROCESS_INFORMATION                         = 0x39, //5.1 and higher
	SysInfoClass_SYSTEM_RECOMMENDED_SHARED_DATA_ALIGNMENT                    = 0x3A, //5.1 and higher
	SysInfoClass_SYSTEM_COM_PLUS_PACKAGE                                     = 0x3B, //5.1 and higher
	SysInfoClass_SYSTEM_NUMA_AVAILABLE_MEMORY                                = 0x3C, //5.1 and higher
	SysInfoClass_SYSTEM_PROCESSOR_POWER_INFORMATION                          = 0x3D, //5.1 and higher
	SysInfoClass_SYSTEM_EMULATION_BASIC_INFORMATION                          = 0x3E, //5.1 and higher
	SysInfoClass_SYSTEM_EMULATION_PROCESSOR_INFORMATION                      = 0x3F, //5.1 and higher
	SysInfoClass_SYSTEM_EXTENDED_HANDLE_INFORMATION                          = 0x40, //5.1 and higher
	SysInfoClass_SYSTEM_LOST_DELAYED_WRITE_INFORMATION                       = 0x41, //5.1 and higher
	SysInfoClass_SYSTEM_BIG_POOL_INFORMATION                                 = 0x42, //5.2 and higher
	SysInfoClass_SYSTEM_SESSION_POOL_TAG_INFORMATION                         = 0x43, //5.2 and higher
	SysInfoClass_SYSTEM_SESSION_MAPPED_VIEW_INFORMATION                      = 0x44, //5.2 and higher
	SysInfoClass_SYSTEM_HOTPATCH_INFORMATION                                 = 0x45, //late 5.1 and higher
	SysInfoClass_SYSTEM_OBJECT_SECURITY_MODE                                 = 0x46, //late 5.1 and higher	earlier as 0x36
	SysInfoClass_SYSTEM_WATCHDOG_TIMER_HANDLER                               = 0x47, //5.2 and higher
	SysInfoClass_SYSTEM_WATCHDOG_TIMER_INFORMATION                           = 0x48, //5.2 and higher
	SysInfoClass_SYSTEM_LOGICAL_PROCESSOR_INFORMATION                        = 0x49, //very late 5.1 and higher
	SysInfoClass_SYSTEM_WOW64_SHARED_INFORMATION_OBSOLETE                    = 0x4A, //late 5.2 and higher
	SysInfoClass_SYSTEM_REGISTER_FIRMWARE_TABLE_INFORMATION_HANDLER          = 0x4B, //late 5.2 and higher
	SysInfoClass_SYSTEM_FIRMWARE_TABLE_INFORMATION                           = 0x4C, //late 5.2 and higher
	SysInfoClass_SYSTEM_MODULE_INFORMATION_EX                                = 0x4D, //6.0 and higher
	SysInfoClass_SYSTEM_VERIFIER_TRIAGE_INFORMATION                          = 0x4E, //6.0 and higher
	SysInfoClass_SYSTEM_SUPERFETCH_INFORMATION                               = 0x4F, //6.0 and higher
	SysInfoClass_SYSTEM_MEMORY_LIST_INFORMATION                              = 0x50, //6.0 and higher
	SysInfoClass_SYSTEM_FILE_CACHE_INFORMATION_EX                            = 0x51, //late 5.2 and higher
	SysInfoClass_SYSTEM_THREAD_PRIORITY_CLIENT_ID_INFORMATION                = 0x52, //6.0 and higher
	SysInfoClass_SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION                = 0x53, //6.0 and higher
	SysInfoClass_SYSTEM_VERIFIER_CANCELLATION_INFORMATION                    = 0x54, //6.0 and higher
	SysInfoClass_SYSTEM_PROCESSOR_POWER_INFORMATION_EX                       = 0x55, //6.0 and higher
	SysInfoClass_SYSTEM_REF_TRACE_INFORMATION                                = 0x56, //6.0 and higher
	SysInfoClass_SYSTEM_SPECIAL_POOL_INFORMATION                             = 0x57, //6.0 and higher
	SysInfoClass_SYSTEM_PROCESS_ID_INFORMATION                               = 0x58, //6.0 and higher
	SysInfoClass_SYSTEM_ERROR_PORT_INFORMATION                               = 0x59, //6.0 and higher
	SysInfoClass_SYSTEM_BOOT_ENVIRONMENT_INFORMATION                         = 0x5A, //6.0 and higher
	SysInfoClass_SYSTEM_HYPERVISOR_INFORMATION                               = 0x5B, //6.0 and higher
	SysInfoClass_SYSTEM_VERIFIER_INFORMATION_EX                              = 0x5C, //6.0 and higher
	SysInfoClass_SYSTEM_TIME_ZONE_INFORMATION                                = 0x5D, //6.0 and higher
	SysInfoClass_SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION             = 0x5E, //6.0 and higher
	SysInfoClass_SYSTEM_COVERAGE_INFORMATION                                 = 0x5F, //6.0 and higher
	SysInfoClass_SYSTEM_PREFETCH_PATCH_INFORMATION                           = 0x60, //6.0 and higher
	SysInfoClass_SYSTEM_VERIFIER_FAULTS_INFORMATION                          = 0x61, //6.0 and higher
	SysInfoClass_SYSTEM_SYSTEM_PARTITION_INFORMATION                         = 0x62, //6.0 and higher
	SysInfoClass_SYSTEM_SYSTEM_DISK_INFORMATION                              = 0x63, //6.0 and higher
	SysInfoClass_SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION                   = 0x64, //6.0 and higher
	SysInfoClass_SYSTEM_NUMA_PROXIMITY_NODE_INFORMATION                      = 0x65, //6.0 and higher
	SysInfoClass_SYSTEM_DYNAMIC_TIME_ZONE_INFORMATION                        = 0x66, //6.0 and higher
	SysInfoClass_SYSTEM_CODE_INTEGRITY_INFORMATION                           = 0x67, //6.0 and higher
	SysInfoClass_SYSTEM_PROCESSOR_MICROCODE_UPDATE_INFORMATION               = 0x68, //6.0 and higher
	SysInfoClass_SYSTEM_PROCESSOR_BRAND_STRING                               = 0x69, //late 6.0 and higher
	SysInfoClass_SYSTEM_VIRTUAL_ADDRESS_INFORMATION                          = 0x6A, //late 6.0 and higher
	SysInfoClass_SYSTEM_LOGICAL_PROCESSOR_AND_GROUP_INFORMATION              = 0x6B, //6.1 and higher
	SysInfoClass_SYSTEM_PROCESSOR_CYCLE_TIME_INFORMATION                     = 0x6C, //6.1 and higher
	SysInfoClass_SYSTEM_STORE_INFORMATION                                    = 0x6D, //6.1 and higher
	SysInfoClass_SYSTEM_REGISTRY_APPEND_STRING                               = 0x6E, //6.1 and higher
	SysInfoClass_SYSTEM_AIT_SAMPLING_VALUE                                   = 0x6F, //6.1 and higher
	SysInfoClass_SYSTEM_VHD_BOOT_INFORMATION                                 = 0x70, //6.1 and higher
	SysInfoClass_SYSTEM_CPU_QUOTA_INFORMATION                                = 0x71, //6.1 and higher
	SysInfoClass_SYSTEM_NATIVE_BASIC_INFORMATION                             = 0x72, //6.1 and higher
	SysInfoClass_SYSTEM_ERROR_PORT_TIMEOUTS                                  = 0x73, //6.1 and higher
	SysInfoClass_SYSTEM_LOW_PRIORITY_IO_INFORMATION                          = 0x74, //6.1 and higher
	SysInfoClass_SYSTEM_BOOT_ENTROPY_INFORMATION                             = 0x75, //6.1 and higher
	SysInfoClass_SYSTEM_VERIFIER_COUNTERS_INFORMATION                        = 0x76, //6.1 and higher
	SysInfoClass_SYSTEM_PAGED_POOL_INFORMATION_EX                            = 0x77, //6.1 and higher
	SysInfoClass_SYSTEM_SYSTEM_PTES_INFORMATION_EX                           = 0x78, //6.1 and higher
	SysInfoClass_SYSTEM_NODE_DISTANCE_INFORMATION                            = 0x79, //6.1 and higher
	SysInfoClass_SYSTEM_ACPI_AUDIT_INFORMATION                               = 0x7A, //6.1 and higher
	SysInfoClass_SYSTEM_BASIC_PERFORMANCE_INFORMATION                        = 0x7B, //6.1 and higher
	SysInfoClass_SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION                = 0x7C, //late 6.1 and higher
	SysInfoClass_SYSTEM_SESSION_BIG_POOL_INFORMATION                         = 0x7D, //6.2 and higher
	SysInfoClass_SYSTEM_BOOT_GRAPHICS_INFORMATION                            = 0x7E, //6.2 and higher
	SysInfoClass_SYSTEM_SCRUB_PHYSICAL_MEMORY_INFORMATION                    = 0x7F, //6.2 and higher
	SysInfoClass_SYSTEM_BAD_PAGE_INFORMATION                                 = 0x80, //6.2 and higher
	SysInfoClass_SYSTEM_PROCESSOR_PROFILE_CONTROL_AREA                       = 0x81, //6.2 and higher
	SysInfoClass_SYSTEM_COMBINE_PHYSICAL_MEMORY_INFORMATION                  = 0x82, //6.2 and higher
	SysInfoClass_SYSTEM_ENTROPY_INTERRUPT_TIMING_INFORMATION                 = 0x83, //6.2 and higher
	SysInfoClass_SYSTEM_CONSOLE_INFORMATION                                  = 0x84, //6.2 and higher
	SysInfoClass_SYSTEM_PLATFORM_BINARY_INFORMATION                          = 0x85, //6.2 and higher
	SysInfoClass_SYSTEM_POLICY_INFORMATION                                   = 0x86, //6.3 and higher
	SysInfoClass_SYSTEM_HYPERVISOR_PROCESSOR_COUNT_INFORMATION               = 0x87, //6.2 and higher
	SysInfoClass_SYSTEM_DEVICE_DATA_INFORMATION                              = 0x88, //6.2 and higher
	SysInfoClass_SYSTEM_DEVICE_DATA_ENUMERATION_INFORMATION                  = 0x89, //6.2 and higher
	SysInfoClass_SYSTEM_MEMORY_TOPOLOGY_INFORMATION                          = 0x8A, //6.2 and higher
	SysInfoClass_SYSTEM_MEMORY_CHANNEL_INFORMATION                           = 0x8B, //6.2 and higher
	SysInfoClass_SYSTEM_BOOT_LOGO_INFORMATION                                = 0x8C, //6.2 and higher
	SysInfoClass_SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX                 = 0x8D, //6.2 and higher
	SysInfoClass_SYSTEM_CRITICAL_PROCESS_ERROR_LOG_INFORMATION               = 0x8E, //1607 and higher
	SysInfoClass_SYSTEM_SECURE_BOOT_POLICY_INFORMATION                       = 0x8F, //6.2 and higher
	SysInfoClass_SYSTEM_PAGE_FILE_INFORMATION_EX                             = 0x90, //6.2 and higher
	SysInfoClass_SYSTEM_SECURE_BOOT_INFORMATION                              = 0x91, //6.2 and higher
	SysInfoClass_SYSTEM_ENTROPY_INTERRUPT_TIMING_RAW_INFORMATION             = 0x92, //6.2 and higher
	SysInfoClass_SYSTEM_PORTABLE_WORKSPACE_EFI_LAUNCHER_INFORMATION          = 0x93, //6.2 and higher
	SysInfoClass_SYSTEM_FULL_PROCESS_INFORMATION                             = 0x94, //6.2 and higher
	SysInfoClass_SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX                       = 0x95, //6.3 and higher
	SysInfoClass_SYSTEM_BOOT_METADATA_INFORMATION                            = 0x96, //6.3 and higher
	SysInfoClass_SYSTEM_SOFT_REBOOT_INFORMATION                              = 0x97, //6.3 and higher
	SysInfoClass_SYSTEM_ELAM_CERTIFICATE_INFORMATION                         = 0x98, //6.3 and higher
	SysInfoClass_SYSTEM_OFFLINE_DUMP_CONFIG_INFORMATION                      = 0x99, //6.3 and higher
	SysInfoClass_SYSTEM_PROCESSOR_FEATURES_INFORMATION                       = 0x9A, //6.3 and higher
	SysInfoClass_SYSTEM_REGISTRY_RECONCILIATION_INFORMATION                  = 0x9B, //6.3 and higher
	SysInfoClass_SYSTEM_EDID_INFORMATION                                     = 0x9C, //6.3 and higher
	SysInfoClass_SYSTEM_MANUFACTURING_INFORMATION                            = 0x9D, //10.0 and higher
	SysInfoClass_SYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION                 = 0x9E, //10.0 and higher
	SysInfoClass_SYSTEM_HYPERVISOR_DETAIL_INFORMATION                        = 0x9F, //10.0 and higher
	SysInfoClass_SYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION                    = 0xA0, //10.0 and higher
	SysInfoClass_SYSTEM_VM_GENERATION_COUNT_INFORMATION                      = 0xA1, //10.0 and higher
	SysInfoClass_SYSTEM_TRUSTED_PLATFORM_MODULE_INFORMATION                  = 0xA2, //10.0 and higher
	SysInfoClass_SYSTEM_KERNEL_DEBUGGER_FLAGS                                = 0xA3, //10.0 and higher
	SysInfoClass_SYSTEM_CODE_INTEGRITY_POLICY_INFORMATION                    = 0xA4, //10.0 and higher
	SysInfoClass_SYSTEM_ISOLATED_USER_MODE_INFORMATION                       = 0xA5, //10.0 and higher
	SysInfoClass_SYSTEM_HARDWARE_SECURITY_TEST_INTERFACE_RESULTS_INFORMATION = 0xA6, //10.0 and higher
	SysInfoClass_SYSTEM_SINGLE_MODULE_INFORMATION                            = 0xA7, //10.0 and higher
	SysInfoClass_SYSTEM_ALLOWED_CPU_SETS_INFORMATION                         = 0xA8, //10.0 and higher
	SysInfoClass_SYSTEM_DMA_PROTECTION_INFORMATION                           = 0xA9, //10.0 and higher
	SysInfoClass_SYSTEM_INTERRUPT_CPU_SETS_INFORMATION                       = 0xAA, //10.0 and higher
	SysInfoClass_SYSTEM_SECURE_BOOT_POLICY_FULL_INFORMATION                  = 0xAB, //10.0 and higher
	SysInfoClass_SYSTEM_CODE_INTEGRITY_POLICY_FULL_INFORMATION               = 0xAC, //10.0 and higher
	SysInfoClass_SYSTEM_AFFINITIZED_INTERRUPT_PROCESSOR_INFORMATION          = 0xAD, //10.0 and higher
	SysInfoClass_SYSTEM_ROOT_SILO_INFORMATION                                = 0xAE, //10.0 and higher
	SysInfoClass_SYSTEM_CPU_SET_INFORMATION                                  = 0xAF, //10.0 and higher
	SysInfoClass_SYSTEM_CPU_SET_TAG_INFORMATION                              = 0xB0, //10.0 and higher
	SysInfoClass_SYSTEM_WIN32_WER_START_CALLOUT                              = 0xB1, //1511 and higher
	SysInfoClass_SYSTEM_SECURE_KERNEL_PROFILE_INFORMATION                    = 0xB2, //1511 and higher
	SysInfoClass_SYSTEM_CODE_INTEGRITY_PLATFORM_MANIFEST_INFORMATION         = 0xB3, //1607 and higher
	SysInfoClass_SYSTEM_INTERRUPT_STEERING_INFORMATION                       = 0xB4, //1607 and higher
	SysInfoClass_SYSTEM_SUPPPORTED_PROCESSOR_ARCHITECTURES                   = 0xB5, //1607 and higher
	SysInfoClass_SYSTEM_MEMORY_USAGE_INFORMATION                             = 0xB6, //1607 and higher
	SysInfoClass_SYSTEM_CODE_INTEGRITY_CERTIFICATE_INFORMATION               = 0xB7, //1607 and higher
	SysInfoClass_SYSTEM_PHYSICAL_MEMORY_INFORMATION                          = 0xB8, //1703 and higher
	SysInfoClass_SYSTEM_CONTROL_FLOW_TRANSITION                              = 0xB9, //1703 and higher
	SysInfoClass_SYSTEM_KERNEL_DEBUGGING_ALLOWED                             = 0xBA, //1703 and higher
	SysInfoClass_SYSTEM_ACTIVITY_MODERATION_EXE_STATE                        = 0xBB, //1703 and higher
	SysInfoClass_SYSTEM_ACTIVITY_MODERATION_USER_SETTINGS                    = 0xBC, //1703 and higher
	SysInfoClass_SYSTEM_CODE_INTEGRITY_POLICIES_FULL_INFORMATION             = 0xBD, //1703 and higher
	SysInfoClass_SYSTEM_CODE_INTEGRITY_UNLOCK_INFORMATION                    = 0xBE, //1703 and higher
	SysInfoClass_SYSTEM_INTEGRITY_QUOTA_INFORMATION                          = 0xBF, //1703 and higher
	SysInfoClass_SYSTEM_FLUSH_INFORMATION                                    = 0xC0, //1703 and higher
	SysInfoClass_SYSTEM_PROCESSOR_IDLE_MASK_INFORMATION                      = 0xC1, //1709 and higher
	SysInfoClass_SYSTEM_SECURE_DUMP_ENCRYPTION_INFORMATION                   = 0xC2, //1709 and higher
	SysInfoClass_SYSTEM_WRITE_CONSTRAINT_INFORMATION                         = 0xC3, //1709 and higher
	SysInfoClass_SYSTEM_KERNEL_VA_SHADOW_INFORMATION                         = 0xC4, //1803 and higher
	SysInfoClass_SYSTEM_HYPERVISOR_SHARED_PAGE_INFORMATION                   = 0xC5, //1803 and higher
	SysInfoClass_SYSTEM_FIRMWARE_BOOT_PERFORMANCE_INFORMATION                = 0xC6, //1803 and higher
	SysInfoClass_SYSTEM_CODE_INTEGRITY_VERIFICATION_INFORMATION              = 0xC7, //1803 and higher
	SysInfoClass_SYSTEM_FIRMWARE_PARTITION_INFORMATION                       = 0xC8, //1803 and higher
	SysInfoClass_SYSTEM_SPECULATION_CONTROL_INFORMATION                      = 0xC9, //1803 and higher
	SysInfoClass_SYSTEM_DMA_GUARD_POLICY_INFORMATION                         = 0xCA, //1803 and higher
	SysInfoClass_SYSTEM_ENCLAVE_LAUNCH_CONTROL_INFORMATION                   = 0xCB, //1803 and higher
	SysInfoClass_SYSTEM_WORKLOAD_ALLOWED_CPU_SETS_INFORMATION                = 0xCC, //1809 and higher
	SysInfoClass_SYSTEM_CODE_INTEGRITY_UNLOCK_MODE_INFORMATION               = 0xCD, //1809 and higher
	SysInfoClass_SYSTEM_LEAP_SECOND_INFORMATION                              = 0xCE, //1809 and higher
	SysInfoClass_SYSTEM_FLAGS2_INFORMATION                                   = 0xCF, //1809 and higher
	SysInfoClass_SYSTEM_SECURITY_MODEL_INFORMATION                           = 0xD0, //1903 and higher
	SysInfoClass_SYSTEM_CODE_INTEGRITY_SYNTHETIC_CACHE_INFORMATION           = 0xD1, //1903 and higher
	SysInfoClass_SYSTEM_FEATURE_CONFIGURATION_INFORMATION                    = 0xD2, //2004 and higher
	SysInfoClass_SYSTEM_FEATURE_CONFIGURATION_SECTION_INFORMATION            = 0xD3, //2004 and higher
	SysInfoClass_SYSTEM_FEATURE_USAGE_SUBSCRIPTION_INFORMATION               = 0xD4, //2004 and higher
	SysInfoClass_SYSTEM_SECURE_SPECULATION_CONTROL_INFORMATION               = 0xD5, //2004 and higher
};

/**
 * https://ntdoc.m417z.com/processinfoclass
 */
enum ProcInfoClass {
	ProcInfoClass_PROCESS_BASIC_INFORMATION,
    ProcInfoClass_PROCESS_QUOTA_LIMITS,
    ProcInfoClass_PROCESS_IO_COUNTERS,
    ProcInfoClass_PROCESS_VM_COUNTERS,
    ProcInfoClass_PROCESS_TIMES,
    ProcInfoClass_PROCESS_BASE_PRIORITY,
    ProcInfoClass_PROCESS_RAISE_PRIORITY,
    ProcInfoClass_PROCESS_DEBUG_PORT,
    ProcInfoClass_PROCESS_EXCEPTION_PORT,
    ProcInfoClass_PROCESS_ACCESS_TOKEN,
    ProcInfoClass_PROCESS_LDT_INFORMATION,
    ProcInfoClass_PROCESS_LDT_SIZE,
    ProcInfoClass_PROCESS_DEFAULT_HARD_ERROR_MODE,
    ProcInfoClass_PROCESS_IO_PORT_HANDLERS,
    ProcInfoClass_PROCESS_POOLED_USAGE_AND_LIMITS,
    ProcInfoClass_PROCESS_WORKING_SET_WATCH,
    ProcInfoClass_PROCESS_USER_MODE_IOPL,
    ProcInfoClass_PROCESS_ENABLE_ALIGNMENT_FAULT_FIXUP,
    ProcInfoClass_PROCESS_PRIORITY_CLASS,
    ProcInfoClass_PROCESS_WX86_INFORMATION,
    ProcInfoClass_PROCESS_HANDLE_COUNT,
    ProcInfoClass_PROCESS_AFFINITY_MASK,
    ProcInfoClass_PROCESS_PRIORITY_BOOST,
    ProcInfoClass_PROCESS_DEVICE_MAP,
    ProcInfoClass_PROCESS_SESSION_INFORMATION,
    ProcInfoClass_PROCESS_FOREGROUND_INFORMATION,
    ProcInfoClass_PROCESS_WOW64_INFORMATION,
    ProcInfoClass_PROCESS_IMAGE_FILE_NAME,
    ProcInfoClass_PROCESS_LUID_DEVICE_MAPS_ENABLED,
    ProcInfoClass_PROCESS_BREAK_ON_TERMINATION,
    ProcInfoClass_PROCESS_DEBUG_OBJECT_HANDLE,
    ProcInfoClass_PROCESS_DEBUG_FLAGS,
    ProcInfoClass_PROCESS_HANDLE_TRACING,
    ProcInfoClass_PROCESS_IO_PRIORITY,
    ProcInfoClass_PROCESS_EXECUTE_FLAGS,
    ProcInfoClass_PROCESS_TLS_INFORMATION,
    ProcInfoClass_PROCESS_COOKIE,
    ProcInfoClass_PROCESS_IMAGE_INFORMATION,
    ProcInfoClass_PROCESS_CYCLE_TIME,
    ProcInfoClass_PROCESS_PAGE_PRIORITY,
    ProcInfoClass_PROCESS_INSTRUMENTATION_CALLBACK,
    ProcInfoClass_PROCESS_THREAD_STACK_ALLOCATION,
    ProcInfoClass_PROCESS_WORKING_SET_WATCH_EX,
    ProcInfoClass_PROCESS_IMAGE_FILE_NAME_WIN32,
    ProcInfoClass_PROCESS_IMAGE_FILE_MAPPING,
    ProcInfoClass_PROCESS_AFFINITY_UPDATE_MODE,
    ProcInfoClass_PROCESS_MEMORY_ALLOCATION_MODE,
    ProcInfoClass_PROCESS_GROUP_INFORMATION,
    ProcInfoClass_PROCESS_TOKEN_VIRTUALIZATION_ENABLED,
    ProcInfoClass_PROCESS_CONSOLE_HOST_PROCESS,
    ProcInfoClass_PROCESS_WINDOW_INFORMATION,
    ProcInfoClass_PROCESS_HANDLE_INFORMATION,
    ProcInfoClass_PROCESS_MITIGATION_POLICY,
    ProcInfoClass_PROCESS_DYNAMIC_FUNCTION_TABLE_INFORMATION,
    ProcInfoClass_PROCESS_HANDLE_CHECKING_MODE,
    ProcInfoClass_PROCESS_KEEP_ALIVE_COUNT,
    ProcInfoClass_PROCESS_REVOKE_FILE_HANDLES,
    ProcInfoClass_PROCESS_WORKING_SET_CONTROL,
    ProcInfoClass_PROCESS_HANDLE_TABLE,
    ProcInfoClass_PROCESS_CHECK_STACK_EXTENTS_MODE,
    ProcInfoClass_PROCESS_COMMAND_LINE_INFORMATION,
    ProcInfoClass_PROCESS_PROTECTION_INFORMATION,
    ProcInfoClass_PROCESS_MEMORY_EXHAUSTION,
    ProcInfoClass_PROCESS_FAULT_INFORMATION,
    ProcInfoClass_PROCESS_TELEMETRY_ID_INFORMATION,
    ProcInfoClass_PROCESS_COMMIT_RELEASE_INFORMATION,
    ProcInfoClass_PROCESS_DEFAULT_CPU_SETS_INFORMATION,
    ProcInfoClass_PROCESS_ALLOWED_CPU_SETS_INFORMATION,
    ProcInfoClass_PROCESS_SUBSYSTEM_PROCESS,
    ProcInfoClass_PROCESS_JOB_MEMORY_INFORMATION,
    ProcInfoClass_PROCESS_IN_PRIVATE,
    ProcInfoClass_PROCESS_RAISE_UM_EXCEPTION_ON_INVALID_HANDLE_CLOSE,
    ProcInfoClass_PROCESS_IUM_CHALLENGE_RESPONSE,
    ProcInfoClass_PROCESS_CHILD_PROCESS_INFORMATION,
    ProcInfoClass_PROCESS_HIGH_GRAPHICS_PRIORITY_INFORMATION,
    ProcInfoClass_PROCESS_SUBSYSTEM_INFORMATION,
    ProcInfoClass_PROCESS_ENERGY_VALUES,
    ProcInfoClass_PROCESS_POWER_THROTTLING_STATE,
    ProcInfoClass_PROCESS_ACTIVITY_THROTTLE_POLICY,
    ProcInfoClass_PROCESS_WIN32K_SYSCALL_FILTER_INFORMATION,
    ProcInfoClass_PROCESS_DISABLE_SYSTEM_ALLOWED_CPU_SETS,
    ProcInfoClass_PROCESS_WAKE_INFORMATION,
    ProcInfoClass_PROCESS_ENERGY_TRACKING_STATE,
    ProcInfoClass_PROCESS_MANAGE_WRITES_TO_EXECUTABLE_MEMORY,
    ProcInfoClass_PROCESS_CAPTURE_TRUSTLET_LIVE_DUMP,
    ProcInfoClass_PROCESS_TELEMETRY_COVERAGE,
    ProcInfoClass_PROCESS_ENCLAVE_INFORMATION,
    ProcInfoClass_PROCESS_ENABLE_READ_WRITE_VM_LOGGING,
    ProcInfoClass_PROCESS_UPTIME_INFORMATION,
    ProcInfoClass_PROCESS_IMAGE_SECTION,
    ProcInfoClass_PROCESS_DEBUG_AUTH_INFORMATION,
    ProcInfoClass_PROCESS_SYSTEM_RESOURCE_MANAGEMENT,
    ProcInfoClass_PROCESS_SEQUENCE_NUMBER,
    ProcInfoClass_PROCESS_LOADER_DETOUR,
    ProcInfoClass_PROCESS_SECURITY_DOMAIN_INFORMATION,
    ProcInfoClass_PROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION,
    ProcInfoClass_PROCESS_ENABLE_LOGGING,
    ProcInfoClass_PROCESS_LEAP_SECOND_INFORMATION,
    ProcInfoClass_PROCESS_FIBER_SHADOW_STACK_ALLOCATION,
    ProcInfoClass_PROCESS_FREE_FIBER_SHADOW_STACK_ALLOCATION,
    ProcInfoClass_PROCESS_ALT_SYSTEM_CALL_INFORMATION,
    ProcInfoClass_PROCESS_DYNAMIC_EH_CONTINUATION_TARGETS,
    ProcInfoClass_PROCESS_DYNAMIC_ENFORCED_CET_COMPATIBLE_RANGES,
    ProcInfoClass_PROCESS_CREATE_STATE_CHANGE,
    ProcInfoClass_PROCESS_APPLY_STATE_CHANGE,
    ProcInfoClass_PROCESS_ENABLE_OPTIONAL_X_STATE_FEATURES,
    ProcInfoClass_PROCESS_ALT_PREFETCH_PARAM,
    ProcInfoClass_PROCESS_ASSIGN_CPU_PARTITIONS,
    ProcInfoClass_PROCESS_PRIORITY_CLASS_EX,
    ProcInfoClass_PROCESS_MEMBERSHIP_INFORMATION,
    ProcInfoClass_PROCESS_EFFECTIVE_IO_PRIORITY,
    ProcInfoClass_PROCESS_EFFECTIVE_PAGE_PRIORITY,
    ProcInfoClass_PROCESS_SCHEDULER_SHARED_DATA,
    ProcInfoClass_PROCESS_SLIST_ROLLBACK_INFORMATION,
    ProcInfoClass_PROCESS_NETWORK_IO_COUNTERS,
    ProcInfoClass_PROCESS_FIND_FIRST_THREAD_BY_TEB_VALUE,
    ProcInfoClass_PROCESS_ENCLAVE_ADDRESS_SPACE_RESTRICTION,
    ProcInfoClass_PROCESS_AVAILABLE_CPUS
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntquerysysteminformation
 */
DLLIMPORT NtStatus NTAPI NtQuerySystemInformation(
	SysInfoClass sys_info_class,
	void*        sys_info,
	u32          sys_info_length,
	u32*         return_length
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntqueryinformationprocess
 */

DLLIMPORT NtStatus NTAPI NtQueryInformationProcess(
	Handle        process_handle,
	ProcInfoClass proc_info_class,
	void*         proc_info,
	u32           proc_info_length,
	u32*          return_length
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtimeasfiletime
 */
DLLIMPORT void WINAPI GetSystemTimeAsFileTime(
	u64* system_time_as_file_time
);

/**
 * https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/ntexapi/system_basic_information.htm
 * https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntquerysysteminformation#systembasicinformation
 *
 * NtQuerySystemInformation(
 *     SysInfoClass_SYSTEM_BASIC_INFORMATION,
 *     SystemBasicInformation* basic_info,
 *     ...
 * );
 */
struct SystemBasicInformation {
	u32  obsolete;
	u32  timer_resolution;
	u32  page_size;
	u32  number_of_physical_pages;
	u32  lowest_physical_page_number;
	u32  highest_physical_page_number;
	u32  allocation_granularity;
	uptr minimum_user_mode_address;
	uptr maximum_user_mode_address;
	u64  active_processor_affinity_mask;
	u8   number_of_processors;
};

/**
 * https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/processor_performance.htm
 * https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntquerysysteminformation#system_processor_performance_information
 *
 * NtQuerySystemInformation(
 *     SysInfoClass_SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION,
 *     SystemProcessorPerformanceInformation processor_perf_stats_buffer[PROCESSOR_COUNT],
 *     ...
 * );
 *
 * All time values are counts of some interval given in SystemBasicInformation.timer_resolution.
 * PROCESSOR_COUNT may likewise be retrieved through SystemBasicInformation.number_of_processors.
 * Time is split between user and kernel, so, e.g., idle time is kernel time.
 */
struct SystemProcessorPerformanceInformation {
	u64 idle_time;
	u64 kernel_time;
	u64 user_time;
	u64 dpc_time;
	u64 interrupt_time;
	u32 interrupt_count;
};

/**
 * https://ntdoc.m417z.com/unicode_string
 */
struct UnicodeString {
	u16      length;
	u16      maximum_length;
	wchar_t* buffer;
};

/**
 * https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/pagefile.htm
 *
 * NtQuerySystemInformation(
 *     SysInfoClass_SYSTEM_PAGE_FILE_INFORMATION,
 *     SystemPageFileInformation* page_file_info,
 *     ...
 * );
 */
struct SystemPageFileInformation {
	u32           next_entry_offset;
	u32           total_size;
	u32           total_in_use;
	u32           peak_usage;
	UnicodeString page_file_name;
};

/**
 * https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/pagefile_ex.htm
 *
 * NtQuerySystemInformation(
 *     SysInfoClass_SYSTEM_PAGE_FILE_INFORMATION_EX,
 *     SystemPageFileInformationEx* page_file_info,
 *     ...
 * );
 */
struct SystemPageFileInformationEx {
	SystemPageFileInformation info;
	u32                       minimum_size;
	u32                       maximum_size;
};

/**
 * https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/performance.htm
 * https://ntdoc.m417z.com/system_performance_information
 *
 * NtQuerySystemInformation(
 *     SysInfoClass_SYSTEM_PERFORMANCE_INFORMATION,
 *     SystemPerformanceInformation* sys_perf_info,
 *     ...
 * );
 */
struct SystemPerformanceInformation {
	u64 idle_process_time;
	u64 io_read_transfer_count;
	u64 io_write_transfer_count;
	u64 io_other_transfer_count;
	u32 io_read_operation_count;
	u32 io_write_operation_count;
	u32 io_other_operation_count;
	u32 available_pages;
	u32 committed_pages;
	u32 commit_limit;
	u32 peak_commitment;
	u32 page_fault_count;
	u32 copy_on_write_count;
	u32 transition_count;
	u32 cache_transition_count;
	u32 demand_zero_count;
	u32 page_read_count;
	u32 page_read_io_count;
	u32 cache_read_count;
	u32 cache_io_count;
	u32 dirty_pages_write_count;
	u32 dirty_write_io_count;
	u32 mapped_pages_write_count;
	u32 mapped_write_io_count;
	u32 paged_pool_pages;
	u32 non_paged_pool_pages;
	u32 paged_pool_allocs;
	u32 paged_pool_frees;
	u32 non_paged_pool_allocs;
	u32 non_paged_pool_frees;
	u32 free_system_ptes;
	u32 resident_system_code_page;
	u32 total_system_driver_pages;
	u32 total_system_code_pages;
	u32 non_paged_pool_lookaside_hits;
	u32 paged_pool_lookaside_hits;
	u32 available_paged_pool_pages;
	u32 resident_system_cache_page;
	u32 resident_paged_pool_page;
	u32 resident_system_driver_page;
	u32 cc_fast_read_no_wait;
	u32 cc_fast_read_wait;
	u32 cc_fast_read_resource_miss;
	u32 cc_fast_read_not_possible;
	u32 cc_fast_mdl_read_no_wait;
	u32 cc_fast_mdl_read_wait;
	u32 cc_fast_mdl_read_resource_miss;
	u32 cc_fast_mdl_read_not_possible;
	u32 cc_map_data_no_wait;
	u32 cc_map_data_wait;
	u32 cc_map_data_no_wait_miss;
	u32 cc_map_data_wait_miss;
	u32 cc_pin_mapped_data_count;
	u32 cc_pin_read_no_wait;
	u32 cc_pin_read_wait;
	u32 cc_pin_read_no_wait_miss;
	u32 cc_pin_read_wait_miss;
	u32 cc_copy_read_no_wait;
	u32 cc_copy_read_wait;
	u32 cc_copy_read_no_wait_miss;
	u32 cc_copy_read_wait_miss;
	u32 cc_mdl_read_no_wait;
	u32 cc_mdl_read_wait;
	u32 cc_mdl_read_no_wait_miss;
	u32 cc_mdl_read_wait_miss;
	u32 cc_read_ahead_ios;
	u32 cc_lazy_write_ios;
	u32 cc_lazy_write_pages;
	u32 cc_data_flushes;
	u32 cc_data_pages;
	u32 context_switches;
	u32 first_level_tb_fills;
	u32 second_level_tb_fills;
	u32 system_calls;
	u64 cc_total_dirty_pages;
	u64 cc_dirty_page_threshold;
	s64 resident_available_pages;
	u64 shared_committed_pages;
};

/**
 * https://ntdoc.m417z.com/client_id
 */
struct ClientId {
	uptr unique_process;
	uptr unique_thread;
};

/**
 * https://ntdoc.m417z.com/system_thread_information
 */
struct SystemThreadInformation {
	u64      kernel_time;
	u64      user_time;
	u64      create_time;
	u32      wait_time;
	void*    start_address;
	ClientId client_id;
	s32      priority;
	s32      base_priority;
	u32      context_switches;
	u32      thread_state;
	u32      wait_reason;
};

/**
 * https://ntdoc.m417z.com/system_process_information
 *
 * NtQuerySystemInformation(
 *     SysInfoClass_SYSTEM_PROCESS_INFORMATION,
 *     SystemProcessInformation* buffer,
 *     ...
 * );
 *
 * This one's tricky because of variable sizing due to threads and ever-changing data. Call first with a NULL buffer
 * and loop with a growing buffer until the status returned is not NT_STATUS_INFO_LENGTH_MISMATCH. The return_size
 * will indicate the size the new buffer should be. Then, use as a linked list with `next_entry_offset` to index the
 * next process in the buffer.
 */
struct SystemProcessInformation {
	u32                     next_entry_offset;
	u32                     number_of_threads;
	u64                     working_set_private_size;
	u32                     hard_fault_count;
	u32                     number_of_threads_high_watermark;
	u64                     cycle_time;
	u64                     create_time;
	u64                     user_time;
	u64                     kernel_time;
	UnicodeString           image_name;
	s32                     base_priority;
	uptr                    unique_process_id;
	uptr                    inherited_from_unique_process_id;
	u32                     handle_count;
	u32                     session_id;
	uptr                    unique_process_key;
	u64                     peak_virtual_size;
	u64                     virtual_size;
	u32                     page_fault_count;
	u64                     peak_working_set_size;
	u64                     working_set_size;
	u64                     quota_peak_paged_pool_usage;
	u64                     quota_paged_pool_usage;
	u64                     quota_peak_non_paged_pool_usage;
	u64                     quota_non_paged_pool_usage;
	u64                     pagefile_usage;
	u64                     peak_pagefile_usage;
	u64                     private_page_count;
	u64                     read_operation_count;
	u64                     write_operation_count;
	u64                     other_operation_count;
	u64                     read_transfer_count;
	u64                     write_transfer_count;
	u64                     other_transfer_count;
	SystemThreadInformation threads[];
};

/**
 * ===============
 * WINDOWS.H ET AL
 * ===============
 */

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess
 */
DLLIMPORT void WINAPI ExitProcess(
	u32 exit_code
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/wtypesbase/ns-wtypesbase-security_attributes
 */
struct SecurityAttributes {
	u32   length;
	void* security_descriptor;
	b32   inherit_handle;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread
 */
DLLIMPORT Handle WINAPI CreateThread(
	SecurityAttributes* thread_attributes,
	u64                 stack_size,
	u32   (WINCALLBACK* start_address)(void* param),
	void*               parameter,
	u32                 creation_flags,
	u32*                thread_id
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleep
 */
DLLIMPORT void WINAPI Sleep(
	u32 milliseconds
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreaddescription
 */
DLLIMPORT s64 WINAPI SetThreadDescription(
	Handle   thread,
	wchar_t* thread_description
);

#define PAGE_NOACCESS                   0x00000001
#define PAGE_READONLY                   0x00000002
#define PAGE_READWRITE                  0x00000004
#define PAGE_WRITECOPY                  0x00000008
#define PAGE_EXECUTE                    0x00000010
#define PAGE_EXECUTE_READ               0x00000020
#define PAGE_EXECUTE_READWRITE          0x00000040
#define PAGE_EXECUTE_WRITECOPY          0x00000080
#define PAGE_GUARD                      0x00000100
#define PAGE_NOCACHE                    0x00000200
#define PAGE_WRITECOMBINE               0x00000400
#define PAGE_GRAPHICS_NOACCESS          0x00000800
#define PAGE_GRAPHICS_READONLY          0x00001000
#define PAGE_GRAPHICS_READWRITE         0x00002000
#define PAGE_GRAPHICS_EXECUTE           0x00004000
#define PAGE_GRAPHICS_EXECUTE_READ      0x00008000
#define PAGE_GRAPHICS_EXECUTE_READWRITE 0x00010000
#define PAGE_GRAPHICS_COHERENT          0x00020000
#define PAGE_GRAPHICS_NOCACHE           0x00040000
#define PAGE_ENCLAVE_THREAD_CONTROL     0x80000000
#define PAGE_REVERT_TO_FILE_MAP         0x80000000
#define PAGE_TARGETS_NO_UPDATE          0x40000000
#define PAGE_TARGETS_INVALID            0x40000000
#define PAGE_ENCLAVE_UNVALIDATED        0x20000000
#define PAGE_ENCLAVE_MASK               0x10000000
#define PAGE_ENCLAVE_DECOMMIT           (PAGE_ENCLAVE_MASK | 0)
#define PAGE_ENCLAVE_SS_FIRST           (PAGE_ENCLAVE_MASK | 1)
#define PAGE_ENCLAVE_SS_REST            (PAGE_ENCLAVE_MASK | 2)

#define MEM_COMMIT                      0x00001000
#define MEM_RESERVE                     0x00002000
#define MEM_REPLACE_PLACEHOLDER         0x00004000
#define MEM_RESERVE_PLACEHOLDER         0x00040000
#define MEM_RESET                       0x00080000
#define MEM_TOP_DOWN                    0x00100000
#define MEM_WRITE_WATCH                 0x00200000
#define MEM_PHYSICAL                    0x00400000
#define MEM_ROTATE                      0x00800000
#define MEM_DIFFERENT_IMAGE_BASE_OK     0x00800000
#define MEM_RESET_UNDO                  0x01000000
#define MEM_LARGE_PAGES                 0x20000000
#define MEM_4MB_PAGES                   0x80000000
#define MEM_64K_PAGES                   (MEM_LARGE_PAGES | MEM_PHYSICAL)
#define MEM_UNMAP_WITH_TRANSIENT_BOOST  0x00000001
#define MEM_COALESCE_PLACEHOLDERS       0x00000001
#define MEM_PRESERVE_PLACEHOLDER        0x00000002
#define MEM_DECOMMIT                    0x00004000
#define MEM_RELEASE                     0x00008000
#define MEM_FREE                        0x00010000

/**
 *https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc
 */
DLLIMPORT void* WINAPI VirtualAlloc(
	void* address,
	u64   size,
	u32   allocation_type,
	u32   protect
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualfree
 */
DLLIMPORT b32 WINAPI VirtualFree(
	void* address,
	u64   size,
	u32   free_type
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-outputdebugstringa
 */
DLLIMPORT void WINAPI OutputDebugStringA(
	CString output_string
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
 */
DLLIMPORT s32 WINAPI WideCharToMultiByte(
	u32      code_page,
	u32      flags,
	wchar_t* wide_char_str,
	s32      wide_char,
	u8*      multi_byte_str,
	s32      multi_byte,
	u8*      default_char,
	b32*     used_default_char
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/windef/ns-windef-point
 */
struct Point {
	s32 x;
	s32 y;
};

#define MONITOR_DEFAULTTONULL       0x00000000
#define MONITOR_DEFAULTTOPRIMARY    0x00000001
#define MONITOR_DEFAULTTONEAREST    0x00000002

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-monitorfrompoint
 */
DLLIMPORT HMonitor MonitorFromPoint(
	Point point,
	u32   flags
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nc-winuser-wndproc
 */
typedef s64 (WINCALLBACK* WndProc)(HWnd window, u32 msg, u64 w_param, s64 l_param);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexw
 */
struct WndClassExW {
	u32             size;
	u32             style;
	WndProc         wnd_proc;
	s32             cls_extra;
	s32             wnd_extra;
	HInstance       instance;
	HIcon           icon;
	HCursor         cursor;
	HBrush          background;
	const wchar_t*  menu_name;
	const wchar_t*  class_name;
	HIcon           icon_sm;
};

/**
 * Class Styles
 */

#define CS_VREDRAW         0x00000001
#define CS_HREDRAW         0x00000002
#define CS_DBLCLKS         0x00000008
#define CS_OWNDC           0x00000020
#define CS_CLASSDC         0x00000040
#define CS_PARENTDC        0x00000080
#define CS_NOCLOSE         0x00000200
#define CS_SAVEBITS        0x00000800
#define CS_BYTEALIGNCLIENT 0x00001000
#define CS_BYTEALIGNWINDOW 0x00002000
#define CS_GLOBALCLASS     0x00004000
#define CS_IME             0x00010000
#define CS_DROPSHADOW      0x00020000

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexw
 */
DLLIMPORT u16 WINAPI RegisterClassExW(WndClassExW* wnd_class);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
 */
DLLIMPORT HWnd WINAPI CreateWindowExW(
	u32            ex_style,
	const wchar_t* class_name,
	const wchar_t* window_name,
	u32            style,
	s32            x,
	s32            y,
	s32            width,
	s32            height,
	HWnd           wnd_parent,
	HMenu          menu,
	HInstance      instance,
	void*          param
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-destroywindow
 */
DLLIMPORT b32 WINAPI DestroyWindow(
  HWnd window
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-postquitmessage
 */
DLLIMPORT void WINAPI PostQuitMessage(
	int exit_code
);

/**
 * Window Styles
 */

#define WS_OVERLAPPED       0x00000000L
#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L
#define WS_MINIMIZE         0x20000000L
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L
#define WS_MAXIMIZE         0x01000000L
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_CAPTION          (WS_BORDER | WS_DLGFRAME)
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L
#define WS_MINIMIZEBOX      0x00020000L
#define WS_MAXIMIZEBOX      0x00010000L
#define WS_TILED            WS_OVERLAPPED
#define WS_ICONIC           WS_MINIMIZE
#define WS_SIZEBOX          WS_THICKFRAME
#define WS_OVERLAPPEDWINDOW ( \
	WS_OVERLAPPED  |          \
	WS_CAPTION     |          \
	WS_SYSMENU     |          \
	WS_THICKFRAME  |          \
	WS_MINIMIZEBOX |          \
	WS_MAXIMIZEBOX            \
)
#define WS_TILEDWINDOW      WS_OVERLAPPEDWINDOW
#define WS_POPUPWINDOW      ( \
	WS_POPUP  |               \
	WS_BORDER |               \
	WS_SYSMENU                \
)
#define WS_CHILDWINDOW          (WS_CHILD)
#define WS_EX_DLGMODALFRAME     0x00000001L
#define WS_EX_NOPARENTNOTIFY    0x00000004L
#define WS_EX_TOPMOST           0x00000008L
#define WS_EX_ACCEPTFILES       0x00000010L
#define WS_EX_TRANSPARENT       0x00000020L
#define WS_EX_MDICHILD          0x00000040L
#define WS_EX_TOOLWINDOW        0x00000080L
#define WS_EX_WINDOWEDGE        0x00000100L
#define WS_EX_CLIENTEDGE        0x00000200L
#define WS_EX_CONTEXTHELP       0x00000400L
#define WS_EX_RIGHT             0x00001000L
#define WS_EX_LEFT              0x00000000L
#define WS_EX_RTLREADING        0x00002000L
#define WS_EX_LTRREADING        0x00000000L
#define WS_EX_LEFTSCROLLBAR     0x00004000L
#define WS_EX_RIGHTSCROLLBAR    0x00000000L
#define WS_EX_CONTROLPARENT     0x00010000L
#define WS_EX_STATICEDGE        0x00020000L
#define WS_EX_APPWINDOW         0x00040000L
#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
#define WS_EX_PALETTEWINDOW     (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)
#define WS_EX_LAYERED           0x00080000
#define WS_EX_NOINHERITLAYOUT   0x00100000L // Disable inheritence of mirroring by children
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#define WS_EX_LAYOUTRTL         0x00400000L // Right to left mirroring
#define WS_EX_COMPOSITED        0x02000000L
#define WS_EX_NOACTIVATE        0x08000000L

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
 */
enum ShowWindowCommand {
	ShowWindowCommand_HIDE            = 0,
	ShowWindowCommand_SHOWNORMAL      = 1,
	ShowWindowCommand_NORMAL          = 1,
	ShowWindowCommand_SHOWMINIMIZED   = 2,
	ShowWindowCommand_SHOWMAXIMIZED   = 3,
	ShowWindowCommand_MAXIMIZE        = 3,
	ShowWindowCommand_SHOWNOACTIVATE  = 4,
	ShowWindowCommand_SHOW            = 5,
	ShowWindowCommand_MINIMIZE        = 6,
	ShowWindowCommand_SHOWMINNOACTIVE = 7,
	ShowWindowCommand_SHOWNA          = 8,
	ShowWindowCommand_RESTORE         = 9,
	ShowWindowCommand_SHOWDEFAULT     = 10,
	ShowWindowCommand_FORCEMINIMIZE   = 11,
	ShowWindowCommand_MAX             = 11,
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
 */
DLLIMPORT b32 WINAPI ShowWindow(
	HWnd              window,
	ShowWindowCommand cmd_show
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-updatewindow
 */
DLLIMPORT b32 WINAPI UpdateWindow(
	HWnd window
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-msg
 */
struct Msg {
  HWnd  wnd;
  u32   message;
  u64   w_param;
  s64   l_param;
  u32   time;
  Point pt;
  u32   l_private;
};

/*
 * PeekMessage remove options
 */

#define PM_NOREMOVE         0x0000
#define PM_REMOVE           0x0001
#define PM_NOYIELD          0x0002

/**
 * Window Messages
 */

#define WM_NULL                           0x0000
#define WM_CREATE                         0x0001
#define WM_DESTROY                        0x0002
#define WM_MOVE                           0x0003
#define WM_SIZE                           0x0005
#define WM_ACTIVATE                       0x0006
#define WM_SETFOCUS                       0x0007
#define WM_KILLFOCUS                      0x0008
#define WM_ENABLE                         0x000A
#define WM_SETREDRAW                      0x000B
#define WM_SETTEXT                        0x000C
#define WM_GETTEXT                        0x000D
#define WM_GETTEXTLENGTH                  0x000E
#define WM_PAINT                          0x000F
#define WM_CLOSE                          0x0010
#define WM_QUERYENDSESSION                0x0011
#define WM_QUERYOPEN                      0x0013
#define WM_ENDSESSION                     0x0016
#define WM_QUIT                           0x0012
#define WM_ERASEBKGND                     0x0014
#define WM_SYSCOLORCHANGE                 0x0015
#define WM_SHOWWINDOW                     0x0018
#define WM_WININICHANGE                   0x001A
#define WM_SETTINGCHANGE                  WM_WININICHANGE
#define WM_DEVMODECHANGE                  0x001B
#define WM_ACTIVATEAPP                    0x001C
#define WM_FONTCHANGE                     0x001D
#define WM_TIMECHANGE                     0x001E
#define WM_CANCELMODE                     0x001F
#define WM_SETCURSOR                      0x0020
#define WM_MOUSEACTIVATE                  0x0021
#define WM_CHILDACTIVATE                  0x0022
#define WM_QUEUESYNC                      0x0023
#define WM_GETMINMAXINFO                  0x0024
#define WM_PAINTICON                      0x0026
#define WM_ICONERASEBKGND                 0x0027
#define WM_NEXTDLGCTL                     0x0028
#define WM_SPOOLERSTATUS                  0x002A
#define WM_DRAWITEM                       0x002B
#define WM_MEASUREITEM                    0x002C
#define WM_DELETEITEM                     0x002D
#define WM_VKEYTOITEM                     0x002E
#define WM_CHARTOITEM                     0x002F
#define WM_SETFONT                        0x0030
#define WM_GETFONT                        0x0031
#define WM_SETHOTKEY                      0x0032
#define WM_GETHOTKEY                      0x0033
#define WM_QUERYDRAGICON                  0x0037
#define WM_COMPAREITEM                    0x0039
#define WM_GETOBJECT                      0x003D
#define WM_COMPACTING                     0x0041
#define WM_COMMNOTIFY                     0x0044  /* no longer suported */
#define WM_WINDOWPOSCHANGING              0x0046
#define WM_WINDOWPOSCHANGED               0x0047
#define WM_POWER                          0x0048
#define WM_COPYDATA                       0x004A
#define WM_CANCELJOURNAL                  0x004B
#define WM_NOTIFY                         0x004E
#define WM_INPUTLANGCHANGEREQUEST         0x0050
#define WM_INPUTLANGCHANGE                0x0051
#define WM_TCARD                          0x0052
#define WM_HELP                           0x0053
#define WM_USERCHANGED                    0x0054
#define WM_NOTIFYFORMAT                   0x0055
#define WM_CONTEXTMENU                    0x007B
#define WM_STYLECHANGING                  0x007C
#define WM_STYLECHANGED                   0x007D
#define WM_DISPLAYCHANGE                  0x007E
#define WM_GETICON                        0x007F
#define WM_SETICON                        0x0080
#define WM_NCCREATE                       0x0081
#define WM_NCDESTROY                      0x0082
#define WM_NCCALCSIZE                     0x0083
#define WM_NCHITTEST                      0x0084
#define WM_NCPAINT                        0x0085
#define WM_NCACTIVATE                     0x0086
#define WM_GETDLGCODE                     0x0087
#define WM_SYNCPAINT                      0x0088
#define WM_NCMOUSEMOVE                    0x00A0
#define WM_NCLBUTTONDOWN                  0x00A1
#define WM_NCLBUTTONUP                    0x00A2
#define WM_NCLBUTTONDBLCLK                0x00A3
#define WM_NCRBUTTONDOWN                  0x00A4
#define WM_NCRBUTTONUP                    0x00A5
#define WM_NCRBUTTONDBLCLK                0x00A6
#define WM_NCMBUTTONDOWN                  0x00A7
#define WM_NCMBUTTONUP                    0x00A8
#define WM_NCMBUTTONDBLCLK                0x00A9
#define WM_NCXBUTTONDOWN                  0x00AB
#define WM_NCXBUTTONUP                    0x00AC
#define WM_NCXBUTTONDBLCLK                0x00AD
#define WM_INPUT_DEVICE_CHANGE            0x00FE
#define WM_INPUT                          0x00FF
#define WM_KEYFIRST                       0x0100
#define WM_KEYDOWN                        0x0100
#define WM_KEYUP                          0x0101
#define WM_CHAR                           0x0102
#define WM_DEADCHAR                       0x0103
#define WM_SYSKEYDOWN                     0x0104
#define WM_SYSKEYUP                       0x0105
#define WM_SYSCHAR                        0x0106
#define WM_SYSDEADCHAR                    0x0107
#define WM_UNICHAR                        0x0109
// TODO(hhammon) Figure out what the hell happened here
// #define WM_KEYLAST                        0x0109
// #define WM_KEYLAST                        0x0108
#define WM_IME_STARTCOMPOSITION           0x010D
#define WM_IME_ENDCOMPOSITION             0x010E
#define WM_IME_COMPOSITION                0x010F
#define WM_IME_KEYLAST                    0x010F
#define WM_INITDIALOG                     0x0110
#define WM_COMMAND                        0x0111
#define WM_SYSCOMMAND                     0x0112
#define WM_TIMER                          0x0113
#define WM_HSCROLL                        0x0114
#define WM_VSCROLL                        0x0115
#define WM_INITMENU                       0x0116
#define WM_INITMENUPOPUP                  0x0117
#define WM_GESTURE                        0x0119
#define WM_GESTURENOTIFY                  0x011A
#define WM_MENUSELECT                     0x011F
#define WM_MENUCHAR                       0x0120
#define WM_ENTERIDLE                      0x0121
#define WM_MENURBUTTONUP                  0x0122
#define WM_MENUDRAG                       0x0123
#define WM_MENUGETOBJECT                  0x0124
#define WM_UNINITMENUPOPUP                0x0125
#define WM_MENUCOMMAND                    0x0126
#define WM_CHANGEUISTATE                  0x0127
#define WM_UPDATEUISTATE                  0x0128
#define WM_QUERYUISTATE                   0x0129
#define WM_CTLCOLORMSGBOX                 0x0132
#define WM_CTLCOLOREDIT                   0x0133
#define WM_CTLCOLORLISTBOX                0x0134
#define WM_CTLCOLORBTN                    0x0135
#define WM_CTLCOLORDLG                    0x0136
#define WM_CTLCOLORSCROLLBAR              0x0137
#define WM_CTLCOLORSTATIC                 0x0138
#define WM_MOUSEFIRST                     0x0200
#define WM_MOUSEMOVE                      0x0200
#define WM_LBUTTONDOWN                    0x0201
#define WM_LBUTTONUP                      0x0202
#define WM_LBUTTONDBLCLK                  0x0203
#define WM_RBUTTONDOWN                    0x0204
#define WM_RBUTTONUP                      0x0205
#define WM_RBUTTONDBLCLK                  0x0206
#define WM_MBUTTONDOWN                    0x0207
#define WM_MBUTTONUP                      0x0208
#define WM_MBUTTONDBLCLK                  0x0209
#define WM_MOUSEWHEEL                     0x020A
#define WM_XBUTTONDOWN                    0x020B
#define WM_XBUTTONUP                      0x020C
#define WM_XBUTTONDBLCLK                  0x020D
#define WM_MOUSEHWHEEL                    0x020E
// TODO(hhammon) Figure out what the hell happened here
// #define WM_MOUSELAST                      0x020E
// #define WM_MOUSELAST                      0x020D
// #define WM_MOUSELAST                      0x020A
// #define WM_MOUSELAST                      0x0209
#define WM_PARENTNOTIFY                   0x0210
#define WM_ENTERMENULOOP                  0x0211
#define WM_EXITMENULOOP                   0x0212
#define WM_NEXTMENU                       0x0213
#define WM_SIZING                         0x0214
#define WM_CAPTURECHANGED                 0x0215
#define WM_MOVING                         0x0216
#define WM_POWERBROADCAST                 0x0218
#define WM_DEVICECHANGE                   0x0219
#define WM_MDICREATE                      0x0220
#define WM_MDIDESTROY                     0x0221
#define WM_MDIACTIVATE                    0x0222
#define WM_MDIRESTORE                     0x0223
#define WM_MDINEXT                        0x0224
#define WM_MDIMAXIMIZE                    0x0225
#define WM_MDITILE                        0x0226
#define WM_MDICASCADE                     0x0227
#define WM_MDIICONARRANGE                 0x0228
#define WM_MDIGETACTIVE                   0x0229
#define WM_MDISETMENU                     0x0230
#define WM_ENTERSIZEMOVE                  0x0231
#define WM_EXITSIZEMOVE                   0x0232
#define WM_DROPFILES                      0x0233
#define WM_MDIREFRESHMENU                 0x0234
#define WM_POINTERDEVICECHANGE            0x238
#define WM_POINTERDEVICEINRANGE           0x239
#define WM_POINTERDEVICEOUTOFRANGE        0x23A
#define WM_TOUCH                          0x0240
#define WM_NCPOINTERUPDATE                0x0241
#define WM_NCPOINTERDOWN                  0x0242
#define WM_NCPOINTERUP                    0x0243
#define WM_POINTERUPDATE                  0x0245
#define WM_POINTERDOWN                    0x0246
#define WM_POINTERUP                      0x0247
#define WM_POINTERENTER                   0x0249
#define WM_POINTERLEAVE                   0x024A
#define WM_POINTERACTIVATE                0x024B
#define WM_POINTERCAPTURECHANGED          0x024C
#define WM_TOUCHHITTESTING                0x024D
#define WM_POINTERWHEEL                   0x024E
#define WM_POINTERHWHEEL                  0x024F
#define WM_POINTERROUTEDTO                0x0251
#define WM_POINTERROUTEDAWAY              0x0252
#define WM_POINTERROUTEDRELEASED          0x0253
#define WM_IME_SETCONTEXT                 0x0281
#define WM_IME_NOTIFY                     0x0282
#define WM_IME_CONTROL                    0x0283
#define WM_IME_COMPOSITIONFULL            0x0284
#define WM_IME_SELECT                     0x0285
#define WM_IME_CHAR                       0x0286
#define WM_IME_REQUEST                    0x0288
#define WM_IME_KEYDOWN                    0x0290
#define WM_IME_KEYUP                      0x0291
#define WM_MOUSEHOVER                     0x02A1
#define WM_MOUSELEAVE                     0x02A3
#define WM_NCMOUSEHOVER                   0x02A0
#define WM_NCMOUSELEAVE                   0x02A2
#define WM_WTSSESSION_CHANGE              0x02B1
#define WM_TABLET_FIRST                   0x02c0
#define WM_TABLET_LAST                    0x02df
#define WM_DPICHANGED                     0x02E0
#define WM_DPICHANGED_BEFOREPARENT        0x02E2
#define WM_DPICHANGED_AFTERPARENT         0x02E3
#define WM_GETDPISCALEDSIZE               0x02E4
#define WM_CUT                            0x0300
#define WM_COPY                           0x0301
#define WM_PASTE                          0x0302
#define WM_CLEAR                          0x0303
#define WM_UNDO                           0x0304
#define WM_RENDERFORMAT                   0x0305
#define WM_RENDERALLFORMATS               0x0306
#define WM_DESTROYCLIPBOARD               0x0307
#define WM_DRAWCLIPBOARD                  0x0308
#define WM_PAINTCLIPBOARD                 0x0309
#define WM_VSCROLLCLIPBOARD               0x030A
#define WM_SIZECLIPBOARD                  0x030B
#define WM_ASKCBFORMATNAME                0x030C
#define WM_CHANGECBCHAIN                  0x030D
#define WM_HSCROLLCLIPBOARD               0x030E
#define WM_QUERYNEWPALETTE                0x030F
#define WM_PALETTEISCHANGING              0x0310
#define WM_PALETTECHANGED                 0x0311
#define WM_HOTKEY                         0x0312
#define WM_PRINT                          0x0317
#define WM_PRINTCLIENT                    0x0318
#define WM_APPCOMMAND                     0x0319
#define WM_THEMECHANGED                   0x031A
#define WM_CLIPBOARDUPDATE                0x031D
#define WM_DWMCOMPOSITIONCHANGED          0x031E
#define WM_DWMNCRENDERINGCHANGED          0x031F
#define WM_DWMCOLORIZATIONCOLORCHANGED    0x0320
#define WM_DWMWINDOWMAXIMIZEDCHANGE       0x0321
#define WM_DWMSENDICONICTHUMBNAIL         0x0323
#define WM_DWMSENDICONICLIVEPREVIEWBITMAP 0x0326
#define WM_GETTITLEBARINFOEX              0x033F
#define WM_HANDHELDFIRST                  0x0358
#define WM_HANDHELDLAST                   0x035F
#define WM_AFXFIRST                       0x0360
#define WM_AFXLAST                        0x037F
#define WM_PENWINFIRST                    0x0380
#define WM_PENWINLAST                     0x038F
#define WM_APP                            0x8000
#define WM_USER                           0x0400
#define WM_INTERCEPTED_WINDOW_ACTION      0x0346
#define WM_TOOLTIPDISMISS                 0x0345
#define WM_CLOAKED_STATE_CHANGED          0x0347

/*
 * WM_SIZE message wParam values
 */
#define SIZE_RESTORED  0
#define SIZE_MINIMIZED 1
#define SIZE_MAXIMIZED 2
#define SIZE_MAXSHOW   3
#define SIZE_MAXHIDE   4

/*
 * System Menu Command Values
 */
#define SC_SIZE         0xF000
#define SC_MOVE         0xF010
#define SC_MINIMIZE     0xF020
#define SC_MAXIMIZE     0xF030
#define SC_NEXTWINDOW   0xF040
#define SC_PREVWINDOW   0xF050
#define SC_CLOSE        0xF060
#define SC_VSCROLL      0xF070
#define SC_HSCROLL      0xF080
#define SC_MOUSEMENU    0xF090
#define SC_KEYMENU      0xF100
#define SC_ARRANGE      0xF110
#define SC_RESTORE      0xF120
#define SC_TASKLIST     0xF130
#define SC_SCREENSAVE   0xF140
#define SC_HOTKEY       0xF150
#define SC_DEFAULT      0xF160
#define SC_MONITORPOWER 0xF170
#define SC_CONTEXTHELP  0xF180
#define SC_SEPARATOR    0xF00F

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-peekmessagea
 */
DLLIMPORT b32 WINAPI PeekMessageW(
	Msg* msg,
	HWnd wnd,
	u32  msg_filter_min,
	u32  msg_filter_max,
	u32  remove_msg
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-translatemessage
 */
DLLIMPORT b32 WINAPI TranslateMessage(
	Msg* msg
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-dispatchmessagew
 */
DLLIMPORT s64 WINAPI DispatchMessageW(
	Msg* msg
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defwindowprocw
 */
DLLIMPORT s64 WINAPI DefWindowProcW(
	HWnd wnd,
	u32  msg,
	u64  w_param,
	s64  l_param
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/gdi/colorref
 */
typedef u32 ColorRef;

#define win32_rgb(r, g, b) ((ColorRef)( \
	(((u32)r & 0xff) <<  0) |           \
	(((u32)g & 0xff) <<  8) |           \
	(((u32)b & 0xff) << 16)             \
))

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createsolidbrush
 */
DLLIMPORT HBrush WINAPI CreateSolidBrush(
	ColorRef color
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgicommon/ns-dxgicommon-dxgi_rational
 */
struct DxgiRational {
	u32 numerator;
	u32 denominator;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
 */
enum DxgiFormat : s32 {
	DxgiFormat_UNKNOWN                                 = 0,
	DxgiFormat_R32G32B32A32_TYPELESS                   = 1,
	DxgiFormat_R32G32B32A32_FLOAT                      = 2,
	DxgiFormat_R32G32B32A32_UINT                       = 3,
	DxgiFormat_R32G32B32A32_SINT                       = 4,
	DxgiFormat_R32G32B32_TYPELESS                      = 5,
	DxgiFormat_R32G32B32_FLOAT                         = 6,
	DxgiFormat_R32G32B32_UINT                          = 7,
	DxgiFormat_R32G32B32_SINT                          = 8,
	DxgiFormat_R16G16B16A16_TYPELESS                   = 9,
	DxgiFormat_R16G16B16A16_FLOAT                      = 10,
	DxgiFormat_R16G16B16A16_UNORM                      = 11,
	DxgiFormat_R16G16B16A16_UINT                       = 12,
	DxgiFormat_R16G16B16A16_SNORM                      = 13,
	DxgiFormat_R16G16B16A16_SINT                       = 14,
	DxgiFormat_R32G32_TYPELESS                         = 15,
	DxgiFormat_R32G32_FLOAT                            = 16,
	DxgiFormat_R32G32_UINT                             = 17,
	DxgiFormat_R32G32_SINT                             = 18,
	DxgiFormat_R32G8X24_TYPELESS                       = 19,
	DxgiFormat_D32_FLOAT_S8X24_UINT                    = 20,
	DxgiFormat_R32_FLOAT_X8X24_TYPELESS                = 21,
	DxgiFormat_X32_TYPELESS_G8X24_UINT                 = 22,
	DxgiFormat_R10G10B10A2_TYPELESS                    = 23,
	DxgiFormat_R10G10B10A2_UNORM                       = 24,
	DxgiFormat_R10G10B10A2_UINT                        = 25,
	DxgiFormat_R11G11B10_FLOAT                         = 26,
	DxgiFormat_R8G8B8A8_TYPELESS                       = 27,
	DxgiFormat_R8G8B8A8_UNORM                          = 28,
	DxgiFormat_R8G8B8A8_UNORM_SRGB                     = 29,
	DxgiFormat_R8G8B8A8_UINT                           = 30,
	DxgiFormat_R8G8B8A8_SNORM                          = 31,
	DxgiFormat_R8G8B8A8_SINT                           = 32,
	DxgiFormat_R16G16_TYPELESS                         = 33,
	DxgiFormat_R16G16_FLOAT                            = 34,
	DxgiFormat_R16G16_UNORM                            = 35,
	DxgiFormat_R16G16_UINT                             = 36,
	DxgiFormat_R16G16_SNORM                            = 37,
	DxgiFormat_R16G16_SINT                             = 38,
	DxgiFormat_R32_TYPELESS                            = 39,
	DxgiFormat_D32_FLOAT                               = 40,
	DxgiFormat_R32_FLOAT                               = 41,
	DxgiFormat_R32_UINT                                = 42,
	DxgiFormat_R32_SINT                                = 43,
	DxgiFormat_R24G8_TYPELESS                          = 44,
	DxgiFormat_D24_UNORM_S8_UINT                       = 45,
	DxgiFormat_R24_UNORM_X8_TYPELESS                   = 46,
	DxgiFormat_X24_TYPELESS_G8_UINT                    = 47,
	DxgiFormat_R8G8_TYPELESS                           = 48,
	DxgiFormat_R8G8_UNORM                              = 49,
	DxgiFormat_R8G8_UINT                               = 50,
	DxgiFormat_R8G8_SNORM                              = 51,
	DxgiFormat_R8G8_SINT                               = 52,
	DxgiFormat_R16_TYPELESS                            = 53,
	DxgiFormat_R16_FLOAT                               = 54,
	DxgiFormat_D16_UNORM                               = 55,
	DxgiFormat_R16_UNORM                               = 56,
	DxgiFormat_R16_UINT                                = 57,
	DxgiFormat_R16_SNORM                               = 58,
	DxgiFormat_R16_SINT                                = 59,
	DxgiFormat_R8_TYPELESS                             = 60,
	DxgiFormat_R8_UNORM                                = 61,
	DxgiFormat_R8_UINT                                 = 62,
	DxgiFormat_R8_SNORM                                = 63,
	DxgiFormat_R8_SINT                                 = 64,
	DxgiFormat_A8_UNORM                                = 65,
	DxgiFormat_R1_UNORM                                = 66,
	DxgiFormat_R9G9B9E5_SHAREDEXP                      = 67,
	DxgiFormat_R8G8_B8G8_UNORM                         = 68,
	DxgiFormat_G8R8_G8B8_UNORM                         = 69,
	DxgiFormat_BC1_TYPELESS                            = 70,
	DxgiFormat_BC1_UNORM                               = 71,
	DxgiFormat_BC1_UNORM_SRGB                          = 72,
	DxgiFormat_BC2_TYPELESS                            = 73,
	DxgiFormat_BC2_UNORM                               = 74,
	DxgiFormat_BC2_UNORM_SRGB                          = 75,
	DxgiFormat_BC3_TYPELESS                            = 76,
	DxgiFormat_BC3_UNORM                               = 77,
	DxgiFormat_BC3_UNORM_SRGB                          = 78,
	DxgiFormat_BC4_TYPELESS                            = 79,
	DxgiFormat_BC4_UNORM                               = 80,
	DxgiFormat_BC4_SNORM                               = 81,
	DxgiFormat_BC5_TYPELESS                            = 82,
	DxgiFormat_BC5_UNORM                               = 83,
	DxgiFormat_BC5_SNORM                               = 84,
	DxgiFormat_B5G6R5_UNORM                            = 85,
	DxgiFormat_B5G5R5A1_UNORM                          = 86,
	DxgiFormat_B8G8R8A8_UNORM                          = 87,
	DxgiFormat_B8G8R8X8_UNORM                          = 88,
	DxgiFormat_R10G10B10_XR_BIAS_A2_UNORM              = 89,
	DxgiFormat_B8G8R8A8_TYPELESS                       = 90,
	DxgiFormat_B8G8R8A8_UNORM_SRGB                     = 91,
	DxgiFormat_B8G8R8X8_TYPELESS                       = 92,
	DxgiFormat_B8G8R8X8_UNORM_SRGB                     = 93,
	DxgiFormat_BC6H_TYPELESS                           = 94,
	DxgiFormat_BC6H_UF16                               = 95,
	DxgiFormat_BC6H_SF16                               = 96,
	DxgiFormat_BC7_TYPELESS                            = 97,
	DxgiFormat_BC7_UNORM                               = 98,
	DxgiFormat_BC7_UNORM_SRGB                          = 99,
	DxgiFormat_AYUV                                    = 100,
	DxgiFormat_Y410                                    = 101,
	DxgiFormat_Y416                                    = 102,
	DxgiFormat_NV12                                    = 103,
	DxgiFormat_P010                                    = 104,
	DxgiFormat_P016                                    = 105,
	DxgiFormat_420_OPAQUE                              = 106,
	DxgiFormat_YUY2                                    = 107,
	DxgiFormat_Y210                                    = 108,
	DxgiFormat_Y216                                    = 109,
	DxgiFormat_NV11                                    = 110,
	DxgiFormat_AI44                                    = 111,
	DxgiFormat_IA44                                    = 112,
	DxgiFormat_P8                                      = 113,
	DxgiFormat_A8P8                                    = 114,
	DxgiFormat_B4G4R4A4_UNORM                          = 115,
	DxgiFormat_P208                                    = 130,
	DxgiFormat_V208                                    = 131,
	DxgiFormat_V408                                    = 132,
	DxgiFormat_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE         = 189,
	DxgiFormat_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
	DxgiFormat_FORCE_UINT                              = -1,
};

/**
 * https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/bb173067(v=vs.85)
 */
enum DxgiModeScanlineOrder : u32 {
	DxgiModeScanlineOrder_UNSPECIFIED        = 0,
	DxgiModeScanlineOrder_PROGRESSIVE        = 1,
	DxgiModeScanlineOrder_UPPER_FIELD_FIRST  = 2,
	DxgiModeScanlineOrder_LOWER_FIELD_FIRST  = 3,
};

/**
 * https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/bb173066(v=vs.85)
 */
enum DxgiModeScaling : u32 {
	DxgiModeScaling_UNSPECIFIED = 0,
	DxgiModeScaling_CENTERED    = 1,
	DxgiModeScaling_STRETCHED   = 2,
};

/**
 * https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/bb173064(v=vs.85)
 */
struct DxgiModeDesc {
	u32                   width;
	u32                   height;
	DxgiRational          refresh_rate;
	DxgiFormat            format;
	DxgiModeScanlineOrder scanline_ordering;
	DxgiModeScaling       scaling;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgicommon/ns-dxgicommon-dxgi_sample_desc
 */
struct DxgiSampleDesc {
	u32 count;
	u32 quality;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-usage
 */
enum_flags(DxgiUsage, u32) {
	DxgiUsage_CPU_ACCESS_NONE       =  0,
	DxgiUsage_CPU_ACCESS_DYNAMIC    =  1,
	DxgiUsage_CPU_ACCESS_READ_WRITE =  2,
	DxgiUsage_CPU_ACCESS_SCRATCH    =  3,
	DxgiUsage_CPU_ACCESS_FIELD      = 15,
	DxgiUsage_SHADER_INPUT          = (1 << (0 + 4)),
	DxgiUsage_RENDER_TARGET_OUTPUT  = (1 << (1 + 4)),
	DxgiUsage_BACK_BUFFER           = (1 << (2 + 4)),
	DxgiUsage_SHARED                = (1 << (3 + 4)),
	DxgiUsage_READ_ONLY             = (1 << (4 + 4)),
	DxgiUsage_DISCARD_ON_PRESENT    = (1 << (5 + 4)),
	DxgiUsage_UNORDERED_ACCESS      = (1 << (6 + 4)),
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_effect
 */
enum DxgiSwapEffect : u32 {
	DxgiSwapEffect_DISCARD         = 0,
	DxgiSwapEffect_SEQUENTIAL      = 1,
	DxgiSwapEffect_FLIP_SEQUENTIAL = 3,
	DxgiSwapEffect_FLIP_DISCARD    = 4,
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_chain_flag
 */
enum_flags(DxgiSwapChainFlags, u32) {
	DxgiSwapChainFlag_NONPREROTATED                          =   1,
	DxgiSwapChainFlag_ALLOW_MODE_SWITCH                      =   2,
	DxgiSwapChainFlag_GDI_COMPATIBLE                         =   4,
	DxgiSwapChainFlag_RESTRICTED_CONTENT                     =   8,
	DxgiSwapChainFlag_RESTRICT_SHARED_RESOURCE_DRIVER        =  16,
	DxgiSwapChainFlag_DISPLAY_ONLY                           =  32,
	DxgiSwapChainFlag_FRAME_LATENCY_WAITABLE_OBJECT          =  64,
	DxgiSwapChainFlag_FOREGROUND_LAYER                       =  128,
	DxgiSwapChainFlag_FULLSCREEN_VIDEO                       =  256,
	DxgiSwapChainFlag_YUV_VIDEO                              =  512,
	DxgiSwapChainFlag_HW_PROTECTED                           = 1024,
	DxgiSwapChainFlag_ALLOW_TEARING                          = 2048,
	DxgiSwapChainFlag_RESTRICTED_TO_ALL_HOLOGRAPHIC_DISPLAYS = 4096,
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/ns-dxgi-dxgi_swap_chain_desc
 */
struct DxgiSwapChainDesc {
	DxgiModeDesc       buffer_desc;
	DxgiSampleDesc     sample_desc;
	DxgiUsage          buffer_usage;
	u32                buffer_count;
	HWnd               output_window;
	b32                windowed;
	DxgiSwapEffect     swap_effect;
	DxgiSwapChainFlags flags;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_driver_type
 */
enum D3DDriverType {
  D3DDriverType_UNKNOWN = 0,
  D3DDriverType_HARDWARE,
  D3DDriverType_REFERENCE,
  D3DDriverType_NULL,
  D3DDriverType_SOFTWARE,
  D3DDriverType_WARP,
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_create_device_flag
 */
enum_flags(D3D11CreateDeviceFlags, u32) {
	D3D11CreateDeviceFlag_SINGLETHREADED                                = 0x1,
	D3D11CreateDeviceFlag_DEBUG                                         = 0x2,
	D3D11CreateDeviceFlag_SWITCH_TO_REF                                 = 0x4,
	D3D11CreateDeviceFlag_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS      = 0x8,
	D3D11CreateDeviceFlag_BGRA_SUPPORT                                  = 0x20,
	D3D11CreateDeviceFlag_DEBUGGABLE                                    = 0x40,
	D3D11CreateDeviceFlag_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY = 0x80,
	D3D11CreateDeviceFlag_DISABLE_GPU_TIMEOUT                           = 0x100,
	D3D11CreateDeviceFlag_VIDEO_SUPPORT                                 = 0x800
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_feature_level
 */
enum D3DFeatureLevel {
	D3DFeatureLevel_1_0_GENERIC = 0x100,
	D3DFeatureLevel_1_0_CORE    = 0x1000,
	D3DFeatureLevel_9_1         = 0x9100,
	D3DFeatureLevel_9_2         = 0x9200,
	D3DFeatureLevel_9_3         = 0x9300,
	D3DFeatureLevel_10_0        = 0xa000,
	D3DFeatureLevel_10_1        = 0xa100,
	D3DFeatureLevel_11_0        = 0xb000,
	D3DFeatureLevel_11_1        = 0xb100,
	D3DFeatureLevel_12_0        = 0xc000,
	D3DFeatureLevel_12_1        = 0xc100,
	D3DFeatureLevel_12_2        = 0xc200,
};

#define	D3D11_SDK_VERSION (7)

#define COM_UNUSED uprocptr

struct COM_Guid {
	u32 data1;
	u16 data2;
	u16 data3;
	u8  data4[8];
};

#define COM_DECLARE_INTERFACE(IName, d1, d2, d3, d4_1, d4_2, d4_3, d4_4, d4_5, d4_6, d4_7, d4_8)     \
struct IName##_VTable;                                                                               \
struct IName {                                                                                       \
	IName##_VTable* vtbl;                                                                            \
};                                                                                                   \
global const COM_Guid IName##_Guid = {d1, d2, d3, {d4_1, d4_2, d4_3, d4_4, d4_5, d4_6, d4_7, d4_8}}; \
struct IName##_VTable

#define COM_EXTEND_INTERFACE(IBase) IBase##_VTable _

#define COM_INTERFACE_GUID(IName) (&IName##_Guid)

#define COM_INTERFACE_OUT(IName, out_var) COM_INTERFACE_GUID(IName), (void**)(out_var)

#define COM_INTERFACE_OUT_PARAM(out_var) const COM_Guid* out_var##_riid, void** out_var

// Forward declarations to make errors go away
struct ID3D11Resource;
struct ID3D11RenderTargetView;

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nn-unknwn-iunknown
 */
COM_DECLARE_INTERFACE(
	IUnknown,
	0x00000000, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46
) {
	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(refiid_void)
	 */
	HResult (WINAPI* query_interface)(
		void* self,
		COM_INTERFACE_OUT_PARAM(object)
	);

	COM_UNUSED add_ref;

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-release
	 */
	u32 (WINAPI* release)(
		void* self
	);
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nn-dxgi-idxgiobject
 */
COM_DECLARE_INTERFACE(
	IDXGIObject,
	0xaec22fb8, 0x76f3, 0x4639, 0x9b, 0xe0, 0x28, 0xeb, 0x43, 0xa6, 0x7a, 0x2e
) {
	COM_EXTEND_INTERFACE(IUnknown);

	COM_UNUSED set_private_data;
	COM_UNUSED set_private_data_interface;
	COM_UNUSED get_private_data;
	COM_UNUSED get_parent;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nn-dxgi-idxgidevicesubobject
 */
COM_DECLARE_INTERFACE(
	IDXGIDeviceSubObject,
	0x3d3e0379, 0xf9de, 0x4d58, 0xbb, 0x6c, 0x18, 0xd6, 0x29, 0x92, 0xf1, 0xa6
) {
	COM_EXTEND_INTERFACE(IDXGIObject);

	COM_UNUSED get_device;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-present
 */
enum_flags(DxgiPresentFlags, u32) {
	DxgiPresentFlag_TEST                   = 0x00000001UL,
	DxgiPresentFlag_DO_NOT_SEQUENCE        = 0x00000002UL,
	DxgiPresentFlag_RESTART                = 0x00000004UL,
	DxgiPresentFlag_DO_NOT_WAIT            = 0x00000008UL,
	DxgiPresentFlag_STEREO_PREFER_RIGHT    = 0x00000010UL,
	DxgiPresentFlag_STEREO_TEMPORARY_MONO  = 0x00000020UL,
	DxgiPresentFlag_RESTRICT_TO_OUTPUT     = 0x00000040UL,
	DxgiPresentFlag_USE_DURATION           = 0x00000100UL,
	DxgiPresentFlag_ALLOW_TEARING          = 0x00000200UL,
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nn-dxgi-idxgiswapchain
 */
COM_DECLARE_INTERFACE(
	IDXGISwapChain,
	0x310d36a0, 0xd2e7, 0x4c0a, 0xaa, 0x04, 0x6a, 0x9d, 0x23, 0xb8, 0x88, 0x6a
) {
	COM_EXTEND_INTERFACE(IDXGIDeviceSubObject);

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-present
	 */
	HResult (WINAPI* present)(
		void*            self,
		u32              sync_interval,
		DxgiPresentFlags flags
	);

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-getbuffer
	 */
	HResult (WINAPI* get_buffer)(
		void* self,
		u32   buffer_idx,
		COM_INTERFACE_OUT_PARAM(surface)
	);

	COM_UNUSED set_fullscreen_state;
	COM_UNUSED get_fullscreen_state;
	COM_UNUSED get_desc;

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-resizebuffers
	 */
	HResult (WINAPI* resize_buffers)(
		void*              self,
		u32                buffer_count,
		u32                width,
		u32                height,
		DxgiFormat         format,
		DxgiSwapChainFlags flags
	);

	COM_UNUSED resize_target;
	COM_UNUSED get_containing_output;
	COM_UNUSED get_frame_statistics;
	COM_UNUSED get_last_present_count;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11device
 */
COM_DECLARE_INTERFACE(
	ID3D11Device,
	0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40
) {
	COM_EXTEND_INTERFACE(IUnknown);

	COM_UNUSED create_buffer;
	COM_UNUSED create_texture1d;
	COM_UNUSED create_texture2d;
	COM_UNUSED create_texture3d;
	COM_UNUSED create_shader_resource_view;
	COM_UNUSED create_unordered_access_view;

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createrendertargetview
	 */
	HResult (WINAPI* create_render_target_view)(
		void*                    self,
		ID3D11Resource*          resource,
		void*                    desc,
		ID3D11RenderTargetView** render_target_view
	);

	COM_UNUSED create_depth_stencil_view;
	COM_UNUSED create_input_layout;
	COM_UNUSED create_vertex_shader;
	COM_UNUSED create_geometry_shader;
	COM_UNUSED create_geometry_shader_with_stream_output;
	COM_UNUSED create_pixel_shader;
	COM_UNUSED create_hull_shader;
	COM_UNUSED create_domain_shader;
	COM_UNUSED create_compute_shader;
	COM_UNUSED create_class_linkage;
	COM_UNUSED create_blend_state;
	COM_UNUSED create_depth_stencil_state;
	COM_UNUSED create_rasterizer_state;
	COM_UNUSED create_sampler_state;
	COM_UNUSED create_query;
	COM_UNUSED create_predicate;
	COM_UNUSED create_counter;
	COM_UNUSED create_deferred_context;
	COM_UNUSED open_shared_resource;
	COM_UNUSED check_format_support;
	COM_UNUSED check_multisample_quality_levels;
	COM_UNUSED check_counter_info;
	COM_UNUSED check_counter;
	COM_UNUSED check_feature_support;
	COM_UNUSED get_private_data;
	COM_UNUSED set_private_data;
	COM_UNUSED set_private_data_interface;
	COM_UNUSED get_feature_level;
	COM_UNUSED get_creation_flags;
	COM_UNUSED get_device_removed_reason;
	COM_UNUSED get_immediate_context;
	COM_UNUSED set_exception_mode;
	COM_UNUSED get_exception_mode;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11devicechild
 */
COM_DECLARE_INTERFACE(
	ID3D11DeviceChild,
	0x1841e5c8, 0x16b0, 0x489b, 0xbc, 0xc8, 0x44, 0xcf, 0xb0, 0xd5, 0xde, 0xae
) {
	COM_EXTEND_INTERFACE(IUnknown);

	COM_UNUSED get_device;
	COM_UNUSED get_private_data;
	COM_UNUSED set_private_data;
	COM_UNUSED set_private_data_interface;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11devicecontext
 */
COM_DECLARE_INTERFACE(
	ID3D11DeviceContext,
	0xc0bfa96c, 0xe089, 0x44fb, 0x8e, 0xaf, 0x26, 0xf8, 0x79, 0x61, 0x90, 0xda
) {
	COM_EXTEND_INTERFACE(ID3D11DeviceChild);

	COM_UNUSED vs_set_constant_buffers;
	COM_UNUSED ps_set_shader_resources;
	COM_UNUSED ps_set_shader;
	COM_UNUSED ps_set_samplers;
	COM_UNUSED vs_set_shader;
	COM_UNUSED draw_indexed;
	COM_UNUSED draw;
	COM_UNUSED map;
	COM_UNUSED unmap;
	COM_UNUSED ps_set_constant_buffers;
	COM_UNUSED ia_set_input_layout;
	COM_UNUSED ia_set_vertex_buffers;
	COM_UNUSED ia_set_index_buffer;
	COM_UNUSED draw_indexed_instanced;
	COM_UNUSED draw_instanced;
	COM_UNUSED gs_set_constant_buffers;
	COM_UNUSED gs_set_shader;
	COM_UNUSED ia_set_primitive_topology;
	COM_UNUSED vs_set_shader_resources;
	COM_UNUSED vs_set_samplers;
	COM_UNUSED begin;
	COM_UNUSED end;
	COM_UNUSED get_data;
	COM_UNUSED set_predication;
	COM_UNUSED gs_set_shader_resources;
	COM_UNUSED gs_set_samplers;

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-omsetrendertargets
	 */
	void (WINAPI* om_set_render_targets)(
		void*                    self,
		u32                      num_views,
		ID3D11RenderTargetView** render_target_views,
		void*                    depth_stencil_view
	);

	COM_UNUSED om_set_render_targets_and_unordered_access_views;
	COM_UNUSED om_set_blend_state;
	COM_UNUSED om_set_depth_stencil_state;
	COM_UNUSED so_set_targets;
	COM_UNUSED draw_auto;
	COM_UNUSED draw_indexed_instanced_indirect;
	COM_UNUSED draw_instanced_indirect;
	COM_UNUSED dispatch;
	COM_UNUSED dispatch_indirect;
	COM_UNUSED rs_set_state;
	COM_UNUSED rs_set_viewports;
	COM_UNUSED rs_set_scissor_rects;
	COM_UNUSED copy_subresource_region;
	COM_UNUSED copy_resource;
	COM_UNUSED update_subresource;
	COM_UNUSED copy_structure_count;

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-clearrendertargetview
	 */
	void (WINAPI* clear_render_target_view)(
		void*                   self,
		ID3D11RenderTargetView* render_target_view,
		f32                     color_rgba[4]
	);

	COM_UNUSED clear_unordered_access_view_uint;
	COM_UNUSED clear_unordered_access_view_float;
	COM_UNUSED clear_depth_stencil_view;
	COM_UNUSED generate_mips;
	COM_UNUSED set_resource_min_lod;
	COM_UNUSED get_resource_min_lod;
	COM_UNUSED resolve_subresource;
	COM_UNUSED execute_command_list;
	COM_UNUSED hs_set_shader_resources;
	COM_UNUSED hs_set_shader;
	COM_UNUSED hs_set_samplers;
	COM_UNUSED hs_set_constant_buffers;
	COM_UNUSED ds_set_shader_resources;
	COM_UNUSED ds_set_shader;
	COM_UNUSED ds_set_samplers;
	COM_UNUSED ds_set_constant_buffers;
	COM_UNUSED cs_set_shader_resources;
	COM_UNUSED cs_set_unordered_access_views;
	COM_UNUSED cs_set_shader;
	COM_UNUSED cs_set_samplers;
	COM_UNUSED cs_set_constant_buffers;
	COM_UNUSED vs_get_constant_buffers;
	COM_UNUSED ps_get_shader_resources;
	COM_UNUSED ps_get_shader;
	COM_UNUSED ps_get_samplers;
	COM_UNUSED vs_get_shader;
	COM_UNUSED ps_get_constant_buffers;
	COM_UNUSED ia_get_input_layout;
	COM_UNUSED ia_get_vertex_buffers;
	COM_UNUSED ia_get_index_buffer;
	COM_UNUSED gs_get_constant_buffers;
	COM_UNUSED gs_get_shader;
	COM_UNUSED ia_get_primitive_topology;
	COM_UNUSED vs_get_shader_resources;
	COM_UNUSED vs_get_samplers;
	COM_UNUSED get_predication;
	COM_UNUSED gs_get_shader_resources;
	COM_UNUSED gs_get_samplers;
	COM_UNUSED om_get_render_targets;
	COM_UNUSED om_get_render_targets_and_unordered_access_views;
	COM_UNUSED om_get_blend_state;
	COM_UNUSED om_get_depth_stencil_state;
	COM_UNUSED so_get_targets;
	COM_UNUSED rs_get_state;
	COM_UNUSED rs_get_viewports;
	COM_UNUSED rs_get_scissor_rects;
	COM_UNUSED hs_get_shader_resources;
	COM_UNUSED hs_get_shader;
	COM_UNUSED hs_get_samplers;
	COM_UNUSED hs_get_constant_buffers;
	COM_UNUSED ds_get_shader_resources;
	COM_UNUSED ds_get_shader;
	COM_UNUSED ds_get_samplers;
	COM_UNUSED ds_get_constant_buffers;
	COM_UNUSED cs_get_shader_resources;
	COM_UNUSED cs_get_unordered_access_views;
	COM_UNUSED cs_get_shader;
	COM_UNUSED cs_get_samplers;
	COM_UNUSED cs_get_constant_buffers;
	COM_UNUSED clear_state;
	COM_UNUSED flush;
	COM_UNUSED get_type;
	COM_UNUSED get_context_flags;
	COM_UNUSED finish_command_list;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11view
 */
COM_DECLARE_INTERFACE(
	ID3D11View,
	0x839d1216, 0xbb2e, 0x412b, 0xb7, 0xf4, 0xa9, 0xdb, 0xeb, 0xe0, 0x8e, 0xd1
) {
	COM_EXTEND_INTERFACE(ID3D11DeviceChild);

	COM_UNUSED get_resource;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11rendertargetview
 */
COM_DECLARE_INTERFACE(
	ID3D11RenderTargetView,
	0xdfdba067, 0x0b8d, 0x4865, 0x87, 0x5b, 0xd7, 0xb4, 0x51, 0x6c, 0xc1, 0x64
) {
	COM_EXTEND_INTERFACE(ID3D11View);

	COM_UNUSED get_desc;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain
 */
DLLIMPORT HResult WINAPI D3D11CreateDeviceAndSwapChain(
	void*                  adapter,
	D3DDriverType          driver_type,
	HModule                software,
	D3D11CreateDeviceFlags flags,
	D3DFeatureLevel*       feature_levels,
	u32                    feature_levels_len,
	u32                    sdk_version,
	DxgiSwapChainDesc*     swap_chain_desc,
	IDXGISwapChain**       swap_chain,
	ID3D11Device**         device,
	D3DFeatureLevel*       feature_level,
	ID3D11DeviceContext**  immediate_context
);

#define DXGI_ERROR(error_val) ((HResult)(0x887a0000 | ((error_val) & 0xffff)))
enum DxgiError : HResult {
	DxgiError_OK                            = 0,
	DxgiError_INVALID_CALL                  = DXGI_ERROR(0x0001),
	DxgiError_NOT_FOUND                     = DXGI_ERROR(0x0002),
	DxgiError_MORE_DATA                     = DXGI_ERROR(0x0003),
	DxgiError_UNSUPPORTED                   = DXGI_ERROR(0x0004),
	DxgiError_DEVICE_REMOVED                = DXGI_ERROR(0x0005),
	DxgiError_DEVICE_HUNG                   = DXGI_ERROR(0x0006),
	DxgiError_DEVICE_RESET                  = DXGI_ERROR(0x0007),
	DxgiError_WAS_STILL_DRAWING             = DXGI_ERROR(0x000a),
	DxgiError_FRAME_STATISTICS_DISJOINT     = DXGI_ERROR(0x000b),
	DxgiError_GRAPHICS_VIDPN_SOURCE_IN_USE  = DXGI_ERROR(0x000c),
	DxgiError_DRIVER_INTERNAL_ERROR         = DXGI_ERROR(0x0020),
	DxgiError_NONEXCLUSIVE                  = DXGI_ERROR(0x0021),
	DxgiError_NOT_CURRENTLY_AVAILABLE       = DXGI_ERROR(0x0022),
	DxgiError_REMOTE_CLIENT_DISCONNECTED    = DXGI_ERROR(0x0023),
	DxgiError_REMOTE_OUTOFMEMORY            = DXGI_ERROR(0x0024),
	DxgiError_ACCESS_LOST                   = DXGI_ERROR(0x0026),
	DxgiError_WAIT_TIMEOUT                  = DXGI_ERROR(0x0027),
	DxgiError_SESSION_DISCONNECTED          = DXGI_ERROR(0x0028),
	DxgiError_RESTRICT_TO_OUTPUT_STALE      = DXGI_ERROR(0x0029),
	DxgiError_CANNOT_PROTECT_CONTENT        = DXGI_ERROR(0x002a),
	DxgiError_ACCESS_DENIED                 = DXGI_ERROR(0x002b),
	DxgiError_NAME_ALREADY_EXISTS           = DXGI_ERROR(0x002c),
	DxgiError_SDK_COMPONENT_MISSING         = DXGI_ERROR(0x002d),
	DxgiError_NOT_CURRENT                   = DXGI_ERROR(0x002e),
	DxgiError_HW_PROTECTION_OUTOFMEMORY     = DXGI_ERROR(0x0030),
	DxgiError_DYNAMIC_CODE_POLICY_VIOLATION = DXGI_ERROR(0x0031),
	DxgiError_NON_COMPOSITED_UI             = DXGI_ERROR(0x0032),
	DxgiError_MODE_CHANGE_IN_PROGRESS       = DXGI_ERROR(0x0025),
	DxgiError_CACHE_CORRUPT                 = DXGI_ERROR(0x0033),
	DxgiError_CACHE_FULL                    = DXGI_ERROR(0x0034),
	DxgiError_CACHE_HASH_COLLISION          = DXGI_ERROR(0x0035),
	DxgiError_ALREADY_EXISTS                = DXGI_ERROR(0x0036),
};

#define DXGI_STATUS(status_val) ((HResult)(0x087a0000 | ((status_val) & 0xffff)))
enum DxgiStatus : HResult {
	DxgiStatus_OCCLUDED                     = DXGI_STATUS(0x0001),
	DxgiStatus_CLIPPED                      = DXGI_STATUS(0x0002),
	DxgiStatus_NO_REDIRECTION               = DXGI_STATUS(0x0004),
	DxgiStatus_NO_DESKTOP_ACCESS            = DXGI_STATUS(0x0005),
	DxgiStatus_GRAPHICS_VIDPN_SOURCE_IN_USE = DXGI_STATUS(0x0006),
	DxgiStatus_MODE_CHANGED                 = DXGI_STATUS(0x0007),
	DxgiStatus_MODE_CHANGE_IN_PROGRESS      = DXGI_STATUS(0x0008),
	DxgiStatus_UNOCCLUDED                   = DXGI_STATUS(0x0009),
	DxgiStatus_DDA_WAS_STILL_DRAWING        = DXGI_STATUS(0x000A),
	DxgiStatus_PRESENT_REQUIRED             = DXGI_STATUS(0x002F),
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11resource
 */
COM_DECLARE_INTERFACE(
	ID3D11Resource,
	0xdc8e63f3, 0xd12b, 0x4952, 0xb4, 0x7b, 0x5e, 0x45, 0x02, 0x6a, 0x86, 0x2d
) {
	COM_EXTEND_INTERFACE(ID3D11DeviceChild);

	COM_UNUSED get_type;
	COM_UNUSED set_eviction_priority;
	COM_UNUSED get_eviction_priority;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11texture2d
 */
COM_DECLARE_INTERFACE(
	ID3D11Texture2D,
	0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c
) {
	COM_EXTEND_INTERFACE(ID3D11Resource);

	COM_UNUSED get_desc;
};

#endif // __WINDOWS_HPP__
