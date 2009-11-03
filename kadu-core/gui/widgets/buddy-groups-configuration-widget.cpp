/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>

#include "configuration/configuration-contact-account-data-manager.h"
#include "buddies/account-data/contact-account-data.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "misc/misc.h"

#include "buddy-groups-configuration-widget.h"

BuddyGroupsConfigurationWidget::BuddyGroupsConfigurationWidget(Buddy &buddy, QWidget *parent)
		: QScrollArea(parent), MyBuddy(buddy)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

BuddyGroupsConfigurationWidget::~BuddyGroupsConfigurationWidget()
{
}

void BuddyGroupsConfigurationWidget::createGui()
{
	setFrameStyle(QFrame::NoFrame);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QWidget *groupsTab = new QWidget(this);

	QGridLayout *layout = new QGridLayout(groupsTab);
	layout->setColumnMinimumWidth(0, 10);
	layout->setColumnMinimumWidth(1, 10);
	layout->setColumnMinimumWidth(5, 20);
	layout->setColumnStretch(3, 10);
	layout->setColumnStretch(6, 2);

	int row = 0;

	setWidget(groupsTab);
	setWidgetResizable(true);

	QLabel *tabLabel = new QLabel(tr("Groups"), this);
	QFont tabLabelFont = tabLabel->font();
	tabLabelFont.setPointSize(tabLabelFont.pointSize() + 3);
	tabLabelFont.setWeight(QFont::Bold);
	tabLabel->setFont(tabLabelFont);
	layout->addWidget(tabLabel, row++, 1, 1, 4);

	QLabel *tabSubLabel = new QLabel(tr("Add %1 to the groups below by checking the box next to the appropriate groups.").arg(MyBuddy.display()), this);
	layout->setRowStretch(row, 1);
	layout->addWidget(tabSubLabel, row++, 2, 1, 4);

	layout->setRowStretch(row++, 3);

	foreach (Group *group, GroupManager::instance()->groups())
	{
		QCheckBox *groupCheckBox = new QCheckBox(group->name(), this);
		groupCheckBox->setChecked(MyBuddy.isInGroup(group));
		layout->addWidget(groupCheckBox, row++, 2, 1, 2);
		GroupCheckBoxList.append(groupCheckBox);
	}

	layout->setRowStretch(row, 100);
}

void BuddyGroupsConfigurationWidget::saveConfiguration()
{
	foreach (Group *group, MyBuddy.groups())
	{
		MyBuddy.removeFromGroup(group);
	}
	foreach (QCheckBox *groupBox, GroupCheckBoxList)
	{
		if (groupBox->isChecked())
			MyBuddy.addToGroup(GroupManager::instance()->byName(groupBox->text()));
	}
}
