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

#ifndef BUDDY_CONTACTS_TABLE_MODEL_PROXY_H
#define BUDDY_CONTACTS_TABLE_MODEL_PROXY_H

#include <QtGui/QSortFilterProxyModel>

class BuddyContactsTableModelProxy : public QSortFilterProxyModel
{
	Q_OBJECT

protected:
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

public:
	explicit BuddyContactsTableModelProxy(QObject *parent = 0);
	virtual ~BuddyContactsTableModelProxy();

};

#endif // BUDDY_CONTACTS_TABLE_MODEL_PROXY_H
