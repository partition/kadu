/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "accounts/account.h"
#include "contacts/contact-account-data.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "protocols/services/avatar-service.h"

#include "avatar-manager.h"

AvatarManager * AvatarManager::Instance = 0;

AvatarManager * AvatarManager::instance()
{
	if (!Instance)
		Instance = new AvatarManager();

	return Instance;
}


AvatarManager::AvatarManager()
{
	triggerAllAccountsRegistered();
}

AvatarManager::~AvatarManager()
{
	triggerAllAccountsUnregistered();
}

AvatarService * AvatarManager::avatarService(Account *account)
{
	Protocol *protocol = account->protocol();
	if (!protocol)
		return 0;
	
	return protocol->avatarService();
}

AvatarService * AvatarManager::avatarService(ContactAccountData *contactAccountData)
{
	Account *account = contactAccountData->account();
	if (!account)
		return 0;

	return avatarService(account);
}

QString AvatarManager::avatarFileName(Avatar avatar)
{
	ContactAccountData *cad = avatar.contactAccountData();
	if (!cad)
		return QString::null;

	Account *account = cad->account();
	if (!account)
		return QString::null;

	return QString("%1-%2").arg(cad->contact().uuid().toString(), account->uuid().toString());
}

void AvatarManager::accountRegistered(Account *account)
{
	AvatarService *service = avatarService(account);
	if (!service)
		return;

	connect(service, SIGNAL(avatarFetched(ContactAccountData *, QPixmap)),
			this, SLOT(avatarFetched(ContactAccountData *, QPixmap)));
}

void AvatarManager::accountUnregistered(Account *account)
{
	AvatarService *service = avatarService(account);
	if (!service)
		return;

	disconnect(service, SIGNAL(avatarFetched(ContactAccountData *, QPixmap)),
			   this, SLOT(avatarFetched(ContactAccountData *, QPixmap)));
}

void AvatarManager::updateAvatar(ContactAccountData *contactAccountData)
{
	QDateTime lastUpdated = contactAccountData->avatar().lastUpdated();
	if (lastUpdated.isValid() && lastUpdated.secsTo(QDateTime()) < 60*60)
		return;

	AvatarService *service = avatarService(contactAccountData);
	if (!service)
		return;

	service->fetchAvatar(contactAccountData);
}

void AvatarManager::avatarFetched(ContactAccountData *contactAccountData, QPixmap pixmap)
{
	Avatar &avatar = contactAccountData->avatar();
	avatar.setLastUpdated(QDateTime());
	avatar.setPixmap(pixmap);
	avatar.setFileName(dataPath("avatars/") + avatarFileName(avatar));

	emit avatarUpdated(contactAccountData);
}
