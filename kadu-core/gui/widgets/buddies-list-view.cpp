/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QModelIndex>
#include <QtCore/QTemporaryFile>
#include <QtGui/QApplication>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QImage>
#include <QtGui/QMenu>
#include <QtGui/QSortFilterProxyModel>

#include "accounts/account.h"
#include "buddies/buddy.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/model/buddies-model-proxy.h"
#include "chat/chat-manager.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"
#include "protocols/protocols-manager.h"

#include "icons-manager.h"

#include "buddies-list-view-delegate.h"
#include "buddies-list-view-menu-manager.h"

#include "buddies-list-view.h"
#include "tool-tip-class-manager.h"

BuddiesListView::BuddiesListView(MainWindow *mainWindow, QWidget *parent) :
		QTreeView(parent), MyMainWindow(mainWindow), ProxyModel(new BuddiesModelProxy(this)),
		Delegate(0), BackgroundTemporaryFile(0)
{
	setAlternatingRowColors(true);
	setAnimated(BackgroundImageMode == BackgroundNone);
	setDragEnabled(true);
	setExpandsOnDoubleClick(false);
	setHeaderHidden(true);
	setItemsExpandable(true);
	setRootIsDecorated(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setUniformRowHeights(false);
	setWordWrap(true);
	setMouseTracking(true);

	Delegate = new BuddiesListViewDelegate(this);
	setItemDelegate(Delegate);

	Delegate->setModel(ProxyModel);
	QTreeView::setModel(ProxyModel);

	connect(&ToolTipTimeoutTimer, SIGNAL(timeout()), this, SLOT(toolTipTimeout()));
	connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedSlot(const QModelIndex &)));
}

BuddiesListView::~BuddiesListView()
{
	if (ProxyModel->sourceModel())
	{
		delete ProxyModel->sourceModel();
		ProxyModel->setSourceModel(0);
	}

	if (Delegate)
	{
		delete Delegate;
		Delegate = 0;
	}
}

void BuddiesListView::setModel(AbstractBuddiesModel *model)
{
	Model = model;

	if (ProxyModel->sourceModel())
	{
		delete ProxyModel->sourceModel();
		ProxyModel->setSourceModel(0);
	}

	ProxyModel->setSourceModel(dynamic_cast<QAbstractItemModel *>(model));
	ProxyModel->sort(1);
	ProxyModel->sort(0); // something is wrong with sorting in my Qt version
}

void BuddiesListView::addFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void BuddiesListView::removeFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->removeFilter(filter);
}

void BuddiesListView::selectBuddy(Buddy buddy)
{
	QModelIndex index = Model->buddyIndex(buddy);
	index = ProxyModel->mapFromSource(index);

	setCurrentIndex(index);
}

Contact BuddiesListView::currentContact() const
{
	return contactAt(currentIndex());
}

ContactSet BuddiesListView::selectedContacts() const
{
	ContactSet result;

	QModelIndexList selectionList = selectedIndexes();
	foreach (QModelIndex selection, selectionList)
	{
		Contact contact = contactAt(selection);
		if (contact)
			result.insert(contact);
	}

	return result;
}

Buddy BuddiesListView::currentBuddy() const
{
	return buddyAt(currentIndex());
}

BuddySet BuddiesListView::selectedBuddies() const
{
	BuddySet result;

	QModelIndexList selectionList = selectedIndexes();
	foreach (QModelIndex selection, selectionList)
	{
		Buddy buddy = buddyAt(selection);
		if (buddy)
			result.insert(buddy);
	}

	return result;
}

Buddy BuddiesListView::buddyAt(const QModelIndex &index) const
{
	const AbstractBuddiesModel *model = dynamic_cast<const AbstractBuddiesModel *>(index.model());
	if (!model)
		return Buddy::null;

	return model->buddyAt(index);
}

Contact BuddiesListView::contactAt(const QModelIndex &index) const
{
	const AbstractBuddiesModel *model = dynamic_cast<const AbstractBuddiesModel *>(index.model());
	if (!model)
		return Contact::null;

	return model->contactAt(index);
}

Chat BuddiesListView::chatForIndex(const QModelIndex &index) const
{
	if (!index.isValid())
		return Chat::null;

	Contact con = contactAt(index);
	if (con.isNull())
		return Chat::null;

	return ChatManager::instance()->findChat(ContactSet(con));
}

