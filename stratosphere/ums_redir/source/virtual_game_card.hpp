#pragma once
#include <stratosphere.hpp>

namespace ams::ums {

    class VirtualGameCard {
        public:
            static bool IsRedirectionActive();
            static Result OpenRedirectionFileSystem(sf::Out<sf::SharedPointer<fssrv::sf::IFileSystem>> &out);
            
            static Result ActivateRedirection(const char *path);
            static Result DeactivateRedirection();

            /* Handle the GameCard ASIC simulation challenges. */
            static Result HandleGcdChallenge(const void *challenge, void *response);

            static u32 GetSpoofedHandle();
    };

}
