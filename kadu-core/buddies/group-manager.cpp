/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QSet>
#include <QtCore/QStringList>
#include <QtXml/QDomElement>

#include "buddies/buddy-manager.h"
#include "buddies/group.h"
#include "configuration/configuration-manager.h"
#include "configuration/storage-point.h"
#include "configuration/xml-configuration-file.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"

#include "debug.h"

#include "group-manager.h"

GroupManager *GroupManager::Instance = 0;

GroupManager * GroupManager::instance()
{
	if (!Instance)
		Instance = new GroupManager();

	return Instance;
}

GroupManager::GroupManager()
{
	ConfigurationManager::instance()->registerStorableObject(this);
}

GroupManager::~GroupManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

StoragePoint * GroupManager::createStoragePoint()
{
	QDomElement groupsNode = xml_config_file->getNode("Groups");
	return new StoragePoint(xml_config_file, groupsNode);
}

void GroupManager::importConfiguration()
{
	StoragePoint *sp = storage();
	if (!sp || !sp->storage())
		return;

	QSet<QString> groups;
	XmlConfigFile *configurationStorage = sp->storage();

	QDomElement contactsNode = configurationStorage->getNode("Contacts", XmlConfigFile::ModeFind);
	if (contactsNode.isNull())
		return;

	QList<QDomElement> contactsElements = configurationStorage->getNodes(contactsNode, "Contact");
	foreach (QDomElement contactElement, contactsElements)
		foreach (QString newGroup, contactElement.attribute("groups").split(",", QString::SkipEmptyParts))
			groups << newGroup;

	foreach (QString groupName, groups)
		byName(groupName); // it can do import, too
}

void GroupManager::load()
{
	StorableObject::load();

	QDomElement groupsNode = xml_config_file->getNode("Groups", XmlConfigFile::ModeFind);
	if (groupsNode.isNull())
	{
		importConfiguration();
		return;
	}

	QDomNodeList groupNodes = groupsNode.elementsByTagName("Group");

	int count = groupNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement groupElement = groupNodes.at(i).toElement();
		if (groupElement.isNull())
			continue;

		StoragePoint *groupStoragePoint = new StoragePoint(xml_config_file, groupElement);
		addGroup(Group::loadFromStorage(groupStoragePoint));
	}
}

void GroupManager::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();
	emit saveGroupData();

	foreach (Group group, Groups)
		group.store();
}

QList<Group> GroupManager::groups()
{
	ensureLoaded();
	return Groups;
}

void GroupManager::addGroup(Group group)
{
	ensureLoaded();

	emit groupAboutToBeAdded(group);
	Groups << group;
	emit groupAdded(group);
}

void GroupManager::removeGroup(Group group)
{
	group.removeFromStorage();

	emit groupAboutToBeRemoved(group);
	Groups.removeAll(group);
	emit groupRemoved(group);
}

Group GroupManager::byUuid(const QString &uuid)
{
	if (uuid.isEmpty())
		return Group::null;

	ensureLoaded();

	foreach (Group group, Groups)
		if (uuid == group.uuid().toString())
			return group;

	return Group::null;
}

Group GroupManager::byIndex(unsigned int index) const
{
	if (index < 0 || index >= count())
		return Group::null;
	
	return Groups.at(index);
}

Group GroupManager::byName(const QString &name, bool create)
{
	if (name.isEmpty())
		return Group::null;

	ensureLoaded();

	foreach (Group group, Groups)
		if (name == group.name())
			return group;

	if (!create)
		return Group::null;

	Group group;
	group.data()->importConfiguration(name);
	addGroup(group);

	return group;
}

unsigned int GroupManager::indexOf(Group group) const
{
	return Groups.indexOf(group);
}

// TODO: move some of this to %like-encoding, so we don't block normal names
bool GroupManager::acceptableGroupName(const QString &groupName)
{
	kdebugf();
	if (groupName.isEmpty())
	{
		kdebugf2();
		return false;
	}

	if (groupName.contains(","))
	{
		MessageDialog::msg(tr("'%1' is prohibited").arg(','), true, "Warning");
		kdebugf2();
		return false;
	}

	if (groupName.contains(";"))
	{
		MessageDialog::msg(tr("'%1' is prohibited").arg(';'), true, "Warning");
		kdebugf2();
		return false;
	}

	bool number;
	groupName.toLong(&number);
	if (number)
	{
		MessageDialog::msg(tr("Numbers are prohibited"), true, "Warning");//because of gadu-gadu contact list format...
		kdebugf2();
		return false;
	}

	ensureLoaded();

	// TODO All translation
 	if (groupName == tr("All") || byName(groupName, false))
 	{
 		MessageDialog::msg(tr("This group already exists!"), true, "Warning");
 		kdebugf2();
 		return false;
 	}

	kdebugf2();
	return true;
}