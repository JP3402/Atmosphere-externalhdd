#pragma once
#include <stratosphere.hpp>

namespace ams::ums::mitm {

    /* Define the MitM interface for fsp-srv. */
    #define AMS_UMS_FS_MITM_INTERFACE_INFO(C, H) \
        AMS_SF_METHOD_INFO(C, H, 31, Result, OpenGameCardFileSystem, (sf::Out<sf::SharedPointer<fssrv::sf::IFileSystem>> out, u32 handle, u32 partition), (out, handle, partition)) \
        AMS_SF_METHOD_INFO(C, H, 400, Result, OpenDeviceOperator, (sf::Out<sf::SharedPointer<fssrv::sf::IDeviceOperator>> out), (out))

    AMS_SF_DEFINE_MITM_INTERFACE(ams::ums::mitm, IUmsFsMitmInterface, AMS_UMS_FS_MITM_INTERFACE_INFO, 0x7DF34ED2)

    class UmsFsMitmService : public sf::MitmServiceImplBase {
        public:
            using MitmServiceImplBase::MitmServiceImplBase;

            static bool ShouldMitm(const sm::MitmProcessInfo &client_info) {
                /* MitM for any process that might want to access the GameCard. */
                return true; 
            }

            /* Overridden commands. */
            Result OpenGameCardFileSystem(sf::Out<sf::SharedPointer<fssrv::sf::IFileSystem>> out, u32 handle, u32 partition);
            Result OpenDeviceOperator(sf::Out<sf::SharedPointer<fssrv::sf::IDeviceOperator>> out);
    };
    static_assert(IsIUmsFsMitmInterface<UmsFsMitmService>);

    /* Define the MitM interface for IDeviceOperator. */
    #define AMS_UMS_IDEVICE_OPERATOR_MITM_INTERFACE_INFO(C, H) \
        AMS_SF_METHOD_INFO(C, H, 200, Result, IsGameCardInserted, (sf::Out<bool> out), (out)) \
        AMS_SF_METHOD_INFO(C, H, 202, Result, GetGameCardHandle, (sf::Out<u32> out), (out)) \
        AMS_SF_METHOD_INFO(C, H, 205, Result, ChallengeCard, (sf::OutBuffer response, sf::InBuffer challenge, u32 handle), (response, challenge, handle))

    AMS_SF_DEFINE_INTERFACE(ams::ums::mitm, IUmsDeviceOperator, AMS_UMS_IDEVICE_OPERATOR_MITM_INTERFACE_INFO, 0x1484E21C)

    class UmsDeviceOperatorService : public sf::MitmServiceImplBase {
        public:
            using MitmServiceImplBase::MitmServiceImplBase;

            /* Overridden commands for IDeviceOperator. */
            Result IsGameCardInserted(sf::Out<bool> out);
            Result GetGameCardHandle(sf::Out<u32> out);
            Result ChallengeCard(sf::OutBuffer out_res, sf::InBuffer in_chal, u32 handle);
    };

    /* Define the MitM interface for ncm. */
    #define AMS_UMS_NCM_MITM_INTERFACE_INFO(C, H) \
        /* Add necessary ncm commands here. For now, we'll forward most. */

    AMS_SF_DEFINE_MITM_INTERFACE(ams::ums::mitm, IUmsNcmMitmInterface, AMS_UMS_NCM_MITM_INTERFACE_INFO, 0x1484E21C)

    class UmsNcmMitmService : public sf::MitmServiceImplBase {
        public:
            using MitmServiceImplBase::MitmServiceImplBase;

            static bool ShouldMitm(const sm::MitmProcessInfo &client_info) {
                return true;
            }
    };

}
