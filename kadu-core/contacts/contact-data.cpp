/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtXml/QDomNamedNodeMap>

#include "accounts/account.h"
#include "contacts/contact-account-data.h"
#include "xml_config_file.h"

#include "contact-data.h"

ContactData::ContactData(QUuid uuid)
	: Uuid(uuid.isNull() ? QUuid::createUuid() : uuid)
{
}

ContactData::~ContactData()
{
}

void ContactData::addAccountData(ContactAccountData *accountData)
{
	AccountsData.insert(accountData->account()->name(), accountData);
}

void ContactData::importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	QDomNamedNodeMap attributes = parent.attributes();
	int count = attributes.count();

	for (int i = 0; i < count; i++)
	{
		QDomAttr attribute = attributes.item(i).toAttr();
		CustomData.insert(attribute.name(), attribute.value());
	}
}

void ContactData::loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
}

void ContactData::storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	QDomElement customDataValues = configurationStorage->getNode(parent,
			"CustomDataValues", XmlConfigFile::ModeCreate);

	foreach (QString key, CustomData.keys())
	{
		configurationStorage->createNamedTextNode(customDataValues, "CustomDataValue", key, CustomData[key]);
	}

	foreach (ContactAccountData *accountData, AccountsData.values())
	{
		QDomElement contactAccountData = configurationStorage->getNamedNode(parent,
			"ContactAccountData", accountData->account()->name(), XmlConfigFile::ModeCreate);
		accountData->storeConfiguration(configurationStorage, contactAccountData);
	}
}
