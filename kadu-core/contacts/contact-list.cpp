/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contact-list.h"

ContactList::ContactList()
{
}

ContactList::ContactList(Contact contact)
{
	append(contact);
}

bool ContactList::operator == (ContactList &compare)
{
	qSort(*this);
	qSort(compare);

	return QList<Contact>::operator == (compare);
}
