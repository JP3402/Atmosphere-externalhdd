#pragma once
#include <stratosphere.hpp>

namespace ams::ums {

    class UsbHandler {
        public:
            static Result Initialize();
            static void Finalize();
            static void WorkerThreadLoop();
            static bool IsReady();

            static Result RequestRead(u64 offset, void *buffer, size_t size);
    };

}
