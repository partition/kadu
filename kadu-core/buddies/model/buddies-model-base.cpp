 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/avatar.h"
#include "buddies/avatar-manager.h"
#include "buddies/buddy.h"
#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact.h"
#include "model/roles.h"
#include "protocols/protocol.h"

#include "buddies-model-base.h"

BuddiesModelBase::BuddiesModelBase(QObject *parent) :
		QAbstractItemModel(parent)
{
}

BuddiesModelBase::~BuddiesModelBase()
{
	triggerAllAccountsUnregistered();
}

void BuddiesModelBase::accountRegistered(Account account)
{
	connect(account.data(), SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SLOT(buddyStatusChanged(Account, Buddy, Status)));
}

void BuddiesModelBase::accountUnregistered(Account account)
{
	disconnect(account.data(), SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SLOT(buddyStatusChanged(Account, Buddy, Status)));
}

void BuddiesModelBase::buddyStatusChanged(Account account, Buddy buddy, Status oldStatus)
{
	QModelIndex index = buddyIndex(buddy);

	if (index.isValid())
		emit dataChanged(index, index);
}

QModelIndex BuddiesModelBase::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column, parent.isValid() ? parent.row() : -1);
}

int BuddiesModelBase::columnCount(const QModelIndex &parent) const
{
	return 1;
}

int BuddiesModelBase::rowCount(const QModelIndex &parentIndex) const
{
	if (!parentIndex.isValid() || parent(parentIndex).isValid())
		return 0;

	Buddy con = buddyAt(parentIndex);
	return con.contacts().size();
}

QFlags<Qt::ItemFlag> BuddiesModelBase::flags(const QModelIndex& index) const
{
	if (index.isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
	else
		return QAbstractItemModel::flags(index);
}

QModelIndex BuddiesModelBase::parent(const QModelIndex &child) const
{
	if (-1 == child.internalId())
		return QModelIndex();
	else
		return index(child.internalId(), 0, QModelIndex());
}

QVariant BuddiesModelBase::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

Contact BuddiesModelBase::buddyDefaultAccountData(const QModelIndex &index) const
{
	Buddy buddy = buddyAt(index);
	if (buddy.isNull())
		return Contact::null;

	Account account = buddy.prefferedAccount();
	if (account.isNull())
		account = AccountManager::instance()->defaultAccount();
	
	return buddy.contact(account);
}

Contact BuddiesModelBase::buddyAccountData(const QModelIndex &index, int accountIndex) const
{
	Buddy buddy = buddyAt(index);
	if (buddy.isNull())
		return Contact::null;

	QList<Contact> contacts = buddy.contacts();
	if (contacts.size() <= accountIndex)
		return Contact::null;

	return contacts[accountIndex];
}

QVariant BuddiesModelBase::data(Buddy buddy, int role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return buddy.display();
		case BuddyRole:
			return QVariant::fromValue(buddy);
		case StatusRole:
			return QVariant::fromValue(Status::null);
		default:
			return QVariant();
	}
}

QVariant BuddiesModelBase::data(Contact contact, int role, bool useDisplay) const
{
	if (contact.isNull())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return useDisplay
					? contact.ownerBuddy().display()
					: QString("%1: %2").arg(contact.contactAccount().name()).arg(contact.id());
		case Qt::DecorationRole:
			if (contact.isNull())
				return QVariant();
			// TODO generic icon
			return !contact.contactAccount().isNull()
					? contact.contactAccount().statusContainer()->statusPixmap(contact.currentStatus())
					: QVariant();
		case BuddyRole:
			return QVariant::fromValue(contact.ownerBuddy());
		case DescriptionRole:
			//TODO 0.6.6:
			//	ContactKaduData *ckd = contact.moduleData<ContactKaduData>(true);
			//	if (!ckd)
			//		return QString::null;
			//	if (ckd->hideDescription())
			//	{
				//		delete ckd;
				//		return QString::null;
				//	}
				//	delete ckd;
				//
				return contact.currentStatus().description();
		case StatusRole:
			return QVariant::fromValue(contact.currentStatus());
		case AccountRole:
			return QVariant::fromValue(contact.contactAccount());
		case AvatarRole:
			// TODO: 0.6.6 move it
			if (contact.contactAvatar().pixmap().isNull())
				AvatarManager::instance()->updateAvatar(contact);
			return QVariant::fromValue(contact.contactAvatar().pixmap());
		default:
			return QVariant();
	}
}

QVariant BuddiesModelBase::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	QModelIndex parentIndex = parent(index);
	if (!parentIndex.isValid())
	{
		Contact contact = buddyDefaultAccountData(index);
		return !contact.isNull() ? data(contact, role, true) : data(buddyAt(index), role);
	}
	else
		return data(buddyAccountData(parentIndex, index.row()), role, false);
}

// D&D

QStringList BuddiesModelBase::mimeTypes() const
{
	return BuddyListMimeDataHelper::mimeTypes();
}

QMimeData * BuddiesModelBase::mimeData(const QModelIndexList &indexes) const
{
	BuddyList list;
	foreach (QModelIndex index, indexes)
	{
		QVariant conVariant = index.data(BuddyRole);;
		if (!conVariant.canConvert<Buddy>())
			continue;
		Buddy con = conVariant.value<Buddy>();
		if (con.isNull())
			continue;
		list << con;
	}

	return BuddyListMimeDataHelper::toMimeData(list);
}