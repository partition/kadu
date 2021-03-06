/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-type.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates empty ChatType object.
 * @param parent parent QObject
 *
 * Creates empty ChatType.
 */
ChatType::ChatType(QObject *parent) :
		QObject(parent)
{
}

ChatType::~ChatType()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Checks if two ChatType objects are identical.
 * @param compare object to compare with
 *
 * Returns true only if these two object have identical names.
 */
bool ChatType::operator == (const ChatType &compare) const
{
	return name() == compare.name();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Compares two ChatType object for sorting.
 * @param compare object to compare with
 *
 * Returns true only if this object has smaller sortIndex that compare object.
 */
bool ChatType::operator < (const ChatType &compare) const
{
	return sortIndex() < compare.sortIndex();
}
