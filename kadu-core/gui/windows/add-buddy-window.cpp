/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#ifdef Q_WS_MAEMO_5
# include <QtGui/QResizeEvent>
# include <QtGui/QScrollArea>
#endif
#include <QtGui/QSortFilterProxyModel>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "accounts/filter/id-validity-filter.h"
#include "accounts/filter/protocol-filter.h"
#include "accounts/filter/writeable-contacts-list-filter.h"
#include "accounts/model/accounts-model.h"
#include "accounts/model/accounts-proxy-model.h"
#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/model/groups-model.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/accounts-combo-box.h"
#include "gui/widgets/groups-combo-box.h"
#include "gui/widgets/select-buddy-combo-box.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "protocols/services/roster-service.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "url-handlers/url-handler-manager.h"
#include "icons-manager.h"

#include "add-buddy-window.h"

AddBuddyWindow::AddBuddyWindow(QWidget *parent, const Buddy &buddy, bool forceBuddyAccount) :
		QDialog(parent, Qt::Window), UserNameLabel(0), UserNameEdit(0), MobileAccountAction(0), EmailAccountAction(0),
		AccountCombo(0), AccountComboIdFilter(0), GroupCombo(0), DisplayNameEdit(0), MergeBuddy(0),
		SelectBuddy(0), AskForAuthorization(0), AllowToSeeMeCheck(0), ErrorLabel(0), AddContactButton(0),
		MyBuddy(buddy), ForceBuddyAccount(forceBuddyAccount)
{
	setWindowRole("kadu-add-buddy");

	setAttribute(Qt::WA_DeleteOnClose);

	if (MyBuddy)
	{
		MyAccount = BuddyPreferredManager::instance()->preferredAccount(MyBuddy);
		if (!MyAccount)
			MyBuddy = Buddy::null;
	}

	createGui();
	if (!MyBuddy)
		addFakeAccountsToComboBox();
}

AddBuddyWindow::~AddBuddyWindow()
{
	saveWindowGeometry(this, "General", "AddBuddyWindowGeometry");
}

#ifdef Q_WS_MAEMO_5
void AddBuddyWindow::resizeEvent(QResizeEvent *event)
{
	ScrollArea->resize(event->size());
}
#endif

