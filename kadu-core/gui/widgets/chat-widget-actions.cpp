/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-shared.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "chat/chat-manager.h"
#include "emoticons/emoticons.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/toolbar.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/open-chat-with/open-chat-with.h"
#include "gui/windows/search-window.h"

#include "custom-input.h"
#include "debug.h"

#include "chat-widget-actions.h"

static void disableEmptyTextBox(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(!chatEditBox->inputBox()->toPlainText().isEmpty());
}

static void disableEmptyMessages(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(0 != chatEditBox->chatWidget()->chatMessagesView()->countMessages());
}

static void disableNoChatImageService(Action *action)
{
	action->setEnabled(false);

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	Account account = action->chat().chatAccount();
	if (!account)
		return;

	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	action->setEnabled(protocol->chatImageService());
}

static void checkBlocking(Action *action)
{
	BuddySet buddies = action->buddies();

	if (!buddies.count() || buddies.contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	if (action && action->dataSource())
		action->setEnabled(!action->dataSource()->hasContactSelected());

	bool on = false;
	foreach (const Buddy &buddy, buddies)
		if (buddy.isBlocked())
		{
			on = true;
			break;
		}
	action->setChecked(on);
}

// TODO: quickhack
static void disableNoGadu(Action *action)
{
	action->setEnabled(false);

	Chat chat = action->chat();
	if (!chat)
		return;

	Protocol *protocol = chat.chatAccount().protocolHandler();
	if (!protocol)
		return;

	if (!protocol->protocolFactory())
		return;

	action->setEnabled(protocol->protocolFactory()->name() == "gadu");
}

ChatWidgetActions::ChatWidgetActions(QObject *parent) : QObject(parent)
{
	MoreActions = new ActionDescription(0,
		ActionDescription::TypeChat, "moreActionsAction",
		this, SLOT(moreActionsActionActivated(QAction *, bool)),
		"", tr("More..."), false
	);

	AutoSend = new ActionDescription(0,
		ActionDescription::TypeChat, "autoSendAction",
		this, SLOT(autoSendActionActivated(QAction *, bool)),
		"kadu_icons/enter", tr("Return Sends Message"), true
	);
	connect(AutoSend, SIGNAL(actionCreated(Action *)), this, SLOT(autoSendActionCreated(Action *)));

	ClearChat = new ActionDescription(0,
		ActionDescription::TypeChat, "clearChatAction",
		this, SLOT(clearActionActivated(QAction *, bool)),
		"edit-clear", tr("Clear Messages in Chat Window"), false,
		disableEmptyMessages
	);
	connect(ClearChat, SIGNAL(actionCreated(Action *)), this, SLOT(clearChatActionCreated(Action *)));

	InsertImage = new ActionDescription(0,
		ActionDescription::TypeChat, "insertImageAction",
		this, SLOT(insertImageActionActivated(QAction *, bool)),
		"insert-image", tr("Insert Image"), false,
		disableNoChatImageService
	);

	Bold = new ActionDescription(0,
		ActionDescription::TypeChat, "boldAction",
		this, SLOT(boldActionActivated(QAction *, bool)),
		"format-text-bold", tr("Bold"), true,
		disableNoGadu
	);

	Italic = new ActionDescription(0,
		ActionDescription::TypeChat, "italicAction",
		this, SLOT(italicActionActivated(QAction *, bool)),
		"format-text-italic", tr("Italic"), true,
		disableNoGadu
	);

	Underline = new ActionDescription(0,
		ActionDescription::TypeChat, "underlineAction",
		this, SLOT(underlineActionActivated(QAction *, bool)),
		"format-text-underline", tr("Underline"), true,
		disableNoGadu
	);

	Send = new ActionDescription(0,
		ActionDescription::TypeChat, "sendAction",
		this, SLOT(sendActionActivated(QAction *, bool)),
		"go-next", tr("&Send"), false,
		disableEmptyTextBox
	);
	connect(Send, SIGNAL(actionCreated(Action *)), this, SLOT(sendActionCreated(Action *)));

	Whois = new ActionDescription(0,
		ActionDescription::TypeChat, "whoisAction",
		this, SLOT(whoisActionActivated(QAction *, bool)),
		"edit-find", tr("Search this User in Directory"), false,
		disableNoContact
	);

	BlockUser = new ActionDescription(0,
		ActionDescription::TypeUser, "blockUserAction",
		this, SLOT(blockUserActionActivated(QAction *, bool)),
		"kadu_icons/block-buddy", tr("Block Buddy"), true,
		checkBlocking
	);

	OpenChat = new ActionDescription(0,
		ActionDescription::TypeUser, "chatAction",
		this, SLOT(openChatActionActivated(QAction *, bool)),
		"internet-group-chat", tr("&Chat"), false,
		disableNoChat
	);

	OpenWith = new ActionDescription(0,
		ActionDescription::TypeGlobal, "openChatWithAction",
		this, SLOT(openChatWithActionActivated(QAction *, bool)),
		"internet-group-chat", tr("Open Chat with...")
	);
	OpenWith->setShortcut("kadu_openchatwith", Qt::ApplicationShortcut);

	InsertEmoticon = new ActionDescription(0,
		ActionDescription::TypeChat, "insertEmoticonAction",
		this, SLOT(insertEmoticonActionActivated(QAction *, bool)),
		"face-smile", tr("Insert Emoticon")
	);
	connect(InsertEmoticon, SIGNAL(actionCreated(Action *)), this, SLOT(insertEmoticonActionCreated(Action *)));
/*
	ColorSelector = new ActionDescription(0,
		ActionDescription::TypeChat, "colorAction",
		this, SLOT(colorSelectorActionActivated(QAction *, bool)),
		"kadu_icons/change-color", tr("Change Color")
	);*/

	BuddiesListViewMenuManager::instance()->addActionDescription(OpenChat, BuddiesListViewMenuItem::MenuCategoryChat, 25);
}

ChatWidgetActions::~ChatWidgetActions()
{
}

void ChatWidgetActions::configurationUpdated()
{
	autoSendActionCheck();
	insertEmoticonsActionCheck();
}

void ChatWidgetActions::autoSendActionCreated(Action *action)
{
	action->setChecked(config_file.readBoolEntry("Chat", "AutoSend"));
}

void ChatWidgetActions::clearChatActionCreated(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	connect(chatEditBox->chatWidget()->chatMessagesView(), SIGNAL(messagesUpdated()), action, SLOT(checkState()));
}

void ChatWidgetActions::sendActionCreated(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	connect(chatEditBox->inputBox(), SIGNAL(textChanged()), action, SLOT(checkState()));

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;
}

void ChatWidgetActions::insertEmoticonActionCreated(Action *action)
{
	if (config_file.readEntry("Chat","EmoticonsTheme").isEmpty())
	{
		action->setToolTip(tr("Insert emoticon - enable in configuration"));
		action->setEnabled(false);
	}
}

void ChatWidgetActions::insertEmoticonsActionCheck()
{
	if (config_file.readEntry("Chat","EmoticonsTheme").isEmpty())
		foreach (Action *action, InsertEmoticon->actions())
		{
			action->setToolTip(tr("Insert emoticon - enable in configuration"));
			action->setEnabled(false);
		}
	else
		foreach (Action *action, InsertEmoticon->actions())
		{
			action->setToolTip(tr("Insert Emoticon"));
			action->setEnabled(true);
		}
}

void ChatWidgetActions::autoSendActionCheck()
{
 	bool check = config_file.readBoolEntry("Chat", "AutoSend");
 	foreach (Action *action, AutoSend->actions())
 		action->setChecked(check);
}

void ChatWidgetActions::autoSendActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	config_file.writeEntry("Chat", "AutoSend", toggled);
 	chatEditBox->setAutoSend(toggled);
	autoSendActionCheck();
}

