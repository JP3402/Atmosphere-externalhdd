#include "ums_mitm_service.hpp"
#include "virtual_game_card.hpp"

namespace ams::ums::mitm {

    /* UmsFsMitmService implementation. */

    Result UmsFsMitmService::OpenGameCardFileSystem(sf::Out<sf::SharedPointer<fssrv::sf::IFileSystem>> out, u32 handle, u32 partition) {
        /* Check if we have an active redirection. */
        if (VirtualGameCard::IsRedirectionActive()) {
            /* Error Handling: If USB is disconnected, return ResultFsTargetNotFound (0x2504). */
            if (!ums::UsbHandler::IsReady()) {
                R_THROW(fs::ResultTargetNotFound());
            }
            R_RETURN(VirtualGameCard::OpenRedirectionFileSystem(out));
        }

        /* Otherwise, forward to the real service. */
        R_RETURN(sm::mitm::ResultShouldForwardToSession());
    }

    Result UmsFsMitmService::OpenDeviceOperator(sf::Out<sf::SharedPointer<fssrv::sf::IDeviceOperator>> out) {
        /* Intercept DeviceOperator to simulate GameCard presence. */
        if (VirtualGameCard::IsRedirectionActive()) {
            /* Open the real device operator from the forward service. */
            sf::SharedPointer<fssrv::sf::IDeviceOperator> forward_device_operator;
            R_ABORT_UNLESS(m_forward_service->OpenDeviceOperator(std::addressof(forward_device_operator)));

            /* Return our MitM wrapper. */
            out.SetValue(sf::CreateSharedObjectEmplaced<fssrv::sf::IDeviceOperator, UmsDeviceOperatorService>(forward_device_operator));
            R_SUCCEED();
        }

        R_RETURN(sm::mitm::ResultShouldForwardToSession());
    }

    /* UmsDeviceOperatorService implementation. */

    Result UmsDeviceOperatorService::IsGameCardInserted(sf::Out<bool> out) {
        if (VirtualGameCard::IsRedirectionActive()) {
            /* Return false if USB is disconnected mid-game. */
            out.SetValue(ums::UsbHandler::IsReady());
            R_SUCCEED();
        }
        R_RETURN(sm::mitm::ResultShouldForwardToSession());
    }

    Result UmsDeviceOperatorService::GetGameCardHandle(sf::Out<u32> out) {
        if (VirtualGameCard::IsRedirectionActive()) {
            out.SetValue(VirtualGameCard::GetSpoofedHandle());
            R_SUCCEED();
        }
        R_RETURN(sm::mitm::ResultShouldForwardToSession());
    }

    Result UmsDeviceOperatorService::ChallengeCard(sf::OutBuffer out_res, sf::InBuffer in_chal, u32 handle) {
        if (VirtualGameCard::IsRedirectionActive()) {
            R_RETURN(VirtualGameCard::HandleGcdChallenge(in_chal.GetPointer(), out_res.GetPointer()));
        }
        R_RETURN(sm::mitm::ResultShouldForwardToSession());
    }

}
