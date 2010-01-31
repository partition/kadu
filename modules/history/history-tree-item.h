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

#ifndef HISTORY_TREE_ITEM_H
#define HISTORY_TREE_ITEM_H

#include <QtCore/QVariant>

#include "model/history-type.h"

class Buddy;
class Chat;

class HistoryTreeItem
{
	HistoryType Type;
	Chat ItemChat;
	Buddy ItemBuddy;

public:
	HistoryTreeItem();
	HistoryTreeItem(const HistoryTreeItem &copyMe);

	explicit HistoryTreeItem(Chat chat);
	explicit HistoryTreeItem(Buddy buddy);

	HistoryTreeItem operator = (HistoryTreeItem &copyMe);

	HistoryType type();
	Chat chat();
	Buddy buddy();

};

Q_DECLARE_METATYPE(HistoryTreeItem)

#endif // HISTORY_TREE_ITEM_H