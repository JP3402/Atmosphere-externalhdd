#include "virtual_game_card.hpp"

namespace ams::ums {

    namespace {
        bool g_is_redirection_active = false;
        char g_xci_path[fs::EntryNameLengthMax];
    }

    bool VirtualGameCard::IsRedirectionActive() {
        return g_is_redirection_active;
    }

    Result VirtualGameCard::OpenRedirectionFileSystem(sf::Out<sf::SharedPointer<fssrv::sf::IFileSystem>> &out) {
        /* 
         * When an .xci path is provided, redirect MountGameCard calls.
         * The implementation redirects read offsets to the USB file handle.
         */
        
        /* 
         * Boilerplate for returning a redirected filesystem.
         * In a real implementation, we would use fssystem to open the .xci from USB.
         */
        // auto usb_file = OpenUsbFile(g_xci_path);
        // out.SetValue(sf::CreateSharedObjectEmplaced<fssrv::sf::IFileSystem, fssystem::XciFileSystem>(usb_file));

        /* Mock implementation for boilerplate: return an empty FS if we don't have a real one. */
        // out.SetValue(sf::CreateSharedObjectEmplaced<fssrv::sf::IFileSystem, fssystem::EmptyFileSystem>());

        R_SUCCEED();
    }

    Result VirtualGameCard::ActivateRedirection(const char *path) {
        std::strncpy(g_xci_path, path, sizeof(g_xci_path) - 1);
        g_xci_path[sizeof(g_xci_path) - 1] = '\0';
        g_is_redirection_active = true;
        R_SUCCEED();
    }

    Result VirtualGameCard::DeactivateRedirection() {
        g_is_redirection_active = false;
        R_SUCCEED();
    }

    Result VirtualGameCard::HandleGcdChallenge(const void *challenge, void *response) {
        /* 
         * Handle the GameCard ASIC simulation.
         * The system must believe the "GameCard" challenge/response (GCD) is valid.
         * This usually involves simulating the responses expected by the Secure Partition.
         */
        
        /* Boilerplate for simulation. */
        std::memset(response, 0xEE, 0x10); /* Dummy valid-looking response. */
        
        R_SUCCEED();
    }

}
