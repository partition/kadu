/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef GADU_CONTACT_PERSONAL_INFO_WIDGET_H
#define GADU_CONTACT_PERSONAL_INFO_WIDGET_H

#include <QtGui/QWidget>

#include "contacts/contact.h"

#include "exports.h"

class QComboBox;
class QLabel;

class GaduContactPersonalInfoWidget : public QWidget
{
	Q_OBJECT

	QLabel *FirstNameText;
	QLabel *LastNameText;
	QLabel *NicknameText;
	QLabel *GenderText;
	QLabel *BirthdateText;
	QLabel *CityText;
	QLabel *StateProvinceText;
	QLabel *IpText;
	QLabel *PortText;
	QLabel *DnsNameText;
	QLabel *ProtocolVerText;

	Contact MyContact;
	void createGui();
	void reset();
	
private slots:
	void personalInfoAvailable(Buddy buddy);

public:
	explicit GaduContactPersonalInfoWidget(Contact &contact, QWidget *parent = 0);
	virtual ~GaduContactPersonalInfoWidget();

};

#endif // GADU_CONTACT_PERSONAL_INFO_WIDGET_H
