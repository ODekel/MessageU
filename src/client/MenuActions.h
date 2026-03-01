#pragma once

#include "ClientInfo.h"
#include "UserInfo.h"

void registerUser(const UserInfoPtr& userInfo, const ClientInfoPtr& sock);
void getClientList(const UserInfoPtr& userInfo, const ClientInfoPtr& sock);
void getClientPublicKey(const UserInfoPtr& userInfo, const ClientInfoPtr& sock);
void getMessages(const UserInfoPtr& userInfo, const ClientInfoPtr& sock);
void sendTextMessage(const UserInfoPtr& userInfo, const ClientInfoPtr& sock);
void requestSymmetricKey(const UserInfoPtr& userInfo, const ClientInfoPtr& sock);
void sendSymmetricKey(const UserInfoPtr& userInfo, const ClientInfoPtr& sock);
void exitClient(const UserInfoPtr& userInfo, const ClientInfoPtr& sock);