Chat  BuddiesListView::currentChat() const
{
	return ChatManager::instance()->findChat(selectedContacts());
}

void BuddiesListView::triggerActivate(const QModelIndex& index)
{
	Chat chat = currentChat();
	if (chat)
		emit chatActivated(chat);

	Buddy buddy = buddyAt(index);
	if (buddy)
		emit buddyActivated(buddy);
}

void BuddiesListView::contextMenuEvent(QContextMenuEvent *event)
{
	if (!MyMainWindow)
		return;

	Buddy con = buddyAt(indexAt(event->pos()));
	if (con.isNull())
		return;

	bool first = true;
	QMenu *menu = new QMenu(this);

	QMenu *actions = new QMenu(tr("Actions"));
	foreach (ActionDescription *actionDescription, BuddiesListViewMenuManager::instance()->buddyListActions())
		if (actionDescription)
		{
			Action *action = actionDescription->createAction(MyMainWindow);
			actions->addAction(action);
			action->checkState();
		}
		else
			actions->addSeparator();

	foreach (ActionDescription *actionDescription, BuddiesListViewMenuManager::instance()->buddiesContexMenu())
	{
		if (actionDescription)
		{

			Action *action = actionDescription->createAction(MyMainWindow);
			menu->addAction(action);
			action->checkState();
		}
		else
		{
			menu->addSeparator();
			if (first)
			{
				menu->addMenu(actions);
				first = false;
			}
		}
	}

	QMenu *management = menu->addMenu(tr("Buddy Options"));

	foreach (ActionDescription *actionDescription, BuddiesListViewMenuManager::instance()->managementActions())
		if (actionDescription)
		{
			Action *action = actionDescription->createAction(MyMainWindow);
			management->addAction(action);
			action->checkState();
		}
		else
			management->addSeparator();

	foreach (Contact contact, con.contacts())
	{
		if (!contact.contactAccount() || !contact.contactAccount().protocolHandler())
			continue;

		Account account = contact.contactAccount();
		ProtocolFactory *protocolFactory = account.protocolHandler()->protocolFactory();

		if (!account.protocolHandler()->protocolFactory() || !protocolFactory->protocolMenuManager())
			continue;

		QMenu *account_menu = menu->addMenu(account.name());
		if (!protocolFactory->iconName().isEmpty())
			account_menu->setIcon(IconsManager::instance()->loadIcon(protocolFactory->iconName()));

		if (protocolFactory->protocolMenuManager()->protocolActions(account, con).size() == 0)
			continue;

		foreach (ActionDescription *actionDescription, protocolFactory->protocolMenuManager()->protocolActions(account, con))
			if (actionDescription)
			{
				Action *action = actionDescription->createAction(MyMainWindow);
				account_menu->addAction(action);
				action->checkState();
			}
			else
				account_menu->addSeparator();
	}

	menu->exec(event->globalPos());
}

void BuddiesListView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
			triggerActivate(currentIndex());
			break;
		default:
			QTreeView::keyPressEvent(event);
	}

	toolTipHide(false);
}

void BuddiesListView::wheelEvent(QWheelEvent *event)
{
	QTreeView::wheelEvent(event);

	// if event source (e->globalPos()) is inside this widget (QRect(...))
	if (QRect(QPoint(0, 0), size()).contains(event->pos()))
		toolTipRestart();
	else
		toolTipHide(false);
}

void BuddiesListView::leaveEvent(QEvent *event)
{
	QTreeView::leaveEvent(event);
	toolTipHide(false);
}

void BuddiesListView::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);
	toolTipHide();
}

void BuddiesListView::mouseReleaseEvent(QMouseEvent *event)
{
	QTreeView::mouseReleaseEvent(event);
	toolTipRestart();
}

void BuddiesListView::mouseMoveEvent(QMouseEvent *event)
{
	QTreeView::mouseMoveEvent(event);
	toolTipRestart();
}

void BuddiesListView::resizeEvent(QResizeEvent *event)
{
	QTreeView::resizeEvent(event);
	doItemsLayout();
	if (BackgroundImageMode == BackgroundStretched)
		updateBackground();
}

void BuddiesListView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	QTreeView::currentChanged(current, previous);

	if (!current.isValid())
		return;
	Buddy con = buddyAt(current);
	if (!con.isNull())
		emit currentBuddyChanged(con);
}

