/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QVBoxLayout>

#include "chat_manager.h"
#include "chat_window.h"
#include "config_file.h"
#include "debug.h"
#include "message_box.h"
#include "usergroup.h"
#include "misc.h"

ChatWindow::ChatWindow(QWidget *parent)
	: QWidget(parent), currentChatWidget(0), title_timer(new QTimer(this, "title_timer"))
{
	kdebugf();

	connect(title_timer, SIGNAL(timeout()), this, SLOT(blinkTitle()));
	connect(this, SIGNAL(chatWidgetActivated(ChatWidget *)), chat_manager, SIGNAL(chatWidgetActivated(ChatWidget *)));

#ifdef Q_OS_MAC
	setAttribute(Qt::WA_MacBrushedMetal);
#endif
	setAttribute(Qt::WA_DeleteOnClose);

	configurationUpdated();
}

ChatWindow::~ChatWindow()
{
	kaduStoreGeometry();
}

void ChatWindow::configurationUpdated()
{
	activateWithNewMessages = config_file.readBoolEntry("Chat", "ActivateWithNewMessages", false);
	showNewMessagesNum = config_file.readBoolEntry("Chat", "NewMessagesInChatTitle", false);
	blinkChatTitle = config_file.readBoolEntry("Chat", "BlinkChatTitle", true);

	if (currentChatWidget && currentChatWidget->newMessagesCount())
		blinkTitle();
}

// TODO: fix it
void ChatWindow::setChatWidget(ChatWidget *newChatWidget)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	currentChatWidget = newChatWidget;
	newChatWidget->reparent(this, QPoint(0, 0));
	newChatWidget->show();

	layout->addWidget(newChatWidget);
	layout->setMargin(0);
	layout->setSpacing(0);

	connect(currentChatWidget, SIGNAL(closed()), this, SLOT(close()));
	connect(currentChatWidget, SIGNAL(captionUpdated()), this, SLOT(updateTitle()));
	connect(currentChatWidget, SIGNAL(messageReceived(ChatWidget *)), this, SLOT(alertNewMessage()));

	setFocusProxy(currentChatWidget);

	kaduRestoreGeometry();
	updateTitle();
}

ChatWidget * ChatWindow::chatWidget()
{
	return currentChatWidget;
}

void ChatWindow::kaduRestoreGeometry()
{
	const UserGroup *group = currentChatWidget->users();
	QRect geom = stringToRect(chat_manager->chatWidgetProperty(group, "Geometry").toString());

	if (geom.isEmpty() && group->count() == 1)
		geom = stringToRect((*(group->constBegin())).data("ChatGeometry").toString());

	if (geom.isEmpty())
	{
		geom = QRect(pos().x(), pos().y(), 400, 400);
		if (group->count() > 1)
			geom.setWidth(550);
	}

	QDesktopWidget *desktop = qApp->desktop();
	QRect desktopRect = desktop->availableGeometry(desktop->screenNumber(this));
	geom = desktopRect.intersected(geom);
	setGeometry(geom);
	currentChatWidget->setGeometry(geom);
	currentChatWidget->kaduRestoreGeometry();
}

void ChatWindow::kaduStoreGeometry()
{
	currentChatWidget->kaduStoreGeometry();

	const UserGroup *users = currentChatWidget->users();
	chat_manager->setChatWidgetProperty(users, "Geometry", rectToString(geometry()));
	if (users->count() == 1)
		(*users->begin()).setData("ChatGeometry", rectToString(geometry()));
}

void ChatWindow::closeEvent(QCloseEvent *e)
{
	kdebugf();

	if (config_file.readBoolEntry("Chat", "ChatCloseTimer"))
	{
		unsigned int period = config_file.readUnsignedNumEntry("Chat",
			"ChatCloseTimerPeriod", 2);

		if (QDateTime::currentDateTime() < currentChatWidget->getLastMsgTime().addSecs(period))
		{
			if (!MessageBox::ask(tr("New message received, close window anyway?")))
			{
				e->ignore();
				return;
			}
		}
	}

 	QWidget::closeEvent(e);
}

void ChatWindow::updateTitle()
{
	setIcon(currentChatWidget->icon());
	setWindowTitle(currentChatWidget->escapedCaption());

	if (showNewMessagesNum && currentChatWidget->newMessagesCount()) // if we don't have new messages or don't want them to be shown
		showNewMessagesNumInTitle();
}

void ChatWindow::blinkTitle()
{
 	if (!isActiveWindow())
  	{
		if (!caption().contains(currentChatWidget->caption()) || !blinkChatTitle)
		{
  			if (!showNewMessagesNum) // if we don't show number od new messages waiting
  				setWindowTitle(currentChatWidget->escapedCaption());
  			else
				showNewMessagesNumInTitle();
		}
		else
			setWindowTitle(QString().fill(' ', (currentChatWidget->caption().length() + 5)));

		if (blinkChatTitle) // timer will not be started, if configuration option was changed
			title_timer->start(500,TRUE);
	}
}

void ChatWindow::showNewMessagesNumInTitle()
{
	if (!isActiveWindow())
		setWindowTitle("[" + QString().setNum(currentChatWidget->newMessagesCount()) + "] " + currentChatWidget->escapedCaption());
}

void ChatWindow::windowActivationChange(bool b)
{
	kdebugf();
	if (isActiveWindow())
	{
		currentChatWidget->markAllMessagesRead();
		setWindowTitle(currentChatWidget->escapedCaption());

		if (title_timer->isActive())
			title_timer->stop();

		if (!b)
			emit chatWidgetActivated(currentChatWidget);
	}
	kdebugf2();
}

void ChatWindow::alertNewMessage()
{
	if (!isActiveWindow())
	{
		if (activateWithNewMessages && qApp->activeWindow() && !isMinimized())
		{
			currentChatWidget->setActiveWindow();
			raise();
		}
		else if (blinkChatTitle)
		{
			if (!title_timer->isActive())
				blinkTitle(); // blinking is able to show new messages also...
		}
		else if (showNewMessagesNum) // ... so we check this condition as 'else'
			showNewMessagesNumInTitle();
#ifndef Q_OS_MAC
		qApp->alert(this);
#endif
	}
	else
		currentChatWidget->markAllMessagesRead();
}

void ChatWindow::closeChatWidget(ChatWidget *chatWidget)
{
	close();
}
