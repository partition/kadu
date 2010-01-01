 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_CONTACT_MODEL_H
#define BUDDY_CONTACT_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "buddies/buddy.h"

class Account;
class Contact;

class BuddyContactModel : public QAbstractListModel
{
	Q_OBJECT

	Buddy SourceBuddy;

private slots:
	void contactAboutToBeAdded(Contact data);
	void contactAdded(Contact data);
	void contactAboutToBeRemoved(Contact data);
	void contactRemoved(Contact data);

public:
	explicit BuddyContactModel(Buddy contact, QObject *parent = 0);
	virtual ~BuddyContactModel();

	virtual int columnCount(const QModelIndex &parent) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	Contact contact(const QModelIndex &index) const;
	int contactIndex(Contact data);
	QModelIndex contactModelIndex(Contact data);

};

// for MOC
#include "contacts/contact.h"

#endif // BUDDY_CONTACT_MODEL_H