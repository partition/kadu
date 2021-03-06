/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef TLEN_CONTACT_DETAILS
#define TLEN_CONTACT_DETAILS

#include "contacts/contact.h"
#include "contacts/contact-details.h"

class Contact;

class TlenContactDetails : public ContactDetails
{
	PROPERTY_DEC(unsigned int, TlenProtocolVersion)

	PROPERTY_DEC(unsigned int, LookingFor)
	PROPERTY_DEC(unsigned int, Job)
	PROPERTY_DEC(unsigned int, TodayPlans)
	PROPERTY_DEC(bool, ShowStatus)
	PROPERTY_DEC(bool, HaveMic)
	PROPERTY_DEC(bool, HaveCam)

protected:
	virtual void load();

public:
	explicit TlenContactDetails(ContactShared *contactShared);
	virtual ~TlenContactDetails();

	virtual bool validateId();

	virtual void store();

	PROPERTY_DEF(unsigned int, tlenProtocolVersion, setTlenProtocolVersion, TlenProtocolVersion)

	PROPERTY_DEF(unsigned int, lookingFor, setLookingFor, LookingFor)
	PROPERTY_DEF(unsigned int, job, setJob, Job)
	PROPERTY_DEF(unsigned int, todayPlans, setTodayPlans, TodayPlans)
	PROPERTY_DEF(bool, showStatus, setShowStatus, ShowStatus)
	PROPERTY_DEF(bool, haveMic, setHaveMic, HaveMic)
	PROPERTY_DEF(bool, haveCam, setHaveCam, HaveCam)
};

#endif // TLEN_CONTACT_DETAILS
