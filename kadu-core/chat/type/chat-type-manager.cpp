/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/type/chat-type-aware-object.h"
#include "chat/type/chat-type-conference.h"
#include "chat/type/chat-type-simple.h"
#include "icons-manager.h"

#include "chat-type-manager.h"

ChatTypeManager * ChatTypeManager::Instance = 0;

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns singleton instance of ChatTypeManager.
 * @return singleton instance of ChatTypeManager
 */
ChatTypeManager * ChatTypeManager::instance()
{
	if (0 == Instance)
		Instance = new ChatTypeManager();

	return Instance;
}

ChatTypeManager::ChatTypeManager()
{
	addChatType(new ChatTypeSimple());
	addChatType(new ChatTypeConference());
}

ChatTypeManager::~ChatTypeManager()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Adds new chat type to manager.
 * @param chatType chat type to add.
 *
 * Adds new chat type to manager. After that all @link ChatTypeAwareObject @endlink
 * gets their chatTypeRegistered methods called.
 */
void ChatTypeManager::addChatType(ChatType *chatType)
{
	if (ChatTypes.contains(chatType))
		return;

	emit chatTypeAboutToBeAdded(chatType);
	ChatTypes.append(chatType);
	ChatTypesMap.insert(chatType->name(), chatType);
	emit chatTypeAdded(chatType);

	ChatTypeAwareObject::notifyChatTypeRegistered(chatType);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Removes chat type from manager.
 * @param chatType chat type to removed.
 *
 * Removes chat type from manager. After that all @link ChatTypeAwareObject @endlink
 * gets their chatTypeUnregistered methods called.
 */
void ChatTypeManager::removeChatType(ChatType *chatType)
{
	if (!ChatTypes.contains(chatType))
		return;

	emit chatTypeAboutToBeRemoved(chatType);
	ChatTypes.removeAll(chatType);
	ChatTypesMap.remove(chatType->name());
	emit chatTypeRemoved(chatType);

	ChatTypeAwareObject::notifyChatTypeUnregistered(chatType);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns list of all registered chat types.
 * @return list of all registered chat types
 *
 * Returns list of all registered chat types.
 */
const QList<ChatType *> & ChatTypeManager::chatTypes() const
{
	return ChatTypes;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns chat type with given internal name.
 * @param name internal name of chat type to return.
 * @return chat type with given internal name
 *
 * Returns chat type with given internal name or null, if not found.
 */
ChatType * ChatTypeManager::chatType(const QString &name)
{
	return ChatTypesMap.value(name);
}
