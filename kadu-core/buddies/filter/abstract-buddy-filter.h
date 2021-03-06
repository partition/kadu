/*
 * %kadu copyright begin%
 * Copyright 2011 Dariusz Markowicz (darom@alari.pl)
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef ABSTRACT_BUDDY_FILTER_H
#define ABSTRACT_BUDDY_FILTER_H

#include <QtCore/QObject>

#include "exports.h"

class Buddy;

class KADUAPI AbstractBuddyFilter : public QObject
{
	Q_OBJECT

public:
	explicit AbstractBuddyFilter(QObject *parent = 0)
			: QObject(parent) {}
	virtual ~AbstractBuddyFilter() {}

	virtual bool acceptBuddy(const Buddy &buddy) = 0;
	virtual bool ignoreNextFilters(const Buddy &buddy)
	{
		Q_UNUSED(buddy);
		return false;
	}

signals:
	void filterChanged();

};

#endif // ABSTRACT_BUDDY_FILTER_H
