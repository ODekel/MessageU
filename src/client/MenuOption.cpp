#include "MenuOption.h"

MenuOption::MenuOption(std::string&& description, std::function<void (const UserInfoPtr&, const SocketPtr&)> action)
    : description(std::move(description)), action(action) { }

const std::string& MenuOption::getDescription() const {
    return description;
}

void MenuOption::execute(const UserInfoPtr& userInfo, const SocketPtr& sock) const {
    action(userInfo, sock);
}