void AddBuddyWindow::createGui()
{
	loadWindowGeometry(this, "General", "AddBuddyWindowGeometry", 0, 50, 425, 430);

#ifdef Q_WS_MAEMO_5
	QWidget *mainWidget = new QWidget(this);

	ScrollArea = new QScrollArea(this);
	ScrollArea->setFrameStyle(QFrame::NoFrame);
	ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ScrollArea->setWidget(mainWidget);
	ScrollArea->setWidgetResizable(true);

	QGridLayout *layout = new QGridLayout(mainWidget);
#else
	QGridLayout *layout = new QGridLayout(this);
#endif

	UserNameEdit = new QLineEdit(this);

	if (MyBuddy)
	{
		UserNameEdit->setText(MyBuddy.id(MyAccount));
		UserNameEdit->hide();
	}
	else
	{
		connect(UserNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAddContactEnabled()));
		connect(UserNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAccountFilter()));

		UserNameLabel = new QLabel(this);
		layout->addWidget(UserNameLabel, 0, 0, Qt::AlignRight);
		layout->addWidget(UserNameEdit, 0, 1);
	}

	AccountCombo = new AccountsComboBox(MyBuddy.isNull(), this);
	AccountCombo->setIncludeIdInDisplay(true);

	AccountComboIdFilter = new IdValidityFilter(AccountCombo);
	AccountCombo->addFilter(AccountComboIdFilter);
	AccountCombo->addFilter(new WriteableContactsListFilter(AccountCombo));

	if (MyBuddy)
	{
		AccountCombo->setCurrentAccount(MyAccount);

		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *addingBuddyDescription = new QLabel(tr("Adding contact with ID <b>%1</b> in").arg(MyBuddy.id(MyAccount)), this);
		hLayout->addWidget(addingBuddyDescription, 0, Qt::AlignLeft);

		if (ForceBuddyAccount)
		{
			// NOTE: keep "%2 (%3)" consistent with AccountsModel::data() for DisplayRole, when IncludeIdInDisplay is true
			// TODO 0.8: remove such code duplication
			addingBuddyDescription->setText(addingBuddyDescription->text() + ' ' + tr("%1 account <b>%2 (%3)</b>")
					.arg(MyAccount.protocolHandler()->protocolFactory()->displayName(),
					MyAccount.accountIdentity().name(), MyAccount.id()));
			AccountCombo->hide();
		}
		else
		{
			ProtocolFilter *protocolFilter = new ProtocolFilter(AccountCombo);
			protocolFilter->setProtocolName(MyAccount.protocolName());
			AccountCombo->addFilter(protocolFilter);
			hLayout->addWidget(AccountCombo, 10, Qt::AlignLeft);
		}

		layout->addLayout(hLayout, 0, 0, 1, -1);
		layout->setRowMinimumHeight(0, 30);
	}
	else
	{
		layout->addWidget(new QLabel(tr("in"), this), 0, 2);
		layout->addWidget(AccountCombo, 0, 3);
	}

	layout->addWidget(new QLabel(tr("Add in group:"), this), 1, 0, Qt::AlignRight);
	GroupCombo = new GroupsComboBox(this);
	layout->addWidget(GroupCombo, 1, 1, 1, 3);

	layout->addWidget(new QLabel(tr("Visible name:"), this), 2, 0, Qt::AlignRight);
	DisplayNameEdit = new QLineEdit(this);
	layout->addWidget(DisplayNameEdit, 2, 1, 1, 1);
	if (MyBuddy)
	{
		DisplayNameEdit->setText(MyBuddy.display());
		DisplayNameEdit->setFocus();
	}
	connect(DisplayNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAddContactEnabled()));

	QLabel *hintLabel = new QLabel(tr("Enter a name for this buddy"));
	QFont hintLabelFont = hintLabel->font();
	hintLabelFont.setItalic(true);
	hintLabelFont.setPointSize(hintLabelFont.pointSize() - 2);
	hintLabel->setFont(hintLabelFont);
	layout->addWidget(hintLabel, 3, 1, 1, 3);

	MergeBuddy = new QCheckBox(tr("Merge with existing buddy"), this);
	layout->addWidget(MergeBuddy, 4, 1, 1, 3);

	QWidget *selectContactWidget = new QWidget(this);
	QHBoxLayout *selectContactLayout = new QHBoxLayout(selectContactWidget);
	selectContactLayout->addSpacing(20);
	SelectBuddy = new SelectBuddyComboBox(selectContactWidget);
	SelectBuddy->setEnabled(false);
	selectContactLayout->addWidget(SelectBuddy);
	layout->addWidget(selectContactWidget, 5, 1, 1, 3);

	AskForAuthorization = new QCheckBox(tr("Ask contact for authorization"), this);
	layout->addWidget(AskForAuthorization, 7, 1, 1, 3);

	AllowToSeeMeCheck = new QCheckBox(tr("Allow buddy to see me when I'm available"), this);
	AllowToSeeMeCheck->setChecked(true);
	layout->addWidget(AllowToSeeMeCheck, 8, 1, 1, 3);

	layout->setRowMinimumHeight(6, 20);
	layout->setRowMinimumHeight(9, 20);
	layout->setRowStretch(9, 100);

	ErrorLabel = new QLabel();
	QFont labelFont = ErrorLabel->font();
	labelFont.setBold(true);
	ErrorLabel->setFont(labelFont);
	layout->addWidget(ErrorLabel, 10, 1, 1, 4);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	layout->addWidget(buttons, 11, 0, 1, 4);

	AddContactButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Add buddy"), this);
	AddContactButton->setDefault(true);
	connect(AddContactButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancel = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(AddContactButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancel, QDialogButtonBox::DestructiveRole);

	if (MyBuddy)
		layout->setContentsMargins(30, 0, 0, 0);
	else
		layout->setColumnMinimumWidth(0, 140);
	layout->setColumnMinimumWidth(1, 200);

#ifndef Q_WS_MAEMO_5
	setFixedHeight(layout->minimumSize().height());
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
#endif

	connect(AccountCombo, SIGNAL(accountChanged(Account, Account)), this, SLOT(accountChanged(Account, Account)));
	connect(AccountCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGui()));
	connect(AccountCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setAddContactEnabled()));
	connect(MergeBuddy, SIGNAL(toggled(bool)), SelectBuddy, SLOT(setEnabled(bool)));
	connect(MergeBuddy, SIGNAL(toggled(bool)), DisplayNameEdit, SLOT(setDisabled(bool)));
	connect(MergeBuddy, SIGNAL(toggled(bool)), AllowToSeeMeCheck, SLOT(setDisabled(bool)));
	connect(MergeBuddy, SIGNAL(toggled(bool)), this, SLOT(setAddContactEnabled()));
	connect(SelectBuddy, SIGNAL(buddyChanged(Buddy)), this, SLOT(setAddContactEnabled()));
	connect(SelectBuddy, SIGNAL(buddyChanged(Buddy)), this, SLOT(setAccountFilter()));

	setAddContactEnabled();
	setAccountFilter();
	accountChanged(MyAccount, Account::null);
	updateGui();
}

