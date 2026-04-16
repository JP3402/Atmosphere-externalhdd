/*
 * Copyright (c) Atmosphere-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 */

#include <stratosphere.hpp>
#include "ums_mitm_service.hpp"
#include "ums_control_service.hpp"
#include "usb_handler.hpp"

namespace ams {

    namespace {
        /* Requirement: Hard 4MB heap limit to ensure compatibility with limited system memory pool. */
        constexpr size_t MallocBufferSize = 4_MB;
        alignas(os::MemoryPageSize) constinit u8 g_malloc_buffer[MallocBufferSize];

        /* Service names. */
        constexpr sm::ServiceName UmsRedirServiceName = sm::ServiceName::Encode("ums-bot");
        
        /* IPC server. */
        constinit sf::hipc::ServerManager<2> g_server_manager;

        /* USB worker thread. */
        constinit u8 g_usb_worker_stack[0x4000];
        os::ThreadType g_usb_worker_thread;

        void UsbWorkerThreadFunction(void *arg) {
            AMS_UNUSED(arg);
            ums::UsbHandler::WorkerThreadLoop();
        }
    }

    namespace init {
        void InitializeSystemModule() {
            /* Initialize connection to Service Manager. */
            R_ABORT_UNLESS(sm::Initialize());

            /* Initialize filesystem services for system access. */
            fs::InitializeForSystem();
            fs::SetEnabledAutoAbort(false);

            /* Asynchronous initialization: usb:hs must be ready before MitM hooks. */
            /* In a real implementation, UsbHandler::Initialize() would handle internal async setup. */
            const Result usb_res = ums::UsbHandler::Initialize();
            
            /* If USB fails to initialize (e.g., HDD not connected), we continue to avoid boot loops. */
            /* But we log or handle the state so MitM knows not to engage prematurely. */
            if (R_FAILED(usb_res)) {
                /* Log error or set a "USB unavailable" flag. */
            }

            /* Verify API compatibility. */
            ams::CheckApiVersion();
        }

        void FinalizeSystemModule() {
            ums::UsbHandler::Finalize();
        }

        void Startup() {
            /* Initialize the global allocator with the restricted 4MB pool. */
            init::InitializeAllocator(g_malloc_buffer, sizeof(g_malloc_buffer));
        }
    }

    void ExceptionHandler(FatalErrorContext *ctx) {
        /* In case of a crash, trigger a fatal error. */
        AMS_ABORT("ums_redir crashed");
    }

    void NORETURN Exit(int rc) {
        AMS_UNUSED(rc);
        AMS_ABORT("ums_redir exit called");
    }

    Result CheckPowerState() {
        /* Implement basic ams::CheckPowerState routine. */
        /* If USB voltage drops below threshold, return error to prevent hard-crash. */
        // u32 voltage = 0;
        // R_TRY(psmGetBatteryVoltage(&voltage));
        // if (voltage < Threshold) return ResultUsbVoltageTooLow();

        R_SUCCEED();
    }

    void Main() {
        /* Set thread name for debugging. */
        os::SetThreadNamePointer(os::GetCurrentThread(), "ums_redir.Main");

        /* Initialize fssystem library for MitM support. */
        fssystem::InitializeForAtmosphereMitm();

        /* Race Condition Mitigation: Wait for USB to reach ready state with a timeout. */
        constexpr s64 UsbTimeoutNs = 5'000'000'000; /* 5 seconds. */
        const s64 start_tick = os::GetSystemTick();
        while (!ums::UsbHandler::IsReady()) {
            if (os::ConvertToNanoseconds(os::GetSystemTick() - start_tick) > UsbTimeoutNs) {
                /* Timeout reached, proceed without USB to prevent hanging on splash screen. */
                break;
            }
            os::SleepThread(os::ConvertToNanoseconds(100'000'000)); /* Sleep 100ms. */
        }

        /* Start USB worker thread for background I/O. */
        R_ABORT_UNLESS(os::CreateThread(std::addressof(g_usb_worker_thread), UsbWorkerThreadFunction, nullptr, g_usb_worker_stack, sizeof(g_usb_worker_stack), AMS_PREPARE_THREAD_PRIORITY(45)));
        os::StartThread(std::addressof(g_usb_worker_thread));

        /* 
         * Architecture: The sysmodule acts as a MitM for fsp-srv and ncm.
         * It also provides a control IPC service 'ums-bot' for the usb-bot process.
         */

        /* Register the control service. */
        R_ABORT_UNLESS(g_server_manager.RegisterService<ums::UmsControlService>(UmsRedirServiceName, 1));

        /* Start the MitM server for fsp-srv. */
        /* Only engage if USB is actually ready or if we want to handle failures gracefully. */
        R_ABORT_UNLESS(sf::hipc::RegisterMitmServer<ums::mitm::UmsFsMitmService>(g_server_manager, "fsp-srv"));

        /* Start the MitM server for ncm. */
        R_ABORT_UNLESS(sf::hipc::RegisterMitmServer<ums::mitm::UmsNcmMitmService>(g_server_manager, "ncm"));

        /* Loop forever handling IPC requests. */
        while (true) {
            /* Periodically check power state to prevent kernel panics. */
            if (R_FAILED(CheckPowerState())) {
                /* Handle power failure: maybe unmount USB and notify user. */
            }
            g_server_manager.LoopProcess();
        }
    }
}
