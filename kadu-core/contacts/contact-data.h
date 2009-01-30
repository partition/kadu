/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_DATA
#define CONTACT_DATA

#include <QtCore/QMap>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "configuration/storable-object.h"

#define Property(type, name, capitalized_name) \
	type name() const { return capitalized_name; } \
	void set##capitalized_name(const type &name) { capitalized_name = name; }

class Account;
class ContactAccountData;
class ContactModuleData;
class XmlConfigFile;

class ContactData : public QSharedData, private StorableObject
{
	QUuid Uuid;
	QMap<QString, QString> CustomData;
	QMap<Account *, ContactAccountData *> AccountsData;
	QMap<QString, ContactModuleData *> ModulesData;

	QString Display;
	QString FirstName;
	QString LastName;
	QString NickName;
	QString HomePhone;
	QString Mobile;
	QString Email;

	bool Ignored;
	bool Blocked;
	bool OfflineTo;

protected:
	virtual StoragePoint * createStoragePoint() const;

public:
	static ContactData * loadFromStorage(StoragePoint *contactStoragePoint);

	ContactData(QUuid uniqueId = QUuid());
	~ContactData();

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void loadConfiguration();
	void storeConfiguration();

	StoragePoint * storagePointForAccountData(Account *account);
	StoragePoint * storagePointForModuleData(const QString &module, bool create = false);

	QUuid uuid() { return Uuid; }
	QString id(Account *account);
	Account * prefferedAccount();

	QMap<QString, QString> & customData() { return CustomData; }

	void addAccountData(ContactAccountData *accountData);
	ContactAccountData * accountData(Account *account);
	bool hasStoredAccountData(Account *account);

	ContactModuleData * moduleData(const QString &key);

template<class T>
	T * moduleData(bool create = false, bool cache = false)
	{
		if (!cache)
			return new T(storagePointForModuleData(T::key(), create));
		if (ModulesData.contains(T::key()))
			ModulesData[T::key()] = new T(storagePointForModuleData(T::key(), create));
		return dynamic_cast<T *>(ModulesData[T::key()]);
	}

	// properties
	bool isIgnored();
	bool setIgnored(bool ignored = true);
	bool isBlocked(Account *account);
	bool isOfflineTo(Account *account);

	Property(QString, display, Display)
	Property(QString, firstName, FirstName)
	Property(QString, lastName, LastName)
	Property(QString, nickName, NickName)
	Property(QString, homePhone, HomePhone)
	Property(QString, mobile, Mobile)
	Property(QString, email, Email)

};

#undef Property

#endif // CONTACT_DATA