void AddBuddyWindow::addFakeAccountsToComboBox()
{
	ActionsProxyModel *actionsModel = AccountCombo->actionsModel();

	MobileAccountAction = new QAction(IconsManager::instance()->iconByPath("phone"), tr("Mobile"), AccountCombo);
	actionsModel->addAfterAction(MobileAccountAction);

	EmailAccountAction = new QAction(IconsManager::instance()->iconByPath("mail-message-new"), tr("E-mail"), AccountCombo);
	actionsModel->addAfterAction(EmailAccountAction);
}

void AddBuddyWindow::displayErrorMessage(const QString &message)
{
	ErrorLabel->setText(message);
}

void AddBuddyWindow::setGroup(Group group)
{
	GroupCombo->setCurrentGroup(group);
}

bool AddBuddyWindow::isMobileAccount()
{
	return (MobileAccountAction && AccountCombo->data(ActionRole).value<QAction *>() == MobileAccountAction);
}

bool AddBuddyWindow::isEmailAccount()
{
	return (EmailAccountAction && AccountCombo->data(ActionRole).value<QAction *>() == EmailAccountAction);
}

void AddBuddyWindow::accountChanged(Account account, Account lastAccount)
{
	if (lastAccount && lastAccount.protocolHandler())
	{
		disconnect(lastAccount.protocolHandler(), SIGNAL(connected(Account)), this, SLOT(setAddContactEnabled()));
		disconnect(lastAccount.protocolHandler(), SIGNAL(disconnected(Account)), this, SLOT(setAddContactEnabled()));
	}

	if (!account || !account.protocolHandler() || !account.protocolHandler()->rosterService())
	{
		AskForAuthorization->setEnabled(false);
		AskForAuthorization->setChecked(false);
	}
	else
	{
		connect(account.protocolHandler(), SIGNAL(connected(Account)), this, SLOT(setAddContactEnabled()));
		connect(account.protocolHandler(), SIGNAL(disconnected(Account)), this, SLOT(setAddContactEnabled()));

		AskForAuthorization->setEnabled(true);
		AskForAuthorization->setChecked(true);
	}
}

void AddBuddyWindow::updateAccountGui()
{
	if (UserNameLabel)
	{
		Account account = AccountCombo->currentAccount();
		if (account.isNull())
			UserNameLabel->setText(tr("User ID:"));
		else
			UserNameLabel->setText(account.protocolHandler()->protocolFactory()->idLabel());
	}

	MergeBuddy->setEnabled(true);
	AllowToSeeMeCheck->setEnabled(true);
}

void AddBuddyWindow::updateMobileGui()
{
	UserNameLabel->setText(tr("Mobile number:"));
	MergeBuddy->setChecked(false);
	MergeBuddy->setEnabled(false);
	SelectBuddy->setCurrentBuddy(Buddy::null);
	AllowToSeeMeCheck->setEnabled(false);
}

void AddBuddyWindow::updateEmailGui()
{
	UserNameLabel->setText(tr("E-mail address:"));
	MergeBuddy->setChecked(false);
	MergeBuddy->setEnabled(false);
	SelectBuddy->setCurrentBuddy(Buddy::null);
	AllowToSeeMeCheck->setEnabled(false);
}

void AddBuddyWindow::updateGui()
{
	if (isMobileAccount())
		updateMobileGui();
	else if (isEmailAccount())
		updateEmailGui();
	else
		updateAccountGui();
}

void AddBuddyWindow::validateData()
{
	AddContactButton->setEnabled(false);

	Account account = AccountCombo->currentAccount();
	if (account.isNull() || !account.protocolHandler() || !account.protocolHandler()->protocolFactory())
	{
		displayErrorMessage(tr("Account is not selected"));
		return;
	}

	if (account.protocolHandler()->rosterService() && !account.protocolHandler()->isConnected())
	{
		displayErrorMessage(tr("You must be connected to add contacts to this account"));
		return;
	}

	if (account.protocolHandler()->protocolFactory()->validateId(UserNameEdit->text()) != QValidator::Acceptable)
	{
		displayErrorMessage(tr("Entered username is invalid"));
		return;
	}

	Contact contact = ContactManager::instance()->byId(account, UserNameEdit->text(), ActionReturnNull);
	if (contact && contact.ownerBuddy() && !contact.ownerBuddy().isAnonymous())
	{
		displayErrorMessage(tr("This contact is already available as <i>%1</i>").arg(contact.ownerBuddy().display()));
		return;
	}

	if (MergeBuddy->isChecked())
	{
		if (!SelectBuddy->currentBuddy())
		{
			displayErrorMessage(tr("Select buddy to merge with"));
			return;
		}
	}
	else
	{
		Buddy existingBuddy = BuddyManager::instance()->byDisplay(DisplayNameEdit->text(), ActionReturnNull);
		if (existingBuddy && existingBuddy != MyBuddy)
		{
			displayErrorMessage(tr("Visible name is already used for another buddy"));
			return;
		}
	}

	AddContactButton->setEnabled(true);
	displayErrorMessage(QString());
}

