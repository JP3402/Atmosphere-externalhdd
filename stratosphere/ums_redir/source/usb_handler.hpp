#pragma once
#include <stratosphere.hpp>

namespace ams::ums {

    class UsbHandler {
        public:
            static Result Initialize();
            static void Finalize();
            static void WorkerThreadLoop();
    };

}
