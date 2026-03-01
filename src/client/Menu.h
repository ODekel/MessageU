#pragma once

#include "ClientInfo.h"
#include "UserInfo.h"
#include <memory>

// Handle the main menu of the client.
void operateMenu(const UserInfoPtr& userInfo, const ClientInfoPtr& sock);
