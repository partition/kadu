/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUPS_COMBO_BOX_H
#define GROUPS_COMBO_BOX_H

#include <QtGui/QComboBox>

#include "buddies/group.h"

class AbstractGroupFilter;
class GroupsModel;
class QSortFilterProxyModel;
class ActionsProxyModel;

class GroupsComboBox : public QComboBox
{
	Q_OBJECT

	GroupsModel *Model;
	QSortFilterProxyModel *ProxyModel;
	ActionsProxyModel *ActionsModel;

	Group CurrentGroup;

private slots:
	void activatedSlot(int index);
	void resetGroup();

public:
	explicit GroupsComboBox(bool includeSelectGroup = false, QWidget *parent = 0);
	virtual ~GroupsComboBox();

	void setCurrentGroup(Group group);
	Group currentGroup();

};

#endif // GROUPS_COMBO_BOX_H
