#include "MenuOption.h"

MenuOption::MenuOption(std::string description, const std::function<void (const std::unique_ptr<UserInfo>&)>& action)
    : description(std::move(description)), action(action) { }

const std::string& MenuOption::getDescription() const {
    return description;
}

void MenuOption::execute(const std::unique_ptr<UserInfo>& userInfo) const {
    action(userInfo);
}
