/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact.h"
#include "contacts/contact-shared.h"
#include "jabber-contact-details.h"

#include "jabber-contact-data-manager.h"

JabberContactDataManager::JabberContactDataManager(Contact data, QObject *parent) :
		ConfigurationContactDataManager(data, parent), Data(dynamic_cast<JabberContactDetails *>(data.details()))
{
}

void JabberContactDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Jabber")
	{
		ConfigurationContactDataManager::writeEntry(section, name, value);
		return;
	}

	// TODO : setId

	// other data
}

QVariant JabberContactDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Jabber")
		return ConfigurationContactDataManager::readEntry(section, name);

	if (name == "Jid")
		return Data->contactData()->id();

	// other data

	return QVariant(QString::null);
}