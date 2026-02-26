#include "Menu.h"
#include "MenuActions.h"
#include "MenuOption.h"
#include <iostream>

void operateMenu(const std::unique_ptr<UserInfo>& userInfo) {
    static const std::vector<std::pair<std::string, MenuOption>> options = {
        { "110", MenuOption("Register", registerUser) },
        { "120", MenuOption("Request for clients list", getClientList) },
        { "130", MenuOption("Request for public key", getClientPublicKey) },
        { "140", MenuOption("Request for waiting messages", getMessages) },
        { "150", MenuOption("Send a text message", sendMessage) },
        { "151", MenuOption("Send a request for symmetric key", requestSymmetricKey) },
        { "152", MenuOption("Send your symmetric key", sendSymmetricKey) },
        { "0", MenuOption("Exit client", exitClient) }
    };

    std::cout << std::endl;
    for (const auto& [key, option] : options) {
        std::cout << key << ") " << option.getDescription() << std::endl;
    }
    std::cout << "? ";
    std::string input;
    std::cin >> input;
    std::cout << std::endl;
    for (const auto& [key, option] : options) {
        if (key == input) {
            option.execute(userInfo);
            return;
        }
    }
    std::cout << "Invalid option. Please try again." << std::endl;
}
