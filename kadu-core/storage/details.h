/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DETAILS_H
#define DETAILS_H

#include "storage/storable-object.h"

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @class Details
 * @author Rafal 'Vogel' Malinowski
 * @param T class type, derivered from @link<StorableObject> StorableObject @endlink
 * @short Object that can extend any DetailsHolder by protocol data.
 *
 * Some object (@link<Account> Account @endlink, @link<Contact> Contact @endlink and others)
 * have common data that should be always available and protocol data that can only be loaded
 * when protocol plugin is loaded. Common data is stored in normal @link<StorableObject>
 * StorableObject @endlink that can be extended by @link<DetailsHolder> DetailsHolder @endlink
 * class that holds object of Details type.
 *
 * Object of Details type is always attached to @link<StorableObject> StorableObject @endlink
 * and uses exactly the same @link<StoragePoint> StoragePoint @endlink (so these two object
 * share XML node for storing data). All other data (like State) is not shared between these
 * objects.
 */
template<typename T>
class Details : public StorableObject
{
	T *MainData;

public:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Construct new object with cloned StoragePoint and state 'StateNotLoaded'.
	 * @param mainData @link<StorableObject> StorableObject to clone storagePoint from
	 *
	 * Construct new object with StoragePoint cloned from mainData object and state set to 'StateNotLoaded'.
	 */
	explicit Details(T *mainData) :
			MainData(mainData)
	{
		setStorage(mainData->storage());
		setState(StateNotLoaded);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Empty destructor.
	 *
	 * Empty destructor.
	 */
	virtual ~Details() {}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns main data object for this Details.
	 * @return main data object for this Details
	 *
	 * Returns main data object (object, with the same StoragePoint) for this Details.
	 */
	T * mainData()
	{
		return MainData;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns storage parent of main data object for this Details.
	 * @return storage parent of main data object for this Details
	 *
	 * Returns storage parent of main data object for this Details.
	 */
	virtual StorableObject * storageParent()
	{
		return MainData
				? MainData->storageParent()
				: 0;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns storage node name of main data object for this Details.
	 * @return storage node name of main data object for this Details
	 *
	 * Returns storage node name of main data object for this Details.
	 */
	virtual QString storageNodeName()
	{
		return MainData
				? MainData->storageNodeName()
				: QString::null;
	}

};

/**
* @}
*/

#endif // DETAILS_H