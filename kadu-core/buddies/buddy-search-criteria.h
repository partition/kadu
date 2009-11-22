/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_SEARCH_CRITERIA_H
#define BUDDY_SEARCH_CRITERIA_H

#include "contacts/contact.h"
#include "buddies/buddy.h"

struct KADUAPI BuddySearchCriteria
{
	Buddy SearchBuddy;
	QString BirthYearFrom;
	QString BirthYearTo;
	bool Active;
	bool IgnoreResults;

	BuddySearchCriteria();
	virtual ~BuddySearchCriteria();

	void reqUin(Account account, const QString& uin);
	void reqFirstName(const QString& firstName);
	void reqLastName(const QString& lastName);
	void reqNickName(const QString& nickName);
	void reqCity(const QString& city);
	void reqBirthYear(const QString& birthYearFrom, const QString& birthYearTo);
	void reqGender(bool female);
	void reqActive();

	void clearData();

};

#endif // BUDDY_SEARCH_CRITERIA_H