void ChatWidgetActions::moreActionsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)
	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	QList<QWidget *> widgets = sender->associatedWidgets();
	if (widgets.size() == 0)
		return;

	QWidget *widget = widgets[widgets.size() - 1];

	QWidget *parent = widget->parentWidget();
	while (0 != parent && 0 == qobject_cast<ToolBar *>(parent))
		parent = parent->parentWidget();
	ToolBar *toolbar = qobject_cast<ToolBar *>(parent);

	QMenu menu;
	QMenu *subMenu = new QMenu(tr("More"), &menu);

	foreach (const QString &actionName, Actions::instance()->keys())
	{
		if (toolbar && toolbar->windowHasAction(actionName, false))
			continue;

		ActionDescription *actionDescription = Actions::instance()->value(actionName);
		if (ActionDescription::TypeChat == actionDescription->type())
			menu.addAction(Actions::instance()->createAction(actionName, chatEditBox));
		else if (ActionDescription::TypeUser == actionDescription->type())
			subMenu->addAction(Actions::instance()->createAction(actionName, chatEditBox));
	}

	menu.addSeparator();
	menu.addMenu(subMenu);
	menu.exec(widget->mapToGlobal(QPoint(0, widget->height())));
}

void ChatWidgetActions::clearActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
		chatWidget->clearChatWindow();

	kdebugf2();
}

