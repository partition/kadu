 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_LIST_MODEL_H
#define BUDDY_LIST_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "accounts/accounts-aware-object.h"

#include "buddies/buddy.h"
#include "buddies/buddy-list.h"

#include "status/status.h"

#include "buddies-model-base.h"

class BuddyListModel : public BuddiesModelBase
{
	Q_OBJECT

	BuddyList List;

public:
	explicit BuddyListModel(BuddyList list, QObject *parent = 0);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	// AbstractContactsModel implementation
	virtual Buddy buddyAt(const QModelIndex& index) const;
	virtual const QModelIndex buddyIndex(Buddy buddy) const;

};

#endif // BUDDY_LIST_MODEL_H