#pragma once

#include "UserInfo.h"

void registerUser(const UserInfoPtr& userInfo);
void getClientList(const UserInfoPtr& userInfo);
void getClientPublicKey(const UserInfoPtr& userInfo);
void getMessages(const UserInfoPtr& userInfo);
void sendMessage(const UserInfoPtr& userInfo);
void requestSymmetricKey(const UserInfoPtr& userInfo);
void sendSymmetricKey(const UserInfoPtr& userInfo);
void exitClient(const UserInfoPtr& userInfo);
