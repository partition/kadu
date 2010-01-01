/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"

#include "group-manager.h"

#include "group.h"

KaduSharedBaseClassImpl(Group)

Group Group::null;

Group Group::create()
{
	return new GroupShared();
}

Group Group::loadFromStorage(StoragePoint *contactStoragePoint)
{
	return GroupShared::loadFromStorage(contactStoragePoint);
}

Group::Group()
{
}

Group::Group(GroupShared *data) :
		SharedBase<GroupShared>(data)
{
	data->ref.ref();
}

Group::Group(QObject *data)
{
	GroupShared *shared = dynamic_cast<GroupShared *>(data);
	if (shared)
		setData(shared);
}

Group::Group(const Group&copy) :
		SharedBase<GroupShared>(copy)
{
}

Group::~Group()
{
}

KaduSharedBase_PropertyDef(Group, QString, name, Name, QString::null)
KaduSharedBase_PropertyDef(Group, QString, icon, Icon, QString::null)
KaduSharedBase_PropertyDef(Group, bool, notifyAboutStatusChanges, NotifyAboutStatusChanges, false)
KaduSharedBase_PropertyDef(Group, bool, showInAllGroup, ShowInAllGroup, false)
KaduSharedBase_PropertyDef(Group, bool, offlineToGroup, OfflineToGroup, false)
KaduSharedBase_PropertyDef(Group, bool, showIcon, ShowIcon, false)
KaduSharedBase_PropertyDef(Group, bool, showName, ShowName, false)
KaduSharedBase_PropertyDef(Group, int, tabPosition, TabPosition, -1)