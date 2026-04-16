#include "ums_mitm_service.hpp"
#include "virtual_game_card.hpp"

namespace ams::ums::mitm {

    /* UmsFsMitmService implementation. */

    Result UmsFsMitmService::OpenGameCardFileSystem(sf::Out<sf::SharedPointer<fssrv::sf::IFileSystem>> out, u32 handle, u32 partition) {
        /* Check if we have an active redirection. */
        if (VirtualGameCard::IsRedirectionActive()) {
            R_RETURN(VirtualGameCard::OpenRedirectionFileSystem(out));
        }

        /* Otherwise, forward to the real service. */
        R_RETURN(sm::mitm::ResultShouldForwardToSession());
    }

    Result UmsFsMitmService::OpenDeviceOperator(sf::Out<sf::SharedPointer<fssrv::sf::IDeviceOperator>> out) {
        /* Intercept DeviceOperator to simulate GameCard presence. */
        if (VirtualGameCard::IsRedirectionActive()) {
            out.SetValue(sf::CreateSharedObjectEmplaced<fssrv::sf::IDeviceOperator, UmsDeviceOperatorService>(m_forward_service));
            R_SUCCEED();
        }

        R_RETURN(sm::mitm::ResultShouldForwardToSession());
    }

    /* UmsDeviceOperatorService implementation. */

    Result UmsDeviceOperatorService::IsGameCardInserted(sf::Out<bool> out) {
        if (VirtualGameCard::IsRedirectionActive()) {
            out.SetValue(true);
            R_SUCCEED();
        }
        R_RETURN(sm::mitm::ResultShouldForwardToSession());
    }

    Result UmsDeviceOperatorService::GetGameCardHandle(sf::Out<u32> out) {
        if (VirtualGameCard::IsRedirectionActive()) {
            out.SetValue(0xDEADBEEF); /* Simulated handle. */
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
