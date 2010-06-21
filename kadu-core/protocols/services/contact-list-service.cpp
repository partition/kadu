/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "gui/windows/message-dialog.h"
#include "protocols/protocol.h"
#include "debug.h"

#include "contact-list-service.h"

ContactListService::ContactListService(Protocol *protocol) :
		QObject(protocol), CurrentProtocol(protocol)
{
}

ContactListService::~ContactListService()
{
}

Buddy ContactListService::mergeBuddy(Buddy oneBuddy)
{
	Buddy buddy;
	QList<Contact> oneBuddyContacts = oneBuddy.contacts(CurrentProtocol->account());

	if (oneBuddyContacts.count() > 0)
	{
		foreach (const Contact &contact, oneBuddyContacts)
		{
			Contact contactOnList = ContactManager::instance()->byId(CurrentProtocol->account(), contact.id(), ActionReturnNull);
			if (!contactOnList) // not on list add this one as new
			{
				buddy = Buddy::create();
				// move contact to buddy
				ContactManager::instance()->addItem(contact);
				kdebugmf(KDEBUG_FUNCTION_START, "\nuuid add: '%s' %s\n",
					 qPrintable(contactOnList.uuid().toString()), qPrintable(buddy.display()));
				contact.setOwnerBuddy(buddy);
			}
			else // already on list
			{
				// found contact so use his buddy
				//kdebugmf(KDEBUG_FUNCTION_START, "\nuuid before: '%s'\n", qPrintable(contactOnList.ownerBuddy().uuid().toString()));
				buddy = contactOnList.ownerBuddy();
				kdebugmf(KDEBUG_FUNCTION_START, "\nuuid owner: '%s' %s\n",
					 qPrintable(contactOnList.uuid().toString()), qPrintable(buddy.display()));
				//unImportedContacts.removeOne(contactOnList);
			}
		}
	}
	else
	{
		// THIS WORKS NICE
		// find one by display, but what if display().isEmpty()?
		buddy = BuddyManager::instance()->byDisplay(oneBuddy.display(), ActionCreateAndAdd);
		if (buddy.isNull())
		{
			// not found so add new one
			buddy = Buddy::create();
			// TODO: 0.6.6
			// maybe just ignore?
			buddy.setDisplay(buddy.uuid().toString());
		}
	}

	// TODO 0.6.6: update rest data, consider to add some logic here
	// TODO 0.6.6: consider to find contact by some data if no contacts inside buddy
	buddy.setFirstName(oneBuddy.firstName());
	buddy.setLastName(oneBuddy.lastName());
	buddy.setNickName(oneBuddy.nickName());
	buddy.setMobile(oneBuddy.mobile());
	buddy.setGroups(oneBuddy.groups());
	buddy.setEmail(oneBuddy.email());
	buddy.setDisplay(oneBuddy.display());
	buddy.setHomePhone(oneBuddy.homePhone());
	buddy.setAnonymous(false);

	BuddyManager::instance()->addItem(buddy);

	return buddy;
}

void ContactListService::setBuddiesList(BuddyList buddies)
{
	QList<Contact> unImportedContacts = ContactManager::instance()->contacts(CurrentProtocol->account());

	foreach (const Buddy &oneBuddy, buddies)
	{
		Buddy mergedBuddy = mergeBuddy(oneBuddy);
		foreach (const Contact &contact, mergedBuddy.contacts(CurrentProtocol->account()))
			unImportedContacts.removeAll(contact);
	}

	if (!unImportedContacts.isEmpty())
	{
		// create names list
		QStringList contactsList;
		foreach (const Contact &c, unImportedContacts)
		{
			QString display = c.ownerBuddy().display();
			if (!contactsList.contains(display))
				contactsList.append(display);
		}

		if (MessageDialog::ask(tr("Following contacts from your list were not found on server: %0.\nDo you want to remove them from contacts list?").arg(contactsList.join(", "))))
			foreach (const Contact &c, unImportedContacts)
				ContactManager::instance()->removeItem(c);
	}

	MessageDialog::msg(tr("Your contact list has been successfully imported from server"), false, "Infromation");

	// flush configuration to save all changes
	ConfigurationManager::instance()->flush();
}