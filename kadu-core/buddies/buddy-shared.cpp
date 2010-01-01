/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QVariant>
#include <QtXml/QDomNamedNodeMap>

#include "accounts/account.h"
#include "buddies/avatar-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-shared.h"
#include "contacts/contact-manager.h"
#include "storage/storage-point.h"

#include "buddy-shared.h"

BuddyShared * BuddyShared::loadFromStorage(StoragePoint *buddyStoragePoint)
{
	BuddyShared *result = new BuddyShared();
	result->setStorage(buddyStoragePoint);
	return result;
}

BuddyShared::BuddyShared(QUuid uuid) :
		QObject(BuddyManager::instance()), Shared(uuid),
		Anonymous(true), Ignored(false), Blocked(false), OfflineTo(false)
{
}

BuddyShared::~BuddyShared()
{
}

StorableObject * BuddyShared::storageParent()
{
	return BuddyManager::instance();
}

QString BuddyShared::storageNodeName()
{
	return QLatin1String("Buddy");
}

#define ImportProperty(name, old_name) \
	set##name(CustomData[#old_name]); \
	CustomData.remove(#old_name);

void BuddyShared::importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	QDomNamedNodeMap attributes = parent.attributes();
	int count = attributes.count();

	for (int i = 0; i < count; i++)
	{
		QDomAttr attribute = attributes.item(i).toAttr();
		CustomData.insert(attribute.name(), attribute.value());
	}

	Anonymous = false;

	QStringList groups = CustomData["groups"].split(',', QString::SkipEmptyParts);
	foreach (const QString &group, groups)
		Groups << GroupManager::instance()->byName(group);

	CustomData.remove("groups");

	ImportProperty(Display, altnick)
	ImportProperty(FirstName, first_name)
	ImportProperty(LastName, last_name)
	ImportProperty(NickName, nick_name)
	ImportProperty(HomePhone, home_phone)
	ImportProperty(Mobile, mobile)
	ImportProperty(Email, email)
}

void BuddyShared::load()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	Shared::load();

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	if (parent.hasAttribute("type"))
	{
		Anonymous = (1 == parent.attribute("type").toInt());
		parent.removeAttribute("type");
	}
	else
		Anonymous = loadValue<bool>("Anonymous", true);

	QDomElement customDataValues = configurationStorage->getNode(parent, "CustomDataValues", XmlConfigFile::ModeFind);
	QDomNodeList customDataValuesList = customDataValues.elementsByTagName("CustomDataValue");

	int count = customDataValuesList.count();
	for (int i = 0; i < count; i++)
	{
		QDomNode customDataNode = customDataValuesList.at(i);
		QDomElement customDataElement = customDataNode.toElement();
		if (customDataElement.isNull())
			continue;

		QString name = customDataElement.attribute("name");
		if (!name.isEmpty())
			CustomData[name] = customDataElement.text();
	}

	Groups.clear();
	QDomElement groupsNode = configurationStorage->getNode(parent, "ContactGroups", XmlConfigFile::ModeFind);
	if (!groupsNode.isNull())
	{
		QDomNodeList groupsList = groupsNode.elementsByTagName("Group");

		count = groupsList.count();
		for (int i = 0; i < count; i++)
		{
			QDomElement groupElement = groupsList.at(i).toElement();
			if (groupElement.isNull())
				continue;
			Group group = GroupManager::instance()->byUuid(groupElement.text());
			if (!group.isNull())
				Groups << group;
		}
	}

	AvatarManager::instance()->ensureLoaded(); // byUuid does not do it
	BuddyAvatar = AvatarManager::instance()->byUuid(loadValue<QString>("Avatar"));
	Display = loadValue<QString>("Display");
	FirstName = loadValue<QString>("FirstName");
	LastName = loadValue<QString>("LastName");
	NickName = loadValue<QString>("NickName");
	HomePhone = loadValue<QString>("HomePhone");
	Mobile = loadValue<QString>("Mobile");
	Email = loadValue<QString>("Email");
	Website = loadValue<QString>("Website");
	Ignored = loadValue<bool>("Ignored", false);
	Blocked = loadValue<bool>("Blocked", false);
	OfflineTo = loadValue<bool>("OfflineTo", false);
}