void ChatWidgetActions::insertImageActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->openInsertImageDialog();
}

void ChatWidgetActions::boldActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontWeight(toggled ? QFont::Bold : QFont::Normal);

	kdebugf2();
}

void ChatWidgetActions::italicActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontItalic(toggled);

	kdebugf2();
}

void ChatWidgetActions::underlineActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontUnderline(toggled);

	kdebugf2();
}

void ChatWidgetActions::sendActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
		chatWidget->sendMessage();

	kdebugf2();
}

void ChatWidgetActions::whoisActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	Buddy buddy = action->buddy();
	if (!buddy)
	{
		(new SearchWindow(Core::instance()->kaduWindow()))->show();
		return;
	}

	SearchWindow *sd = new SearchWindow(Core::instance()->kaduWindow(), buddy);
	sd->show();
	sd->firstSearch();

	kdebugf2();
}

void ChatWidgetActions::blockUserActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	BuddySet buddies = action->buddies();
	if (buddies.isEmpty())
		return;

	bool on = false;

	foreach (const Buddy &buddy, buddies)
		if (!buddy.isBlocked())
		{
			on = true;
			break;
		}

	foreach (Buddy buddy, buddies)
	{
		buddy.setBlocked(on);
		// update actions
		updateBlockingActions(buddy);
	}

	kdebugf2();
}

void ChatWidgetActions::updateBlockingActions(Buddy buddy)
{
	QList<Contact> buddyContacts = buddy.contacts();

	foreach (Action *action, BlockUser->actions())
	{
		ContactSet contacts = action->contacts();
		if (1 == contacts.size())
			if (buddyContacts.contains(*contacts.begin()))
				action->setChecked(buddy.isBlocked());
	}
}

void ChatWidgetActions::openChatActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	Chat chat = action->chat();
	if (chat)
		ChatWidgetManager::instance()->openPendingMessages(chat, true);

	kdebugf2();
}

void ChatWidgetActions::openChatWithActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	OpenChatWith::instance()->show();

	kdebugf2();
}

void ChatWidgetActions::colorSelectorActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	QList<QWidget *> widgets = sender->associatedWidgets();
	if (widgets.size() == 0)
		return;

	chatEditBox->openColorSelector(widgets[widgets.size() - 1]);
}

void ChatWidgetActions::insertEmoticonActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	QList<QWidget *> widgets = sender->associatedWidgets();
	if (widgets.size() == 0)
		return;

	chatEditBox->openEmoticonSelector(widgets[widgets.size() - 1]);
}
