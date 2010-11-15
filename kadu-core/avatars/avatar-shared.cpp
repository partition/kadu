/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "avatars/avatar-manager.h"
#include "misc/misc.h"

#include "avatar-shared.h"

AvatarShared * AvatarShared::loadStubFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	AvatarShared *result = loadFromStorage(storagePoint);
	result->loadStub();

	return result;
}

AvatarShared * AvatarShared::loadFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	AvatarShared *result = new AvatarShared();
	result->setStorage(storagePoint);

	return result;
}

AvatarShared::AvatarShared(QUuid uuid) :
		Shared(uuid), AvatarContact(Contact::null)
{
	AvatarsDir = profilePath("avatars/");
}

AvatarShared::~AvatarShared()
{
}

StorableObject * AvatarShared::storageParent()
{
	return AvatarManager::instance();
}

QString AvatarShared::storageNodeName()
{
	return QLatin1String("Avatar");
}

QString AvatarShared::filePath()
{
	ensureLoaded();
	return AvatarsDir + uuid().toString();
}

void AvatarShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	LastUpdated = loadValue<QDateTime>("LastUpdated");
	NextUpdate = loadValue<QDateTime>("NextUpdate");
	Pixmap.load(filePath());
}

void AvatarShared::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	Shared::store();

	storeValue("LastUpdated", LastUpdated);
	storeValue("NextUpdate", NextUpdate);
}

bool AvatarShared::shouldStore()
{
	return UuidStorableObject::shouldStore() && !Pixmap.isNull();
}

void AvatarShared::aboutToBeRemoved()
{
	// cleanup references
	AvatarContact = Contact::null;

	QFile avatarFile(filePath());
	if (avatarFile.exists())
		avatarFile.remove();
}

bool AvatarShared::isEmpty()
{
	ensureLoaded();

	return Pixmap.isNull();
}

void AvatarShared::setPixmap(QPixmap pixmap)
{
	QDir avatarsDir(profilePath("avatars"));
	if (!avatarsDir.exists())
		avatarsDir.mkpath(profilePath("avatars"));

	Pixmap = pixmap;
	dataUpdated();

	if (pixmap.isNull())
		QFile::remove(filePath());
	else
		pixmap.save(filePath(), "PNG");
}

void AvatarShared::emitUpdated()
{
	emit updated();
}