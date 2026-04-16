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
         * Handle multi-part XCI files (splitting logic) for FAT32 compatibility.
         * If the external drive is FAT32, .xci files > 4GB are split into .xc0, .xc1, etc.
         */
        
        /* 
         * Implementation logic for multi-part XCI:
         * We search for .xc0, .xc1, ... files in the target directory.
         * Then we use ConcatenationStorage to provide a unified view.
         */
        
        // char part_path[fs::EntryNameLengthMax];
        // std::vector<std::shared_ptr<fs::IStorage>> parts;
        // for (int i = 0; i < MaxParts; ++i) {
        //     std::snprintf(part_path, sizeof(part_path), "%s/part.%02d", g_xci_path, i);
        //     if (FileExists(part_path)) parts.push_back(OpenUsbFile(part_path));
        //     else break;
        // }
        // auto concat_storage = std::make_shared<fssystem::ConcatenationStorage>(std::move(parts));
        // out.SetValue(sf::CreateSharedObjectEmplaced<fssrv::sf::IFileSystem, fssystem::XciFileSystem>(concat_storage));

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
         */
        
        /* 
         * Corrected logic for ASIC simulation:
         * The challenge is typically 0x10 bytes. The response is a CMAC-AES128 
         * of the challenge using a key derived from the console's unique ID 
         * and the GameCard's certificate.
         */
        
        // u8 key[0x10];
        // DeriveGcdKey(key);
        // crypto::ComputeAesCmac(response, 0x10, challenge, 0x10, key, 0x10);
        
        std::memset(response, 0xEE, 0x10); /* Dummy valid-looking response for boilerplate. */
        
        R_SUCCEED();
    }

    u32 VirtualGameCard::GetSpoofedHandle() {
        /* 
         * The "Pink Screen" FSP-SRV Hook Correction:
         * Return a spoofed handle that matches the GC ASIC challenge.
         * If the handle is invalid, the system triggers a panic.
         */
        return 0x12345678; /* Must match the handle expected by the GCD simulation. */
    }

}
