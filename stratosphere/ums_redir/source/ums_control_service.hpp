#pragma once
#include <stratosphere.hpp>

namespace ams::ums {

    /* Define the control interface for the usb-bot process. */
    #define AMS_UMS_CONTROL_INTERFACE_INFO(C, H) \
        AMS_SF_METHOD_INFO(C, H, 0, Result, MountExternalFile, (const sf::InPath &path), (path)) \
        AMS_SF_METHOD_INFO(C, H, 1, Result, UnmountExternalFile, (), ())

    AMS_SF_DEFINE_INTERFACE(ams::ums, IUmsControlInterface, AMS_UMS_CONTROL_INTERFACE_INFO, 0x554D5300)

    class UmsControlService {
        public:
            Result MountExternalFile(const sf::InPath &path);
            Result UnmountExternalFile();
    };
    static_assert(sf::IsInterface<UmsControlService>);

}
