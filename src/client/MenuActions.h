#pragma once

#include "Typedefs.h"
#include "UserInfo.h"

void registerUser(const UserInfoPtr& userInfo, const SocketPtr& sock);
void getClientList(const UserInfoPtr& userInfo, const SocketPtr& sock);
void getClientPublicKey(const UserInfoPtr& userInfo, const SocketPtr& sock);
void getMessages(const UserInfoPtr& userInfo, const SocketPtr& sock);
void sendMessage(const UserInfoPtr& userInfo, const SocketPtr& sock);
void requestSymmetricKey(const UserInfoPtr& userInfo, const SocketPtr& sock);
void sendSymmetricKey(const UserInfoPtr& userInfo, const SocketPtr& sock);
void exitClient(const UserInfoPtr& userInfo, const SocketPtr& sock);
