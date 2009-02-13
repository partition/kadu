/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QScrollBar>
#include <QtWebKit/QWebFrame>

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "chat/chat_message.h"
#include "chat/chat_styles_manager.h"
#include "chat/style-engines/chat_style_engine.h"

#include "protocols/services/chat-image-service.h"

#include "chat_widget.h"
#include "config_file.h"
#include "debug.h"

#include "chat_messages_view.h"

ChatMessagesView::ChatMessagesView(QWidget *parent) : KaduTextBrowser(parent),
	lastScrollValue(0), lastLine(false), Chat(0), PrevMessage(0)
{
	setMinimumSize(QSize(100,100));

 	ChatStylesManager::instance()->chatViewCreated(this);

	// TODO: for me with empty styleSheet if has artifacts on scrollbars...
	// maybe Qt bug?
  	setStyleSheet("QWidget { }");

	Protocol *protocol = AccountManager::instance()->defaultAccount()->protocol();
	ChatImageService *chatImageService = protocol->chatImageService();
	if (chatImageService)
			connect(chatImageService, SIGNAL(imageReceived(const QString &, const QString &)),
				this, SLOT(imageReceived(const QString &, const QString &)));


	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(scrollToLine()));

	if (parent)
		Chat = dynamic_cast<ChatWidget *>(parent);

	settings()->setAttribute(QWebSettings::JavascriptEnabled, true);

	setFocusPolicy(Qt::NoFocus);
}

ChatMessagesView::~ChatMessagesView()
{
	qDeleteAll(Messages);
	Messages.clear();
 	ChatStylesManager::instance()->chatViewDestroyed(this);

	Protocol *protocol = AccountManager::instance()->defaultAccount()->protocol();
	ChatImageService *chatImageService = protocol->chatImageService();
	if (chatImageService)
			disconnect(chatImageService, SIGNAL(imageReceived(const QString &, const QString &)),
				this, SLOT(imageReceived(const QString &, const QString &)));
}

void ChatMessagesView::pageUp()
{
	QKeyEvent event(QEvent::KeyPress, 0x01000016, Qt::NoModifier);
	keyPressEvent(&event);
}

void ChatMessagesView::pageDown()
{
	QKeyEvent event(QEvent::KeyPress, 0x01000017, Qt::NoModifier);
	keyPressEvent(&event);
}

void ChatMessagesView::imageReceived(const QString &messageId, const QString &messagePath)
{
	foreach (ChatMessage *message, Messages)
		message->replaceLoadingImages(messageId, messagePath);

 	ChatStylesManager::instance()->currentEngine()->refreshView(this);
}

void ChatMessagesView::updateBackgroundsAndColors()
{
	QString myBackgroundColor = config_file.readEntry("Look", "ChatMyBgColor");
	QString myFontColor = config_file.readEntry("Look", "ChatMyFontColor");
	QString myNickColor = config_file.readEntry("Look", "ChatMyNickColor");
	QString usrBackgroundColor = config_file.readEntry("Look", "ChatUsrBgColor");
	QString usrFontColor = config_file.readEntry("Look", "ChatUsrFontColor");
	QString usrNickColor = config_file.readEntry("Look", "ChatUsrNickColor");

	foreach(ChatMessage *message, Messages)
	{
		switch (message->type())
		{
			case TypeSent:
				message->setColorsAndBackground(myBackgroundColor, myNickColor, myFontColor);
				break;

			case TypeReceived:
				message->setColorsAndBackground(usrBackgroundColor, usrNickColor, usrFontColor);
				break;

			case TypeSystem:
				break;
		}

	}
}

void ChatMessagesView::appendMessage(ChatMessage *message)
{
	kdebugf();

	Messages.append(message);

	pruneMessages();

	ChatStylesManager::instance()->currentEngine()->appendMessage(this, message);
}

void ChatMessagesView::appendMessages(QList<ChatMessage *> messages)
{
	kdebugf2();

	Messages += messages;

	pruneMessages();

	ChatStylesManager::instance()->currentEngine()->appendMessages(this, messages);
}

void ChatMessagesView::pruneMessages()
{
	kdebugf();

	if (ChatStylesManager::instance()->prune() == 0)
		return;

	if (Messages.count() < ChatStylesManager::instance()->prune())
		return;

	QList<ChatMessage *>::iterator start = Messages.begin();
	QList<ChatMessage *>::iterator stop = Messages.find(Messages.at(Messages.size() - ChatStylesManager::instance()->prune()));
	for (QList<ChatMessage *>::iterator it = start; it != stop; ++it)
	{
		delete *it;
		ChatStylesManager::instance()->currentEngine()->pruneMessage(this);
	}
	Messages.erase(start, stop);
}

void ChatMessagesView::clearMessages()
{
	qDeleteAll(Messages);
	Messages.clear();
	PrevMessage = 0;
 	ChatStylesManager::instance()->currentEngine()->clearMessages(this);
}

unsigned int ChatMessagesView::countMessages()
{
	return Messages.count();
}

void ChatMessagesView::resizeEvent(QResizeEvent *e)
{
 	lastScrollValue = page()->currentFrame()->scrollBarValue(Qt::Vertical);
 	lastLine = (lastScrollValue == page()->currentFrame()->scrollBarMaximum(Qt::Vertical));

 	KaduTextBrowser::resizeEvent(e);

	scrollToLine();
}

void ChatMessagesView::rememberScrollBarPosition()
{
	lastScrollValue = page()->currentFrame()->scrollBarValue(Qt::Vertical);
	lastLine = (lastScrollValue == page()->currentFrame()->scrollBarMaximum(Qt::Vertical));
}

void ChatMessagesView::scrollToLine()
{
 	if (lastLine)
 		page()->currentFrame()->setScrollBarValue(Qt::Vertical, page()->currentFrame()->scrollBarMaximum(Qt::Vertical));
 	else
 		page()->currentFrame()->setScrollBarValue(Qt::Vertical, lastScrollValue);
}

