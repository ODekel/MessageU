#include "MenuOption.h"

MenuOption::MenuOption(std::string description, std::function<void()> action)
    : description(std::move(description)), action(std::move(action)) { }

const std::string& MenuOption::getDescription() const {
    return description;
}

void MenuOption::execute() const {
    action();
}
