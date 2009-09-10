/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "contacts/contact-account-data.h"

#include "avatar.h"

Avatar::Avatar(ContactAccountData *contactAccountData) :
		StorableObject("Avatar", contactAccountData),
		MyContactAccountData(contactAccountData)
{
}

Avatar::~Avatar()
{
}

void Avatar::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	LastUpdated = loadValue<QDateTime>("LastUpdated");
	NextUpdate = loadValue<QDateTime>("NextUpdate");
	FileName = loadValue<QString>("FileName");

	Pixmap.load(FileName);
}

void Avatar::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	storeValue("LastUpdated", LastUpdated);
	storeValue("NextUpdate", NextUpdate);
	storeValue("FileName", FileName);
}

ContactAccountData * Avatar::contactAccountData()
{
	return MyContactAccountData;
}

QDateTime Avatar::lastUpdated()
{
	ensureLoaded();
	return LastUpdated;
}

void Avatar::setLastUpdated(const QDateTime &lastUpdated)
{
	ensureLoaded();
	LastUpdated = lastUpdated;
}

QDateTime Avatar::nextUpdate()
{
	ensureLoaded();
	return NextUpdate;
}

void Avatar::setNextUpdate(const QDateTime &nextUpdate)
{
	ensureLoaded();
	NextUpdate = nextUpdate;
}

QString Avatar::fileName()
{
	ensureLoaded();
	return FileName;
}

void Avatar::setFileName(const QString &fileName)
{
	ensureLoaded();
	FileName = fileName;
}

QPixmap Avatar::pixmap()
{
	ensureLoaded();
	return Pixmap;
}

void Avatar::setPixmap(const QPixmap &pixmap)
{
	ensureLoaded();
	Pixmap = pixmap;
}