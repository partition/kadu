/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtCore/QFile>
#include <QtCore/QTimer>

#include "contacts/contact.h"

#include "keys/key.h"
#include "keys/key-shared.h"

#include "keys-manager.h"

KeysManager * KeysManager::Instance = 0;

ENCRYPTIONAPI KeysManager * KeysManager::instance()
{
	if (!Instance)
		Instance = new KeysManager();

	return Instance;
}

void KeysManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

KeysManager::KeysManager()
{
}

KeysManager::~KeysManager()
{
}

void KeysManager::itemAboutToBeAdded(Key item)
{
	emit keyAboutToBeAdded(item);
}

void KeysManager::itemAdded(Key item)
{
	connect(item, SIGNAL(updated()), this, SLOT(keyDataUpdated()));
	emit keyAdded(item);
}

void KeysManager::itemAboutToBeRemoved(Key item)
{
	emit keyAboutToBeRemoved(item);
	disconnect(item, SIGNAL(updated()), this, SLOT(keyDataUpdated()));
}

void KeysManager::itemRemoved(Key item)
{
	emit keyRemoved(item);
}

void KeysManager::keyDataUpdated()
{
	QMutexLocker(&mutex());

	Key key(sender());
	if (key)
		emit keyUpdated(key);
}

Key KeysManager::byContactAndType(Contact contact, const QString &keyType, NotFoundAction action)
{
	ensureLoaded();

	foreach (const Key &key, items())
		if (key.keyContact() == contact && key.keyType() == keyType)
			return key;

	if (ActionReturnNull == action)
		return Key::null;

	Key key = Key::create();
	key.setKeyContact(contact);
	key.setKeyType(keyType);

	if (ActionCreateAndAdd == action)
		addItem(key);

	return key;
}
