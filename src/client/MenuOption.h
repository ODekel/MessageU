#pragma once

#include "Typedefs.h"
#include "UserInfo.h"
#include <functional>
#include <memory>
#include <string>

class MenuOption {
private:
    const std::string description;
    const std::function<void (const UserInfoPtr&, const SocketPtr&)> action;
public:
    MenuOption(std::string&& description, std::function<void (const UserInfoPtr&, const SocketPtr&)> action);
    const std::string& getDescription() const;
    void execute(const UserInfoPtr& userInfo, const SocketPtr& sock) const;
};
