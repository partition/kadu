/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_SHARED_H
#define CONTACT_SHARED_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QUuid>

#include "buddies/avatar.h"
#include "buddies/buddy.h"
#include "protocols/protocols-aware-object.h"
#include "status/status.h"

#include "shared/shared.h"

class ContactDetails;

class KADUAPI ContactShared : public QObject, public Shared, ProtocolsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactShared)

	ContactDetails *Details;

	Account ContactAccount;
	Avatar ContactAvatar;
	Buddy OwnerBuddy;
	QString Id;

	Status CurrentStatus;

	QString ProtocolVersion;

	QHostAddress Address;
	unsigned int Port;
	QString DnsName;

protected:
	virtual void emitUpdated();

	virtual void protocolRegistered(ProtocolFactory *protocolFactory);
	virtual void protocolUnregistered(ProtocolFactory *protocolFactory);

public:
	static ContactShared * loadFromStorage(StoragePoint *contactStoragePoint);

	explicit ContactShared(QUuid uuid = QUuid());
	virtual ~ContactShared();

	virtual void load();
	virtual void store();

	void loadDetails();
	void unloadDetails();

	KaduShared_Property(ContactDetails *, details, Details)
	KaduShared_Property(Account, contactAccount, ContactAccount)
	KaduShared_Property(Avatar, contactAvatar, ContactAvatar)
	KaduShared_PropertyRead(Buddy, ownerBuddy, OwnerBuddy)
	void setOwnerBuddy(Buddy buddy);

	KaduShared_PropertyRead(QString, id, Id)
	void setId(const QString &id);

	KaduShared_Property(Status, currentStatus, CurrentStatus)
	KaduShared_Property(QString, protocolVersion, ProtocolVersion)
	KaduShared_Property(QHostAddress, address, Address)
	KaduShared_Property(unsigned int, port, Port)
	KaduShared_Property(QString, dnsName, DnsName)

signals:
	void updated();

	void idChanged(const QString &id);

	void protocolLoaded();
	void protocolUnloaded();

};

#endif // CONTACT_SHARED_H