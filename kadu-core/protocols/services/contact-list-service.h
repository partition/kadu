/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef CONTAT_LIST_SERVICE_H
#define CONTAT_LIST_SERVICE_H

#include <QtCore/QObject>

#include "buddies/buddy-list.h"
#include "exports.h"

class Protocol;

class KADUAPI ContactListService : public QObject
{
	Q_OBJECT

	Protocol *CurrentProtocol;

	// TODO: rename
	Buddy mergeBuddy(Buddy buddy);

public:
	explicit ContactListService(Protocol *protocol = 0);
	virtual ~ContactListService();

	virtual void importContactList(bool automaticallySetBuddiesList = true);
	virtual void exportContactList() = 0;
	virtual void exportContactList(const BuddyList &buddies) = 0;

	virtual QList<Buddy> loadBuddyList(QTextStream &dataStream) = 0;
	virtual QByteArray storeBuddyList(const BuddyList &buddies) = 0;

	void setBuddiesList(const BuddyList &buddies, bool removeOld = true);

public slots:
	virtual void contactListImportedSlot(bool ok, const BuddyList &buddies);

signals:
	void contactListImported(bool ok, const BuddyList &buddies);
	void contactListExported(bool ok);

};

#endif // CONTAT_LIST_SERVICE_H
