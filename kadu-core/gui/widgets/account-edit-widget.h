/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef ACCOUNT_EDIT_WIDGET_H
#define ACCOUNT_EDIT_WIDGET_H

#include <QtGui/QWidget>

#include "accounts/account.h"
#include "gui/widgets/modal-configuration-widget.h"
#include "exports.h"

class KADUAPI AccountEditWidget : public ModalConfigurationWidget
{
	Q_OBJECT

	Account MyAccount;

protected:
	Account account() { return MyAccount; }

public:
	explicit AccountEditWidget(Account account, QWidget *parent = 0) :
			ModalConfigurationWidget(parent), MyAccount(account) {}
	virtual ~AccountEditWidget() {}

};

#endif // ACCOUNT_EDIT_WIDGET_H
