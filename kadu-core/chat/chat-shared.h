/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_SHARED_H
#define CHAT_SHARED_H

#include <QtGui/QPixmap>

#include "accounts/account.h"
#include "chat/type/chat-type-aware-object.h"
#include "shared/shared.h"

class BuddySet;
class Chat;
class ChatDetails;

class KADUAPI ChatShared : public QObject, public Shared, ChatTypeAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatShared)

	ChatDetails *Details;

	Account ChatAccount;
	QString Type;
	QString Title;
	QPixmap Icon;

protected:
	void emitUpdated();

	virtual void chatTypeRegistered(ChatType *chatType);
	virtual void chatTypeUnregistered(ChatType *chatType);

public:
	static ChatShared * loadFromStorage(StoragePoint *storagePoint);

	explicit ChatShared(QUuid uuid = QUuid());
	virtual ~ChatShared();

	virtual void load();
	virtual void store();

	BuddySet buddies() const;
	QString name() const;

	KaduShared_Property(ChatDetails *, details, Details)
	KaduShared_Property(Account, chatAccount, ChatAccount)
	KaduShared_Property(QString, type, Type)
	KaduShared_Property(QString, title, Title)
	KaduShared_Property(QPixmap, icon, Icon)

public slots:
	void refreshTitle();

signals:
	void titleChanged(Chat chat, const QString &newTitle);

	void chatTypeLoaded();
	void chatTypeUnloaded();

	void updated();

};

#include "chat/chat.h" // for MOC

#endif // CHAT_SHARED_H