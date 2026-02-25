#pragma once

#include <string>
#include <functional>

class MenuOption {
private:
    const std::string description;
    const std::function<void()> action;
public:
    MenuOption(std::string description, std::function<void()> action);
    const std::string& getDescription() const;
    void execute() const;
};
