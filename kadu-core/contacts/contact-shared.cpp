
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "buddies/avatar.h"
#include "buddies/avatar-manager.h"
#include "buddies/avatar-shared.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "contacts/contact-details.h"
#include "contacts/contact-manager.h"
#include "protocols/protocol.h"

#include "contact-shared.h"

ContactShared * ContactShared::loadFromStorage(StoragePoint *storagePoint)
{
	ContactShared *result = new ContactShared();
	result->setStorage(storagePoint);

	return result;
}

ContactShared::ContactShared(QUuid uuid) :
		Shared(uuid),
		ContactAccount(Account::null), ContactAvatar(Avatar::null), OwnerBuddy(Buddy::null),
		Priority(-1)
{
}

ContactShared::~ContactShared()
{
	triggerAllProtocolsUnregistered();
}

StorableObject * ContactShared::storageParent()
{
	return ContactManager::instance();
}

QString ContactShared::storageNodeName()
{
	return QLatin1String("Contact");
}

void ContactShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	Id = loadValue<QString>("Id");
	Priority = loadValue<int>("Priority", -1);

	ContactAccount = AccountManager::instance()->byUuid(loadValue<QString>("Account"));

	QString buddyUuid = loadValue<QString>("Buddy");
	if (buddyUuid.isNull())
		buddyUuid = loadValue<QString>("Contact");

	setOwnerBuddy(BuddyManager::instance()->byUuid(buddyUuid));

	if (storage()->point().isElement())
	{
		QDomNodeList avatars = storage()->point().elementsByTagName("Avatar");
		if (avatars.count() == 1)
			if (!avatars.at(0).firstChildElement("LastUpdated").isNull())
				storage()->point().removeChild(avatars.at(0));
	}

	ContactAvatar = AvatarManager::instance()->byUuid(loadValue<QString>("Avatar"));

	triggerAllProtocolsRegistered();
}

void ContactShared::aboutToBeRemoved()
{
	// clean up references
	ContactAccount = Account::null;
	OwnerBuddy = Buddy::null;

	AvatarManager::instance()->removeItem(ContactAvatar);
	ContactAvatar = Avatar::null;

	setDetails(0);
}

void ContactShared::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	Shared::store();

	storeValue("Id", Id);
	storeValue("Priority", Priority);
	storeValue("Account", ContactAccount.uuid().toString());
	storeValue("Buddy", OwnerBuddy.uuid().toString());
	if (!ContactAvatar.isNull())
		storeValue("Avatar", ContactAvatar.uuid().toString());
	removeValue("Contact");
}

bool ContactShared::shouldStore()
{
	ensureLoaded();

	return UuidStorableObject::shouldStore() && !Id.isEmpty() && !ContactAccount.uuid().isNull();
}

void ContactShared::emitUpdated()
{
	emit updated();
}

void ContactShared::setOwnerBuddy(Buddy buddy)
{
	if (OwnerBuddy == buddy)
		return;

	if (!OwnerBuddy.isNull())
		OwnerBuddy.removeContact(this);

	OwnerBuddy = buddy;

	if (!OwnerBuddy.isNull())
		OwnerBuddy.addContact(this);

	dataUpdated();
}

void ContactShared::protocolRegistered(ProtocolFactory *protocolFactory)
{
	if (ContactAccount.protocolName() != protocolFactory->name())
		return;

	if (details())
		return;

	setDetails(protocolFactory->createContactDetails(this));
}

void ContactShared::protocolUnregistered(ProtocolFactory *protocolFactory)
{
	if (ContactAccount.protocolName() != protocolFactory->name())
		return;

	setDetails(0);
}

void ContactShared::detailsAdded()
{
	details()->ensureLoaded();
}

void ContactShared::detailsAboutToBeRemoved()
{
	// do not store contacts that are not in contact manager
	if (ContactManager::instance()->allItems().contains(this))
		details()->store();
}

void ContactShared::setId(const QString &id)
{
	if (Id == id)
		return;

	QString oldId = Id;
	Id = id;

	emit idChanged(oldId);
}