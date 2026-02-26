#pragma once

#include "UserInfo.h"
#include <functional>
#include <memory>
#include <string>

class MenuOption {
private:
    const std::string& description;
    const std::function<void (const std::unique_ptr<UserInfo>&)>& action;
public:
    MenuOption(std::string description, const std::function<void (const std::unique_ptr<UserInfo>&)>& action);
    const std::string& getDescription() const;
    void execute(const std::unique_ptr<UserInfo>& userInfo) const;
};
