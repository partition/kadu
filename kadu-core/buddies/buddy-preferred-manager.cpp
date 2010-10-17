/*
 * %kadu copyright begin%
 * Copyright 2010, Piotr Dąbrowski (ultr@ultr.pl)
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

#include "buddies/buddy.h"
#include "chat/message/pending-messages-manager.h"
#include "chat/recent-chat-manager.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget-manager.h"

#include "buddies/buddy-preferred-manager.h"

BuddyPreferredManager *BuddyPreferredManager::Instance = 0;

BuddyPreferredManager *BuddyPreferredManager::instance()
{
	if (0 == Instance)
	{
		Instance = new BuddyPreferredManager();
	}
	return Instance;
}

BuddyPreferredManager::BuddyPreferredManager()
{
}

BuddyPreferredManager::~BuddyPreferredManager()
{
}

Contact BuddyPreferredManager::preferredContact(Buddy buddy, Account account, bool includechats)
{
	if (!buddy || buddy.contacts().isEmpty())
		return Contact::null;

	if (!includechats)
		return preferredContactByStatus(buddy, account);

	if (!account)
	{
		if (Preferreds.contains(buddy))
			return Preferreds.value(buddy);
		return preferredContactByStatus(buddy);
	}

	Contact contact;
	contact = preferredContactByPendingMessages(buddy, account);
	if (contact)
		return contact;
	contact = preferredContactByChatWidgets(buddy, account);
	if (contact)
		return contact;
	contact = preferredContactByRecentChats(buddy, account);
	if (contact)
		return contact;
	contact = preferredContactByStatus(buddy, account);
	return contact;
}

Contact BuddyPreferredManager::preferredContact(Buddy buddy, bool includechats)
{
	return BuddyPreferredManager::preferredContact(buddy, Account::null, includechats);
}

Account BuddyPreferredManager::preferredAccount(Buddy buddy, bool includechats)
{
	Contact contact = BuddyPreferredManager::preferredContact(buddy, includechats);
	return contact.contactAccount();
}

void BuddyPreferredManager::updatePreferred(Buddy buddy)
{
	Contact contact;
	contact = preferredContactByPendingMessages(buddy);
	if (!contact)
	contact = preferredContactByChatWidgets(buddy);
	if (!contact)
	contact = preferredContactByRecentChats(buddy);

	if (contact)
		Preferreds.insert(buddy, contact);
	else
		Preferreds.remove(buddy);

	emit buddyUpdated(buddy);
}

Contact BuddyPreferredManager::preferredContactByPendingMessages(Buddy buddy, Account account)
{
	Contact result;
	foreach (Message message, PendingMessagesManager::instance()->pendingMessagesForBuddy(buddy))
	{
		Contact contact = message.messageSender();
		result = morePreferredContactByStatus(result, contact, account);
	}
	return result;
}

Contact BuddyPreferredManager::preferredContactByChatWidgets(Buddy buddy, Account account)
{
	Contact result;
	foreach (ChatWidget *chatwidget, ChatWidgetManager::instance()->chats())
	{
		Chat chat = chatwidget->chat();
		if (chat.contacts().isEmpty())
			continue;
		Contact contact = *chat.contacts().begin();
		if (contact.ownerBuddy() != buddy)
			continue;
		result = morePreferredContactByStatus(result, contact, account);
	}
	return result;
}

Contact BuddyPreferredManager::preferredContactByRecentChats(Buddy buddy, Account account)
{
	Contact result;
	foreach (Chat chat, RecentChatManager::instance()->recentChats())
	{
		if (chat.contacts().isEmpty())
			continue;
		Contact contact = *chat.contacts().begin();
		if (contact.ownerBuddy() != buddy)
			continue;
		result = morePreferredContactByStatus(result, contact, account);
	}
	return result;
}

Contact BuddyPreferredManager::preferredContactByStatus(Buddy buddy, Account account)
{
	Contact result;
	foreach (const Contact &contact, buddy.contacts())
		result = morePreferredContactByStatus(result, contact, account);
	return result;
}

Contact BuddyPreferredManager::morePreferredContactByStatus(Contact c1, Contact c2, Account account)
{
	if (account)
	{
		if (c1.contactAccount() != account)
			c1 = Contact::null;
		if (c2.contactAccount() != account)
			c2 = Contact::null;
	}
	if (!c1)
		return c2;
	if (!c2)
		return c1;
	return Contact::contactWithHigherStatus(c1, c2);
}