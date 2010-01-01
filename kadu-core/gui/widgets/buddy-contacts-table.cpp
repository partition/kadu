/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QTableView>

#include "gui/widgets/buddy-contacts-table-delegate.h"
#include "gui/widgets/buddy-contacts-table-item.h"
#include "gui/widgets/buddy-contacts-table-model.h"
#include "gui/widgets/buddy-contacts-table-model-proxy.h"
#include "model/roles.h"

#include "buddy-contacts-table.h"
#include <gui/windows/message-dialog.h>
#include <QInputDialog>

BuddyContactsTable::BuddyContactsTable(Buddy buddy, QWidget *parent) :
		QWidget(parent), MyBuddy(buddy)
{
	Delegate = new BuddyContactsTableDelegate(this);
	Model = new BuddyContactsTableModel(buddy, this);
	Proxy = new BuddyContactsTableModelProxy(Model);
	Proxy->setSourceModel(Model);

	connect(Model, SIGNAL(validChanged()), this, SIGNAL(validChanged()));
	connect(Model, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SIGNAL(validChanged()));
	connect(Model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SIGNAL(validChanged()));

	createGui();
}

BuddyContactsTable::~BuddyContactsTable()
{
}

void BuddyContactsTable::createGui()
{
	QHBoxLayout *layout = new QHBoxLayout(this);

	View = new QTableView(this);
	View->setAlternatingRowColors(true);
	View->setDragEnabled(true);
	View->setEditTriggers(QAbstractItemView::AllEditTriggers);
	View->setItemDelegate(Delegate);
	View->setModel(Proxy);

	View->setSelectionBehavior(QAbstractItemView::SelectRows);
	View->setVerticalHeader(0);

	View->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	View->horizontalHeader()->setStretchLastSection(true);

	layout->addWidget(View);

	QWidget *buttons = new QWidget(View);
	QVBoxLayout *buttonsLayout = new QVBoxLayout(buttons);

	QPushButton *moveUpButton = new QPushButton(tr("Move up"), buttons);
	connect(moveUpButton, SIGNAL(clicked(bool)), this, SLOT(moveUpClicked()));
	buttonsLayout->addWidget(moveUpButton);

	QPushButton *moveDownButton = new QPushButton(tr("Move down"), buttons);
	connect(moveDownButton, SIGNAL(clicked(bool)), this, SLOT(moveDownClicked()));
	buttonsLayout->addWidget(moveDownButton);

	QPushButton *addContactButton = new QPushButton(tr("Add contact"), buttons);
	connect(addContactButton, SIGNAL(clicked(bool)), this, SLOT(addClicked()));
	buttonsLayout->addWidget(addContactButton);

	QPushButton *detachContactButton = new QPushButton(tr("Detach contact"), buttons);
	connect(detachContactButton, SIGNAL(clicked(bool)), this, SLOT(detachClicked()));
	buttonsLayout->addWidget(detachContactButton);

	QPushButton *removeContactButton = new QPushButton(tr("Remove contact"), buttons);
	connect(removeContactButton, SIGNAL(clicked(bool)), this, SLOT(removeClicked()));
	buttonsLayout->addWidget(removeContactButton);

	layout->addWidget(buttons);
}

bool BuddyContactsTable::isValid()
{
	return Model->isValid();
}

void BuddyContactsTable::save()
{
	Model->save();
}

void BuddyContactsTable::moveUpClicked()
{
	QModelIndex currentItem = View->currentIndex();
	QModelIndex previousItem = currentItem.sibling(currentItem.row() - 1, currentItem.column());
	if (!previousItem.isValid())
		return;

	BuddyContactsTableItem *current = qvariant_cast<BuddyContactsTableItem *>(currentItem.data(BuddyContactsTableItemRole));
	BuddyContactsTableItem *previous = qvariant_cast<BuddyContactsTableItem *>(previousItem.data(BuddyContactsTableItemRole));

	if (!current || !previous)
		return;

	int priority = current->itemContactPriority();
	current->setItemContactPriority(previous->itemContactPriority());
	previous->setItemContactPriority(priority);
}

void BuddyContactsTable::moveDownClicked()
{
	QModelIndex currentItem = View->currentIndex();
	QModelIndex nextItem = currentItem.sibling(currentItem.row() + 1, currentItem.column());
	if (!nextItem.isValid())
		return;
	
	BuddyContactsTableItem *current = qvariant_cast<BuddyContactsTableItem *>(currentItem.data(BuddyContactsTableItemRole));
	BuddyContactsTableItem *next = qvariant_cast<BuddyContactsTableItem *>(nextItem.data(BuddyContactsTableItemRole));
	
	if (!current || !next)
		return;
	
	int priority = current->itemContactPriority();
	current->setItemContactPriority(next->itemContactPriority());
	next->setItemContactPriority(priority);
}

void BuddyContactsTable::addClicked()
{
	Model->insertRow(Model->rowCount());
}

void BuddyContactsTable::detachClicked()
{
	QVariant selected = View->currentIndex().data(BuddyContactsTableItemRole);
	if (!selected.canConvert<BuddyContactsTableItem *>())
		return;

	BuddyContactsTableItem *item = qvariant_cast<BuddyContactsTableItem *>(selected);
	if (!item)
		return;

	QString display = QString("%1 (%2)").arg(item->id()).arg(item->itemAccount().name());
	display = QInputDialog::getText(this, tr("New buddy display name"),
			tr("Give name for new buddy for this contact"), QLineEdit::Normal, display);

	if (display.isEmpty())
		return;

	item->setAction(BuddyContactsTableItem::ItemDetach);
	item->setDetachedBuddyName(display);
}

void BuddyContactsTable::removeClicked()
{
	QVariant selected = View->currentIndex().data(BuddyContactsTableItemRole);
	if (!selected.canConvert<BuddyContactsTableItem *>())
		return;

	BuddyContactsTableItem *item = qvariant_cast<BuddyContactsTableItem *>(selected);
	if (!item)
		return;

	bool sure = MessageDialog::ask(tr("Are you sure do you want to delete this contact from buddy <b>%1</b>?").arg(MyBuddy.display()),
			"Warning", this);
	if (!sure)
		return;

	if (item->action() == BuddyContactsTableItem::ItemAdd)
		// remove it, we don't need it anyway
		Model->removeRow(View->currentIndex().row());
	else
		item->setAction(BuddyContactsTableItem::ItemRemove);
}