/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_MANAGER_H
#define BUDDY_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "buddies/buddy.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-shared.h"
#include "storage/simple-manager.h"

#include "exports.h"

class Account;
class Group;
class XmlConfigFile;

class KADUAPI BuddyManager : public QObject, public SimpleManager<Buddy>
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddyManager)

	static BuddyManager * Instance;

	BuddyManager();
	virtual ~BuddyManager();

	void init();

	void importConfiguration(XmlConfigFile *configurationStorage);

private slots:
	void buddyDataUpdated();
	void groupRemoved(Group group);

protected:
	virtual void load();

	virtual void itemAboutToBeAdded(Buddy buddy);
	virtual void itemAdded(Buddy buddy);
	virtual void itemAboutToBeRemoved(Buddy buddy);
	virtual void itemRemoved(Buddy buddy);

public:
	static BuddyManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Buddies"); }
	virtual QString storageNodeItemName() { return QLatin1String("Buddy"); }

	BuddyList buddies(Account account, bool includeAnonymous = false);
	void mergeBuddies(Buddy destination, Buddy source);

	Buddy byDisplay(const QString &display, bool create = false);
	Buddy byId(Account account, const QString &id, bool create = false);
	Buddy byContact(Contact contact, bool create = false);

signals:
	void buddyAboutToBeAdded(Buddy &buddy);
	void buddyAdded(Buddy &buddy);
	void buddyAboutToBeRemoved(Buddy &buddy);
	void buddyRemoved(Buddy &buddy);

	void buddyUpdated(Buddy &buddy);

};

// for MOC
#include "buddies/group.h"

#endif // BUDDY_MANAGER_H