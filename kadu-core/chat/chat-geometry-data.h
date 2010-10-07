/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CHAT_GEOMETRY_DATA_H
#define CHAT_GEOMETRY_DATA_H

#include <QtCore/QRect>

#include "storage/module-data.h"

#undef Property
#define Property(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; } \
	void set##capitalized_name(const type &name) { ensureLoaded(); capitalized_name = name; }

class ChatGeometryData : public ModuleData
{
	QRect WindowGeometry;
	QList<int> WidgetVerticalSizes;
	QList<int> WidgetHorizontalSizes;

protected:
	virtual void load();

public:
	ChatGeometryData(StorableObject *parent);
	virtual ~ChatGeometryData();

	virtual void store();
	virtual QString name() const;

	Property(QRect, windowGeometry, WindowGeometry)
	Property(QList<int>, widgetVerticalSizes, WidgetVerticalSizes)
	Property(QList<int>, widgetHorizontalSizes, WidgetHorizontalSizes)
};

#endif // CHAT_GEOMETRY_DATA_H