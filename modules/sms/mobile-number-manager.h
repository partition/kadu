/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MOBILE_NUMBER_MANAGER_H
#define MOBILE_NUMBER_MANAGER_H

#include "storage/storable-object.h"

class MobileNumber;

class MobileNumberManager : public StorableObject
{
	Q_DISABLE_COPY(MobileNumberManager)

	static MobileNumberManager *Instance;

	QList<MobileNumber *> Numbers;

	MobileNumberManager();
	virtual ~MobileNumberManager();

protected:
	virtual QSharedPointer<StoragePoint> createStoragePoint();

	virtual void load();

public:
	static MobileNumberManager * instance();
	static void destroyInstance();

	virtual void store();

	void registerNumber(QString number, QString gatewayId);
	void unregisterNumber(QString number);

	virtual QString storageNodeName() { return QLatin1String("MobileNumbers"); }
	virtual QString storageNodeItemName() { return QLatin1String("MobileNumber"); }
	virtual StorableObject * storageParent();

	QString gatewayId(const QString &mobileNumber);

};

#endif // MOBILE_NUMBER_MANAGER_H