void AddBuddyWindow::validateMobileData()
{
	static QRegExp mobileRegularExpression("[0-9]{3,12}");

	if (!mobileRegularExpression.exactMatch(UserNameEdit->text()))
	{
		displayErrorMessage(tr("Entered mobile number is invalid"));
		return;
	}

	if (MergeBuddy->isChecked())
	{
		displayErrorMessage(tr("Merging mobile number with buddy is not supported. Please use edit buddy window."));
		return;
	}

	AddContactButton->setEnabled(true);
	displayErrorMessage(QString());
}

void AddBuddyWindow::validateEmailData()
{
	if (!UrlHandlerManager::instance()->mailRegExp().exactMatch(UserNameEdit->text()))
	{
		displayErrorMessage(tr("Entered e-mail is invalid"));
		return;
	}

	if (MergeBuddy->isChecked())
	{
		displayErrorMessage(tr("Merging e-mail with buddy is not supported. Please use edit buddy window."));
		return;
	}

	AddContactButton->setEnabled(true);
	displayErrorMessage(QString());
}

void AddBuddyWindow::setAddContactEnabled()
{
	if (isMobileAccount())
		validateMobileData();
	else if (isEmailAccount())
		validateEmailData();
	else
		validateData();
}

void AddBuddyWindow::setAccountFilter()
{
	AccountComboIdFilter->setId(UserNameEdit->text());
}

bool AddBuddyWindow::addContact()
{
	Account account = AccountCombo->currentAccount();
	if (account.isNull())
		return false;

	Buddy buddy;

	if (!MergeBuddy->isChecked())
	{
		if (MyBuddy.isNull())
		{
			buddy = Buddy::create();
			buddy.data()->setState(StorableObject::StateNew);
		}
		else
			buddy = MyBuddy;

		BuddyManager::instance()->addItem(buddy);

		buddy.setAnonymous(false);
		buddy.setOfflineTo(!AllowToSeeMeCheck->isChecked());

		QString display = DisplayNameEdit->text().isEmpty() ? UserNameEdit->text() : DisplayNameEdit->text();
		buddy.setDisplay(display);
	}
	else
	{
		buddy = SelectBuddy->currentBuddy();
		if (buddy.isNull())
			return false;
	}

	Contact contact = ContactManager::instance()->byId(account, UserNameEdit->text(), ActionCreateAndAdd);
	// force reattach for gadu protocol, even if buddy == contact.ownerBuddy()
	contact.setOwnerBuddy(Buddy::null);
	contact.setOwnerBuddy(buddy);

	buddy.addToGroup(GroupCombo->currentGroup());

	if (!buddy.isOfflineTo())
		sendAuthorization(contact);

	if (AskForAuthorization->isChecked())
		askForAuthorization(contact);

	return true;
}

bool AddBuddyWindow::addMobile()
{
	Buddy buddy = Buddy::create();
	buddy.data()->setState(StorableObject::StateNew);
	buddy.setAnonymous(false);
	buddy.setMobile(UserNameEdit->text());
	buddy.setDisplay(DisplayNameEdit->text().isEmpty() ? UserNameEdit->text() : DisplayNameEdit->text());
	buddy.addToGroup(GroupCombo->currentGroup());

	BuddyManager::instance()->addItem(buddy);

	return true;
}

bool AddBuddyWindow::addEmail()
{
	Buddy buddy = Buddy::create();
	buddy.data()->setState(StorableObject::StateNew);
	buddy.setAnonymous(false);
	buddy.setEmail(UserNameEdit->text());
	buddy.setDisplay(DisplayNameEdit->text().isEmpty() ? UserNameEdit->text() : DisplayNameEdit->text());
	buddy.addToGroup(GroupCombo->currentGroup());

	BuddyManager::instance()->addItem(buddy);

	return true;
}

void AddBuddyWindow::accept()
{
	bool ok;

	if (isMobileAccount())
		ok = addMobile();
	else if (isEmailAccount())
		ok = addEmail();
	else
		ok = addContact();

	if (ok)
		QDialog::accept();
}

void AddBuddyWindow::reject()
{
	QDialog::reject();
}

void AddBuddyWindow::askForAuthorization(const Contact &contact)
{
	Account account = AccountCombo->currentAccount();

	if (!account || !account.protocolHandler() || !account.protocolHandler()->rosterService())
		return;

	account.protocolHandler()->rosterService()->askForAuthorization(contact);
}

void AddBuddyWindow::sendAuthorization(const Contact &contact)
{
	Account account = AccountCombo->currentAccount();

	if (!account || !account.protocolHandler() || !account.protocolHandler()->rosterService())
		return;

	account.protocolHandler()->rosterService()->sendAuthorization(contact);
}
