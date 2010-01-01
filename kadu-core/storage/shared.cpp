/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "shared.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @param uuid objects uuid
 * @short Contructs empty object with given (or generated) uuid.
 *
 * Contructs empty object with given uuid. When uuid is invalid (NULL)
 * new uuid is created and assigned to object.
 */
Shared::Shared(QUuid uuid) :
		Uuid(uuid.isNull() ? QUuid::createUuid() : uuid)
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Destroys object.
 *
 * Destroys object.
 */
Shared::~Shared()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads object from storage.
 *
 * Loads obejct from storage. Loads uuid from uuid attribute. Superclass method
 * is also called. You must call this method in load methods of derivered class.
 */
void Shared::load()
{
	if (!isValidStorage())
		return;

	UuidStorableObject::load();
	Uuid = QUuid(loadAttribute<QString>("uuid"));
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores object to storage.
 *
 * Storeas obejct to storage. Stored uuid to uuid attribute.
 * You must call this method in load methods of derivered class.
 */
void Shared::store()
{
	if (!isValidStorage())
		return;

	storeAttribute("uuid", Uuid.toString());
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Method called just before this obejct is removed from storage.
 *
 * Method is called just before this obejct is removed from storage.
 * Override it when you need to take special actions (like removing files from disk).
 * If this object stored references to other SharedBase objects you must set them
 * all to null value or remove them (if stored in list).
 */
void Shared::aboutToBeRemoved()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Call this method to block calling of emitUpdated method.
 *
 * Calling this method blocks calling of emitUpdated when data stored in this object is
 * changed. Homever, if data is changed after calling this method and unblockUpdatedSignal
 * is called, emitUpdated is called. If blockUpdatedSignal is called more than once
 * you have to call unblockUpdatedSignal the same amount to get emitUpdated called.
 */
void Shared::blockUpdatedSignal()
{
	if (0 == BlockUpdatedSignalCount)
		Updated = false;
	BlockUpdatedSignalCount++;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Call this method to unblock calling of emitUpdated method.
 *
 * Calling this method unblocks calling of emitUpdated when data stored in this object is
 * changed. If data was changed after call of blockUpdatedSignal this method calls emitUpdated.
 * If blockUpdatedSignal was called more than once this method must be calles the same
 * amount to get emitUpdated called.
 */
void Shared::unblockUpdatedSignal()
{
	BlockUpdatedSignalCount--;
	emitUpdated();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Call this method when any data was changed.
 *
 * Method should be called after any data in this object was changed. Setters implemented by
 * KaduShared_PropertyWrite, KaduShared_Property, KaduShared_PropertyBoolWrite calls this
 * method by default.
 */
void Shared::dataUpdated()
{
	Updated = true;
	doEmitUpdated();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Calls emitUpdated and changed Updated flag.
 *
 * Calls emitUpdated when no blockUpdatedSignal was called without correspondent unblockUpdatedSignal
 * and any data was changed (Updated flag is set to true). Then resets Updated flag to false.
 */
void Shared::doEmitUpdated()
{
	if (0 == BlockUpdatedSignalCount && Updated)
	{
		emitUpdated();
		Updated = false;
	}
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Method called when any data was changed.
 *
 * Method called when any data was changed. Reimplement it to emit signal from you class when needed.
 */
void Shared::emitUpdated()
{
}