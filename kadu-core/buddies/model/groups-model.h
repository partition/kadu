/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef GROUPS_MODEL_H
#define GROUPS_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "model/kadu-abstract-model.h"

class Group;

class GroupsModel : public QAbstractListModel, public KaduAbstractModel
{
	Q_OBJECT

private slots:
	void groupAboutToBeAdded(Group group);
	void groupAdded(Group group);
	void groupAboutToBeRemoved(Group group);
	void groupRemoved(Group group);

public:
	explicit GroupsModel(QObject *parent = 0);
	virtual ~GroupsModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	Group group(const QModelIndex &index) const;
	int groupIndex(Group group) const;
	virtual QModelIndex indexForValue(const QVariant &value) const;

};

#include "buddies/group.h" // for MOC

#endif // GROUPS_MODEL_H