void BuddyShared::store()
{
	ensureLoaded();

	StoragePoint *sp = storage();
	if (!sp)
		return;

	Shared::store();

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	QDomElement customDataValues = configurationStorage->getNode(parent, "CustomDataValues");

	foreach (const QString &key, CustomData.keys())
		configurationStorage->createNamedTextNode(customDataValues, "CustomDataValue", key, CustomData[key]);

	if (BuddyAvatar.uuid().isNull())
		removeValue("Avatar");
	else
		storeValue("Avatar", BuddyAvatar.uuid().toString());

	storeValue("Display", Display);
	storeValue("FirstName", FirstName);
	storeValue("LastName", LastName);
	storeValue("NickName", NickName);
	storeValue("HomePhone", HomePhone);
	storeValue("Mobile", Mobile);
	storeValue("Email", Email);
	storeValue("Website", Website);
	storeValue("Anonymous", Anonymous);
	storeValue("Ignored", Ignored);
	storeValue("Blocked", Blocked);
	storeValue("OfflineTo", OfflineTo);

	if (Groups.count())
	{
		QDomElement groupsNode = configurationStorage->getNode(parent, "ContactGroups", XmlConfigFile::ModeCreate);
		foreach (Group group, Groups)
			configurationStorage->appendTextNode(groupsNode, "Group", group.uuid().toString());
	}
	else
		configurationStorage->removeNode(parent, "ContactGroups");

	configurationStorage->createTextNode(parent, "Ignored", QVariant(Ignored).toString());
}

void BuddyShared::aboutToBeRemoved()
{
	foreach (Contact contact, Contacts)
		contact.setOwnerBuddy(Buddy::null);

	Contacts = QList<Contact>();
	Groups = QList<Group>();
}

void BuddyShared::addContact(Contact contact)
{
	ensureLoaded();

	if (contact.isNull() || Contacts.contains(contact))
		return;

	emit contactAboutToBeAdded(contact);
	Contacts.append(contact);

	if (contact.priority() != -1)
		sortContacts();
	else
	{
		int last = Contacts.count() > 1
				? Contacts[Contacts.count() - 2].priority()
				: 0;
		contact.setPriority(last);
	}

	emit contactAdded(contact);
}

void BuddyShared::removeContact(Contact contact)
{
	ensureLoaded();

	if (contact.isNull() || !Contacts.contains(contact))
		return;

	emit contactAboutToBeRemoved(contact);
	Contacts.removeAll(contact);
	emit contactRemoved(contact);

	normalizePriorities();
}

QList<Contact> BuddyShared::contacts(Account account)
{
	ensureLoaded();

	QList<Contact> contacts;

	foreach (const Contact &contact, Contacts)
		if (contact.contactAccount() == account)
			contacts.append(contact);

	// TODO 0.6.6 : if count() > 1 ... sort out! (0 - preffered)
	return contacts;
}

QList<Contact> BuddyShared::contacts()
{
	ensureLoaded();

	return Contacts;
}

QString BuddyShared::id(Account account)
{
	ensureLoaded();

	QList<Contact> contactslist;
	contactslist = contacts(account);
	if (contactslist.count() > 0)
		return contactslist[0].id();

	return QString::null;
}

Contact BuddyShared::prefferedContact()
{
	ensureLoaded();

	// TODO 0.6.6: implement it to have most available contact
	int count = Contacts.count();
	if (count == 0)
		return Contact::null;

	if (count == 1)
		return Contacts[0];

	Contact prefferedContact = Contacts[0];
	foreach (const Contact &contact, Contacts)
		if (contact.currentStatus() < prefferedContact.currentStatus())
			prefferedContact = contact;

	return prefferedContact;
}

bool contactPriorityLessThan(const Contact &c1, const Contact &c2)
{
	return c1.priority() < c2.priority();
}

void BuddyShared::sortContacts()
{
	qStableSort(Contacts.begin(), Contacts.end(), contactPriorityLessThan);
}

void BuddyShared::normalizePriorities()
{
	int priority = 0;
	foreach (Contact contact, Contacts)
		contact.setPriority(priority++);
}

Account BuddyShared::prefferedAccount()
{
	ensureLoaded();

	return prefferedContact().contactAccount();
}

void BuddyShared::emitUpdated()
{
	emit updated();
}

// properties

bool BuddyShared::isInGroup(Group group)
{
	ensureLoaded();

	return Groups.contains(group);
}

bool BuddyShared::showInAllGroup()
{
	ensureLoaded();

	foreach (const Group group, Groups)
		if (!group.isNull() && !group.showInAllGroup())
			return false;

	return true;
}

void BuddyShared::addToGroup(Group group)
{
	ensureLoaded();

	Groups.append(group);
	dataUpdated();
}

void BuddyShared::removeFromGroup(Group group)
{
	ensureLoaded();

	Groups.removeAll(group);
	dataUpdated();
}