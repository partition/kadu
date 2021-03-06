/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
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

#ifndef ENCRYPTION_MANAGER_H
#define ENCRYPTION_MANAGER_H

#include <QtCore/QObject>
#include <QtGui/QAction>

#include "accounts/accounts-aware-object.h"
#include "chat/chat.h"

#include "encryption_exports.h"

class ActionDescription;
class ChatWidget;
class KeyGenerator;

class ENCRYPTIONAPI EncryptionManager : public QObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(EncryptionManager)

	static EncryptionManager *Instance;

	KeyGenerator *Generator;

	EncryptionManager();
	virtual ~EncryptionManager();

private slots:
	void filterRawOutgoingMessage(Chat chat, QByteArray &message, bool &stop);
	void filterRawIncomingMessage(Chat chat, Contact sender, QByteArray &message, bool &ignore);

	void chatWidgetCreated(ChatWidget *chatWidget);
	void chatWidgetDestroying(ChatWidget *chatWidget);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static void createInstance();
	static void destroyInstance();

	static EncryptionManager * instance() { return Instance; }

	void setGenerator(KeyGenerator *generator);
	KeyGenerator * generator();

	bool setEncryptionEnabled(const Chat &chat, bool enable);

};

#endif // ENCRYPTION_MANAGER_H
