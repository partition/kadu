/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact-manager.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"

#include "buddy-contacts-table-item.h"

BuddyContactsTableItem::BuddyContactsTableItem(Contact contact, QObject *parent) :
		QObject(parent)
{
	ItemContact = contact;
	ItemContactPriority = contact.priority();
	ItemAccount = contact.contactAccount();
	Id = contact.id();
	Action = ItemEdit;
}

void BuddyContactsTableItem::setItemContact(Contact contact)
{
	if (ItemContact != contact)
	{
		ItemContact = contact;
		emit updated(this);
	}
}

void BuddyContactsTableItem::setItemContactPriority(int itemContactPriority)
{
	if (ItemContactPriority != itemContactPriority)
	{
		ItemContactPriority = itemContactPriority;
		emit updated(this);
	}
}

void BuddyContactsTableItem::setItemAccount(Account account)
{
	if (ItemAccount != account)
	{
		ItemAccount = account;
		emit updated(this);
	}
}

void BuddyContactsTableItem::setId(const QString& id)
{
	if (Id != id)
	{
		Id = id;
		emit updated(this);
	}
}

void BuddyContactsTableItem::setAction(BuddyContactsTableItem::ItemAction action)
{
	if (Action != action)
	{
		Action = action;
		emit updated(this);
	}
}

void BuddyContactsTableItem::setDetachedBuddyName(const QString &detachedBuddyName)
{
	if (DetachedBuddyName != detachedBuddyName)
	{
		DetachedBuddyName = detachedBuddyName;
		emit updated(this);
	}
}

bool BuddyContactsTableItem::isValid() const
{
	if (ItemRemove == Action)
		return true;

	if (ItemDetach == Action)
		return !DetachedBuddyName.isEmpty();

	if (ItemAdd == Action)
		return isAddValid();

	return isEditValid();
}

bool BuddyContactsTableItem::isAddValid() const
{
	if (ItemContact)
		return false;

	if (!ItemAccount)
		return false;

	if (Id.isEmpty())
		return false;

	Protocol *handler = ItemAccount.protocolHandler();
	if (!handler)
		return true;

	if (!handler->validateUserID(Id))
		return false;

	// allow contacts without buddy or new ones
	Contact contact = ContactManager::instance()->byId(ItemAccount, Id, false);
	return !contact.ownerBuddy();
}

bool BuddyContactsTableItem::isEditValid() const
{
	if (!ItemContact)
		return false;

	if (!ItemAccount)
		return false;

	if (Id.isEmpty())
		return false;

	Protocol *handler = ItemAccount.protocolHandler();
	if (!handler)
		return true;

	if (!handler->validateUserID(Id))
		return false;

	if (ItemAccount != ItemContact.contactAccount() || Id != ItemContact.id())
	{
		// allow contacts without buddy or new ones
		Contact contact = ContactManager::instance()->byId(ItemAccount, Id, false);
		return !contact.ownerBuddy();
	}

	return true;
}
