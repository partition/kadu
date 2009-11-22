 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUP_BUDDY_FILTER
#define GROUP_BUDDY_FILTER

#include "buddies/group.h"

#include "abstract-buddy-filter.h"

class GroupBuddyFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	Group CurrentGroup;
	bool AllGroupShown;

public:
	GroupBuddyFilter(QObject *parent = 0);

	void setGroup(Group group);
	virtual bool acceptBuddy(Buddy buddy);

	void refresh();

	void setAllGroupShown(bool shown);
};

#endif // GROUP_BUDDY_FILTER