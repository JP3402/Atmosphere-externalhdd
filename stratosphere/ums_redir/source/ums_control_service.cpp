#include "ums_control_service.hpp"
#include "virtual_game_card.hpp"

namespace ams::ums {

    Result UmsControlService::MountExternalFile(const sf::InPath &path) {
        /* IPC command handling logic for mounting an external file. */
        R_RETURN(VirtualGameCard::ActivateRedirection(path.str));
    }

    Result UmsControlService::UnmountExternalFile() {
        R_RETURN(VirtualGameCard::DeactivateRedirection());
    }

}
