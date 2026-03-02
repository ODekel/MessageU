#pragma once

#include "ClientInfo.h"
#include "UserInfo.h"
#include <functional>
#include <memory>
#include <string>

class MenuOption {
private:
    const std::string description;
    const std::function<void (const UserInfoPtr&, const ClientInfoPtr&)> action;
public:
    MenuOption(std::string&& description, std::function<void (const UserInfoPtr&, const ClientInfoPtr&)> action);
    const std::string& getDescription() const;
    void execute(const UserInfoPtr& userInfo, const ClientInfoPtr& sock) const;
};