void BuddiesListView::selectionChanged( const QItemSelection &selected, const QItemSelection &deselected)
{
	emit buddySelectionChanged();
}

void BuddiesListView::doubleClickedSlot(const QModelIndex &index)
{
	triggerActivate(index);
}

void BuddiesListView::setBackground(const QString &backgroundColor, const QString &file, BackgroundMode mode)
{
	BackgroundColor = backgroundColor;
	setAnimated(mode == BackgroundNone);
	BackgroundImageMode = mode;
	BackgroundImageFile = file;
	updateBackground();
}

void BuddiesListView::updateBackground()
{
	// TODO 0.6.6 fix image "Stretched" + update on resize event - write image into resource tree
	QString style;

	style.append("QFrame {");

	style.append(QString(" background-color: %1;").arg(BackgroundColor));

	if (BackgroundImageMode == BackgroundNone)
	{
		style.append("}");
		//TODO 0.6.6: make an option in configuration:
		setAlternatingRowColors(false);
		setStyleSheet(style);
		return;
	}

	setAlternatingRowColors(false);

	if (BackgroundImageMode != BackgroundTiled && BackgroundImageMode != BackgroundTiledAndCentered)
		style.append(" background-repeat: no-repeat;");
	if (BackgroundImageMode == BackgroundCentered || BackgroundImageMode == BackgroundTiledAndCentered)
		style.append("background-position: center;");
	if (BackgroundImageMode == BackgroundStretched)
	{
		// style.append("background-size: 100% 100%;"); will work in 4.6 maybe?
		QImage image(BackgroundImageFile);
		if (image.isNull())
		{
			setStyleSheet("");
			return;
		}

		if (BackgroundTemporaryFile)
			delete BackgroundTemporaryFile;
		BackgroundTemporaryFile = new QTemporaryFile(QDir::tempPath() + "/kadu_background_XXXXXX.png", this);

		if (BackgroundTemporaryFile->open())
		{
			QImage stretched = image.scaled(viewport()->width(), viewport()->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			if (stretched.save(BackgroundTemporaryFile, "PNG"))
				style.append(QString("background-image: url(%1);").arg(BackgroundTemporaryFile->fileName()));
			BackgroundTemporaryFile->close();
		}
	}
	else
		style.append(QString("background-image: url(%1);").arg(BackgroundImageFile));
	style.append("background-attachment:fixed;}");

	setStyleSheet(style);
}

// Tool Tips

void BuddiesListView::toolTipTimeout()
{
	if (!ToolTipContact.isNull())
	{
		ToolTipClassManager::instance()->showToolTip(QCursor::pos(), ToolTipContact);
		ToolTipTimeoutTimer.stop();
	}
}

#define TOOL_TIP_TIMEOUT 1000

void BuddiesListView::toolTipRestart()
{
	Buddy con = buddyAt(indexAt(mapFromGlobal(QCursor::pos())));

	if (!con.isNull())
	{
		if (con != ToolTipContact)
			toolTipHide();
		ToolTipContact = con;
	}
	else
	{
		toolTipHide();
		ToolTipContact = Buddy::null;
	}

	ToolTipTimeoutTimer.start(TOOL_TIP_TIMEOUT);
}

void BuddiesListView::toolTipHide(bool waitForAnother)
{
	ToolTipClassManager::instance()->hideToolTip();

	if (waitForAnother)
		ToolTipTimeoutTimer.start(TOOL_TIP_TIMEOUT);
	else
		ToolTipTimeoutTimer.stop();
}
/*
QImage *UserBox::backgroundImage = 0;

class ULEComparer
{
	public:
		inline bool operator()(const Contact &e1, const Contact &e2) const;
		QList<UserBox::CmpFuncDesc> CmpFunctions;
		ULEComparer() : CmpFunctions() {}
};

inline bool ULEComparer::operator()(const Contact &e1, const Contact &e2) const
{
	int ret = 0;
	foreach(const UserBox::CmpFuncDesc &f, CmpFunctions)
	{
		ret = f.func(e1, e2);
//		kdebugm(KDEBUG_WARNING, "%s %s %d\n", qPrintable(e1.altNick()), qPrintable(e2.altNick()), ret);
		if (ret)
			break;
	}
	return ret < 0;
}*/

// CreateNotifier UserBox::createNotifier;


// 	showDescriptionAction = new ActionDescription(
// 		ActionDescription::TypeUserList, "descriptionsAction",
// 		this, SLOT(showDescriptionsActionActivated(QAction *, bool)),
// 		"ShowDescription", tr("Hide descriptions"),
// 		true, tr("Show descriptions")
// 	);
// 	connect(showDescriptionAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(setDescriptionsActionState()));

// 	setDescriptionsActionState();

// 	addCompareFunction("Status", tr("Statuses"), compareStatus);
// 	if (brokenStringCompare)
// 		addCompareFunction("AltNick", tr("Nicks, case insensitive"), compareAltNickCaseInsensitive);
// 	else
// 		addCompareFunction("AltNick", tr("Nicks"), compareAltNick);

// 	connect(&pending, SIGNAL(messageFromUserAdded(Contact)), this, SLOT(messageFromUserAdded(Contact)));
// 	connect(&pending, SIGNAL(messageFromUserDeleted(Contact)), this, SLOT(messageFromUserAdded(Contact)));

// void UserBox::showDescriptionsActionActivated(QActiogn *sender, bool toggle)
// {
// 	config_file.writeEntry("Look", "ShowDesc", !toggle);
// 	KaduListBoxPixmap::setShowDesc(!toggle);
// 	UserBox::refreshAllLater();
// 	setDescriptionsActionState();
// }

// void UserBox::setDescriptionsActionState()
// {
// 	foreach (KaduAction *action, showDescriptionAction->actions())
// 		action->setChecked(!KaduListBoxPixmap::ShowDesc);
// }

// void UserBox::messageFromUserAdded(Contact elem)
// {
// 	if (visibleUsers()->contains(UserListElement::fromContact(elem, AccountManager::instance()->defaultAccount())))
// 		refreshLater();
// }

// void UserBox::messageFromUserDeleted(Contact elem)
// {
// 	if (visibleUsers()->contains(UserListElement::fromContact(elem, AccountManager::instance()->defaultAccount())))
// 		refreshLater();
// }

/*
void UserBox::addCompareFunction(const QString &id, const QString &trDescription,
			int (*cmp)(const Contact &, const Contact &))
{
	comparer->CmpFunctions.append(CmpFuncDesc(id, trDescription, cmp));
	refreshLater();
}

void UserBox::removeCompareFunction(const QString &id)
{
	foreach(const CmpFuncDesc &c, comparer->CmpFunctions)
		if (c.id == id)
		{
			comparer->CmpFunctions.remove(c);
			refreshLater();
			break;
		}
}*/

// bool UserBox::moveUpCompareFunction(const QString &id)
// {
// 	kdebugf();
// 	CmpFuncDesc d;
// 	int pos = 0;
// 	bool found = false;
// 	QList<CmpFuncDesc>::iterator c;
// 	for (c = comparer->CmpFunctions.begin(); c != comparer->CmpFunctions.end(); ++c)
// 	{
//  		if ((*c).id == id)
//  		{
//  			found = true;
//  			if (pos == 0)
//  				break;
//  			d = *c;
//  			--c;
//  			c = comparer->CmpFunctions.insert(c, d);
//  			c += 2;
//  			comparer->CmpFunctions.remove(c);
//  			refreshLater();
//  			break;
//  		}
//  		++pos;
// 	}
// 	kdebugf2();
// 	return found;
// }

// bool UserBox::moveDownCompareFunction(const QString &id)
// {
// 	kdebugf();
// 	CmpFuncDesc d;
// 	int pos = 0;
// 	int cnt = comparer->CmpFunctions.count();
// 	bool found = false;
// 	QList<CmpFuncDesc>::iterator c;
// 	for (c = comparer->CmpFunctions.begin(); c != comparer->CmpFunctions.end(); ++c)
// 	{
//  		if ((*c).id == id)
//  		{
//  			found = true;
//  			if (pos == cnt - 1)
//  				break;
//  			d = *c;
//  			++c;
//  			c = comparer->CmpFunctions.insert(c, d);
//  			c -= 2;
//  			comparer->CmpFunctions.remove(c);
//  			refreshLater();
//  			break;
//  		}
//  		++pos;
// 	}
// 	kdebugf2();
// 	return found;
// }