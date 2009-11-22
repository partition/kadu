/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLineEdit>

#include "configuration/configuration-contact-data-manager.h"
#include "contacts/contact.h"
#include "misc/misc.h"

#include "contact-widget.h"

ContactWidget::ContactWidget(ConfigurationContactDataManager *dataManager, Contact contact, QWidget *parent)
	: ConfigurationWidget(dataManager, parent), Data(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

ContactWidget::~ContactWidget()
{
}

void ContactWidget::createGui()
{
	appendUiFile(dataPath("kadu/configuration/contact.ui"));

	QLineEdit *addrLineEdit = dynamic_cast<QLineEdit *>(widgetById("Addr"));
	addrLineEdit->setReadOnly(true);
	addrLineEdit->setBackgroundRole(QPalette::Button);

	QLineEdit *versionLineEdit = dynamic_cast<QLineEdit *>(widgetById("Version"));
	versionLineEdit->setReadOnly(true);
	versionLineEdit->setBackgroundRole(QPalette::Button);

	QLineEdit *dnsLineEdit = dynamic_cast<QLineEdit *>(widgetById("DnsName"));
	dnsLineEdit->setReadOnly(true);
	dnsLineEdit->setBackgroundRole(QPalette::Button);

	QLineEdit *statusLineEdit = dynamic_cast<QLineEdit *>(widgetById("Status"));
	statusLineEdit->setReadOnly(true);
	statusLineEdit->setBackgroundRole(QPalette::Button);
}