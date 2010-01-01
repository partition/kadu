/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STORAGE_POINT_H
#define STORAGE_POINT_H

#include <QtXml/QDomElement>

/**
 * @defgroup Storage Kadu Storage API
 *
 * This group contains classes and templates that allows to store and load
 * data from XML file. This includes support for Managers (@link<Manager> Manager @endlink
 * and @link<SimpleManager> SimpleManager @endlink) that automates all basic tasks
 * (loading, storing, counting) and @link<Details> Details @endlink and @link<DetailsHolder>
 * DetailsHolder @endlink that allows to separate data for each object into two
 * different class (one for common data for all instances and one for data specific
 * to given implementation of that class).
 */

/**
 * @addtogroup Storage
 * @{
 */

class XmlConfigFile;

/**
 * @class StoragePoint
 * @author Rafal 'Vogel' Malinowski
 * @short Unique place that can store any data.
 *
 * StoragePoint provides unique place that can store any data
 * Storage point consists of two parts:
 * <ul>
 *   <li>storage - pointer to XML file that contains persistent data</li>
 *   <li>point - DOM node that is parent of all nodes this object uses for storage</li>
 * </ul>
 *
 * Storage points are created in @link<StorableObject> StorableObject @endlink class in
 * @link<createStoragePoint> createStoragePoint @endlink method. You generally should not
 * create instances of StoragePoint outside these classes. Consider derivering from them instead.
 */
class StoragePoint
{
	XmlConfigFile *Storage;
	QDomElement Point;

public:
	StoragePoint(XmlConfigFile *storage, QDomElement point);

	XmlConfigFile * storage();
	QDomElement point();

};

/**
 * @}
 */

#endif // STORAGE_POINT_H