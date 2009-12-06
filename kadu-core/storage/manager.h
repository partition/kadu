/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MANAGER_H
#define MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "configuration/configuration-manager.h"
#include "storage/storable-object.h"

#include "exports.h"

template<class Item>
class KADUAPI Manager : public StorableObject
{
	QList<Item> Items;
	QList<Item> ItemsWithDetails;

protected:
	Manager()
	{
		setState(StateNotLoaded);
		ConfigurationManager::instance()->registerStorableObject(this);
	}

	virtual ~Manager()
	{
		ConfigurationManager::instance()->unregisterStorableObject(this);
	}

	virtual StorableObject * storageParent()
	{
		return 0;
	}

	void registerItem(Item item)
	{
		if (ItemsWithDetails.contains(item))
			return;
		if (!Items.contains(item))
			return;

		itemAboutToBeRegistered(item);
		ItemsWithDetails.append(item);
		itemRegisterd(item);
	}

	void unregisterItem(Item item)
	{
		if (!ItemsWithDetails.contains(item))
			return;
		if (!Items.contains(item))
			return;

		itemAboutToBeUnregisterd(item);
		ItemsWithDetails.removeAll(item);
		itemUnregistered(item);
	}

	virtual QString storageNodeItemName() = 0;

	virtual void itemAboutToBeAdded(Item item) {}
	virtual void itemAboutToBeRemoved(Item item) {}

	virtual void itemAboutToBeRegistered(Item item) {}
	virtual void itemRegisterd(Item item) {}
	virtual void itemAboutToBeUnregisterd(Item item) {}
	virtual void itemUnregistered(Item item) {}

	virtual void load()
	{
		if (!isValidStorage())
			return;

		StorableObject::load();

		QDomElement itemsNode = storage()->point();
		if (itemsNode.isNull())
			return;

		QList<QDomElement> itemElements = storage()->storage()->getNodes(itemsNode, storageNodeItemName());
		foreach (QDomElement itemElement, itemElements)
		{
			StoragePoint *storagePoint = new StoragePoint(storage()->storage(), itemElement);
			Item item = Item::loadFromStorage(storagePoint);

			addItem(item);
		}
	}

public:
	virtual void store()
	{
		ensureLoaded();

		foreach (Item item, Items)
			item.store();
	}

	Item byIndex(unsigned int index)
	{
		ensureLoaded();

		if (index < 0 || index >= count())
			return Item::null;

		return ItemsWithDetails.at(index);
	}

	Item byUuid(const QUuid &uuid)
	{
		ensureLoaded();

		foreach (Item item, Items)
			if (item.uuid() == uuid)
				return item;

		return Item::null;
	}

	unsigned int indexOf(Item item)
	{
		ensureLoaded();
		return ItemsWithDetails.indexOf(item);
	}

	unsigned int count()
	{
		ensureLoaded();
		return ItemsWithDetails.count();
	}

	const QList<Item> allItems()
	{
		ensureLoaded();
		return Items;
	}

	const QList<Item> items()
	{
		ensureLoaded();
		return ItemsWithDetails;
	}

	void addItem(Item item)
	{
		ensureLoaded();

		if (Items.contains(item))
			return;

		itemAboutToBeAdded(item);

		Items.append(item);
		if (item.details())
			registerItem(item);
	}

	void removeItem(Item item)
	{
		ensureLoaded();

		if (!Items.contains(item))
			return;

		itemAboutToBeRemoved(item);

		Items.removeAll(item);
		if (item.details())
			unregisterItem(item);

		item.remove();
	}

};

#endif // MANAGER_H