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
#include "configuration/xml-configuration-file.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "storage/storage-point.h"

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
	QDomElement groupsNode = xml_config_file->getNode("Groups", XmlConfigFile::ModeFind);
	if (groupsNode.isNull())
	{
		importConfiguration();
		setState(StateLoaded);
		return;
	}

	SimpleManager<Group>::load();
}

Group GroupManager::byName(const QString &name, bool create)
{
	if (name.isEmpty())
		return Group::null;

	ensureLoaded();

	foreach (Group group, items())
		if (name == group.name())
			return group;

	if (!create)
		return Group::null;

	Group group = Group::create();
	group.data()->importConfiguration(name);
	addItem(group);

	return group;
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

void GroupManager::itemAboutToBeAdded(Group item)
{
	emit groupAboutToBeAdded(item);
}

void GroupManager::itemAdded(Group item)
{
	emit groupAdded(item);
}

void GroupManager::itemAboutToBeRemoved(Group item)
{
	emit groupAboutToBeRemoved(item);
}

void GroupManager::itemRemoved(Group item)
{
	emit groupRemoved(item);
}