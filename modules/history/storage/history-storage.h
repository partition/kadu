/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef HISTORY_STORAGE_H
#define HISTORY_STORAGE_H

#include <QtCore/QDate>
#include <QtCore/QDateTime>
#include <QtCore/QObject>

#include "chat/chat.h"
#include "buddies/buddy.h"

#include "../history_exports.h"

class HistorySearchParameters;
class Message;
class TimedStatus;

class HISTORYAPI HistoryStorage : public QObject
{
	Q_OBJECT

private slots:
	virtual void messageReceived(const Message &message) = 0;
	virtual void messageSent(const Message &message) = 0;

public:
	explicit HistoryStorage(QObject *parent) : QObject(parent) {}
	virtual ~HistoryStorage() {}

	virtual QList<Chat> chats(const HistorySearchParameters &search) = 0;

	virtual QList<QDate> chatDates(const Chat &chat, const HistorySearchParameters &search) = 0;
	virtual QList<Message> messages(const Chat &chat, const QDate &date = QDate(), int limit = 0) = 0;
	virtual QList<Message> messagesSince(const Chat &chat, const QDate &date) = 0;
	virtual QList<Message> messagesBackTo(const Chat &chat, const QDateTime &datetime, int limit) = 0;
	virtual int messagesCount(const Chat &chat, const QDate &date = QDate()) = 0;

	virtual QList<Buddy> statusBuddiesList(const HistorySearchParameters &search) = 0;
	virtual QList<QDate> datesForStatusBuddy(const Buddy &buddy, const HistorySearchParameters &search) = 0;
	virtual QList<TimedStatus> statuses(const Buddy &buddy, const QDate &date = QDate(), int limit = 0) = 0;
	virtual int statusBuddyCount(const Buddy &buddy, const QDate &date = QDate()) = 0;

	virtual QList<QString> smsRecipientsList(const HistorySearchParameters &search) = 0;
	virtual QList<QDate> datesForSmsRecipient(const QString &recipient, const HistorySearchParameters &search) = 0;
	virtual QList<Message> sms(const QString &recipient, const QDate &date = QDate(), int limit = 0) = 0;
	virtual int smsCount(const QString &recipient, const QDate &date = QDate()) = 0;

	virtual void appendMessage(const Message &message) = 0;
	virtual void appendStatus(const Contact &contact, const Status &status, const QDateTime &time = QDateTime::currentDateTime()) = 0;
	virtual void appendSms(const QString &recipient, const QString &content, const QDateTime &time = QDateTime::currentDateTime()) = 0;
	virtual void sync() = 0;

	virtual void clearChatHistory(const Chat &chat, const QDate &date = QDate()) = 0;
	virtual void clearSmsHistory(const QString &recipient, const QDate &date = QDate()) = 0;
	virtual void clearStatusHistory(const Buddy &buddy, const QDate &date = QDate()) = 0;
	virtual void deleteHistory(const Buddy &buddy) = 0;

};

#endif // HISTORY_STORAGE_H
