/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QLocale>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-kadu-data.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "buddies/group-manager.h"
#include "buddies/filter/blocked-buddy-filter.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "buddies/filter/has-description-buddy-filter.h"
#include "buddies/filter/offline-buddy-filter.h"
#include "buddies/filter/online-and-description-buddy-filter.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/buddies-list-view-delegate-configuration.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/status-menu.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/buddy-data-window.h"
#include "gui/windows/buddy-delete-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/merge-buddies-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/search-window.h"
#include "gui/windows/your-accounts.h"
#include "misc/misc.h"
#include "os/generic/url-opener.h"
#include "parser/parser.h"
#include "status/status-changer-manager.h"
#include "status/status-container-manager.h"
#include "url-handlers/url-handler-manager.h"

#include "about.h"
#include "debug.h"
#include "modules.h"

#include "kadu-window-actions.h"

void disableContainsSelfUles(Action *action)
{
	if (action->buddies().contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
}

void checkBuddyProperties(Action *action)
{
	kdebugf();

	if (!action->contact().isNull() && action->contact().ownerBuddy().isAnonymous())
	{
		action->setIcon(IconsManager::instance()->iconByPath("contact-new"));
		action->setText(qApp->translate("KaduWindowActions", "Add Buddy"));
	}
	else
	{
		action->setText(qApp->translate("KaduWindowActions", "View Buddy Properties"));
		action->setIcon(IconsManager::instance()->iconByPath("x-office-address-book"));
	}

	kdebugf2();
}

void checkHideDescription(Action *action)
{
	action->setEnabled(true);

	bool on = false;
	foreach (const Buddy buddy, action->buddies())
	{
		BuddyKaduData *ckd = 0;
		if (buddy.data())
			ckd = buddy.data()->moduleStorableData<BuddyKaduData>("kadu", false);
		if (!ckd)
			continue;

		if (ckd->hideDescription())
		{
			on = true;
			break;
		}
	}

	action->setChecked(on);
}

void disableNoContact(Action *action)
{
	action->setEnabled(action->contact());
}

void disableNoDescription(Action *action)
{
	action->setEnabled(!action->contact().currentStatus().description().isEmpty());
}

void disableNoDescriptionUrl(Action *action)
{
	action->setEnabled(action->contact().currentStatus().description().indexOf(UrlHandlerManager::instance()->urlRegExp()) >= 0);
}

void disableNoEMail(Action *action)
{
	kdebugf();

	if (action->contacts().count() != 1)
	{
		action->setEnabled(false);
		return;
	}

	const Buddy buddy = action->contact().ownerBuddy();

	if (buddy.email().isEmpty() || buddy.email().indexOf(UrlHandlerManager::instance()->mailRegExp()) < 0)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

KaduWindowActions::KaduWindowActions(QObject *parent) : QObject(parent)
{
	Configuration = new ActionDescription(this,
		ActionDescription::TypeGlobal, "configurationAction",
		this, SLOT(configurationActionActivated(QAction *, bool)),
		"preferences-other", "preferences-other", tr("Preferences...")
	);
	Configuration->setShortcut("kadu_configure", Qt::ApplicationShortcut);

	ShowYourAccounts = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "yourAccountsAction",
		this, SLOT(yourAccountsActionActivated(QAction *, bool)),
		"x-office-address-book", "x-office-address-book", tr("Your Accounts...")
	);

	ManageModules = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "manageModulesAction",
		ModulesManager::instance(), SLOT(showWindow(QAction *, bool)),
		"kadu_icons/kadu-modmanager", "kadu_icons/kadu-modmanager", tr("Plugins...")
	);
	ManageModules->setShortcut("kadu_modulesmanager", Qt::ApplicationShortcut);

	ExitKadu = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "exitKaduAction",
		this, SLOT(exitKaduActionActivated(QAction *, bool)),
		"application-exit", "application-exit", tr("&Quit")
	);
	ExitKadu->setShortcut("kadu_exit", Qt::ApplicationShortcut);

	AddUser = new ActionDescription(this,
		ActionDescription::TypeGlobal, "addUserAction",
		this, SLOT(addUserActionActivated(QAction *, bool)),
		"contact-new", "contact-new", tr("Add Buddy...")
	);
	AddUser->setShortcut("kadu_adduser", Qt::ApplicationShortcut);

	AddGroup= new ActionDescription(this,
		ActionDescription::TypeGlobal, "addGroupAction",
		this, SLOT(addGroupActionActivated(QAction *, bool)),
		//TODO 0.6.6 proper icon
		"contact-new", "contact-new", tr("Add Group...")
	);

	OpenSearch = new ActionDescription(this,
		ActionDescription::TypeGlobal, "openSearchAction",
		this, SLOT(openSearchActionActivated(QAction *, bool)),
		"edit-find", "edit-find", tr("Search for Buddy...")
	);

	Help = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "helpAction",
		this, SLOT(helpActionActivated(QAction *, bool)),
		"help-contents", "help-contents", tr("Getting H&elp")
	);

	Bugs = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "bugsAction",
		this, SLOT(bugsActionActivated(QAction *, bool)),
		"", "", tr("Report a Bug...")
	);

	Support = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "supportAction",
		this, SLOT(supportActionActivated(QAction *, bool)),
		"", "", tr("Support Us")
	);

	GetInvolved = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "getInvolvedAction",
		this, SLOT(getInvolvedActionActivated(QAction *, bool)),
		"", "", tr("Get Involved")
	);

	About = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "aboutAction",
		this, SLOT(aboutActionActivated(QAction *, bool)),
		"help-about", "help-about", tr("A&bout Kadu")
	);

	Translate = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "translateAction",
		this, SLOT(translateActionActivated(QAction *, bool)),
		"", "", tr("Translate Kadu...")
	);
// TODO 0.6.6: icon
	ShowInfoPanel = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "showInfoPanelAction",
		this, SLOT(showInfoPanelActionActivated(QAction *, bool)),
		"", "", tr("Show Information Panel"), true
	);
	connect(ShowInfoPanel, SIGNAL(actionCreated(Action *)), this, SLOT(showInfoPanelActionCreated(Action *)));

	ShowBlockedBuddies = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "showIgnoredAction",
		this, SLOT(showBlockedActionActivated(QAction *, bool)),
		"", "", tr("Show Blocked Buddies"), true
	);
	connect(ShowBlockedBuddies, SIGNAL(actionCreated(Action *)), this, SLOT(showBlockedActionCreated(Action *)));

	CopyDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "copyDescriptionAction",
		this, SLOT(copyDescriptionActionActivated(QAction *, bool)),
		"edit-copy", "edit-copy", tr("Copy Description"), false, "",
		disableNoDescription
	);
	BuddiesListViewMenuManager::instance()->addListActionDescription(CopyDescription, BuddiesListViewMenuItem::MenuCategoryActions, 10);

	CopyPersonalInfo = new ActionDescription(this,
		ActionDescription::TypeUser, "copyPersonalInfoAction",
		this, SLOT(copyPersonalInfoActionActivated(QAction *, bool)),
		"kadu_icons/kadu-copypersonal", "kadu_icons/kadu-copypersonal", tr("Copy Personal Info")
	);
	BuddiesListViewMenuManager::instance()->addListActionDescription(CopyPersonalInfo, BuddiesListViewMenuItem::MenuCategoryActions, 20);

	OpenDescriptionLink = new ActionDescription(this,
		ActionDescription::TypeUser, "openDescriptionLinkAction",
		this, SLOT(openDescriptionLinkActionActivated(QAction *, bool)),
		"go-jump", "go-jump", tr("Open Description Link in Browser..."), false, "",
		disableNoDescriptionUrl
	);
	BuddiesListViewMenuManager::instance()->addListActionDescription(OpenDescriptionLink, BuddiesListViewMenuItem::MenuCategoryActions, 30);

	WriteEmail = new ActionDescription(this,
		ActionDescription::TypeUser, "writeEmailAction",
		this, SLOT(writeEmailActionActivated(QAction *, bool)),
		"mail-message-new", "mail-message-new", tr("Send E-Mail"), false, "",
		disableNoEMail
	);
	BuddiesListViewMenuManager::instance()->addActionDescription(WriteEmail, BuddiesListViewMenuItem::MenuCategoryActions, 200);

	LookupUserInfo = new ActionDescription(this,
		ActionDescription::TypeUser, "lookupUserInfoAction",
		this, SLOT(lookupInDirectoryActionActivated(QAction *, bool)),
		"edit-find", "edit-find", tr("Search in Directory"), false, "",
		disableNoContact
	);

	HideDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "hideDescriptionAction",
		this, SLOT(hideDescriptionActionActivated(QAction *, bool)),
		"kadu_icons/kadu-descriptions_on", "kadu_icons/kadu-descriptions_off", tr("Hide Description"), true, "",
		checkHideDescription
	);

	InactiveUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "inactiveUsersAction",
		this, SLOT(inactiveUsersActionActivated(QAction *, bool)),
		"protocols/gadu-gadu/offline", "protocols/gadu-gadu/offline", tr("Show Offline Buddies"),
		true
	);
	connect(InactiveUsers, SIGNAL(actionCreated(Action *)), this, SLOT(inactiveUsersActionCreated(Action *)));
	InactiveUsers->setShortcut("kadu_showoffline");

	DescriptionUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "descriptionUsersAction",
		this, SLOT(descriptionUsersActionActivated(QAction *, bool)),
		"kadu_icons/kadu-showdescriptionusers_off", "kadu_icons/kadu-showdescriptionusers", tr("Hide Users Without Description"),
		true, tr("Show Users Without Description")
	);
	connect(DescriptionUsers, SIGNAL(actionCreated(Action *)), this, SLOT(descriptionUsersActionCreated(Action *)));
	DescriptionUsers->setShortcut("kadu_showonlydesc");

	ShowDescriptions = new ActionDescription(this,
		ActionDescription::TypeUserList, "descriptionsAction",
		this, SLOT(showDescriptionsActionActivated(QAction *, bool)),
		"kadu_icons/kadu-descriptions_off", "kadu_icons/kadu-descriptions_on", tr("Show descriptions"),
		true, tr("Hide descriptions")
	);
	connect(ShowDescriptions, SIGNAL(actionCreated(Action *)), this, SLOT(showDescriptionsActionCreated(Action *)));

	OnlineAndDescriptionUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "onlineAndDescriptionUsersAction",
		this, SLOT(onlineAndDescUsersActionActivated(QAction *, bool)),
		"kadu_icons/kadu-onoff_onlineandd_off", "kadu_icons/kadu-onoff_onlineandd", tr("Show Only Online and Description Users"),
		true, tr("Show all users")
	);
	connect(OnlineAndDescriptionUsers, SIGNAL(actionCreated(Action *)), this, SLOT(onlineAndDescUsersActionCreated(Action *)));

	EditUser = new ActionDescription(this,
		ActionDescription::TypeUser, "editUserAction",
		this, SLOT(editUserActionActivated(QAction *, bool)),
		"x-office-address-book", "x-office-address-book", tr("View Buddy Properties"), false, QString::null,
		checkBuddyProperties
	);
	connect(EditUser, SIGNAL(actionCreated(Action *)), this, SLOT(editUserActionCreated(Action *)));
	BuddiesListViewMenuManager::instance()->addActionDescription(EditUser, BuddiesListViewMenuItem::MenuCategoryView, 0);

	MergeContact = new ActionDescription(this,
		ActionDescription::TypeUser, "mergeContactAction",
		this, SLOT(mergeContactActionActivated(QAction *, bool)),
		"", "", tr("Merge Buddies...")
	);
	BuddiesListViewMenuManager::instance()->addActionDescription(MergeContact, BuddiesListViewMenuItem::MenuCategoryManagement, 100);

	BuddiesListViewMenuManager::instance()->addActionDescription(ChatWidgetManager::instance()->actions()->blockUser(), BuddiesListViewMenuItem::MenuCategoryManagement, 500);

	DeleteUsers = new ActionDescription(this,
		ActionDescription::TypeUser, "deleteUsersAction",
		this, SLOT(deleteUsersActionActivated(QAction *, bool)),
		"edit-delete", "edit-delete", tr("Delete Buddy...")
	);
	DeleteUsers->setShortcut("kadu_deleteuser");
	BuddiesListViewMenuManager::instance()->addActionDescription(DeleteUsers, BuddiesListViewMenuItem::MenuCategoryManagement, 1000);

	ShowStatus = new ActionDescription(this,
		ActionDescription::TypeGlobal, "openStatusAction",
		this, SLOT(showStatusActionActivated(QAction *, bool)),
		"protocols/gadu-gadu/offline", "protocols/gadu-gadu/offline", tr("Change Status")
	);
	connect(ShowStatus, SIGNAL(actionCreated(Action *)), this, SLOT(showStatusActionCreated(Action *)));

	UseProxy = new ActionDescription(this,
		ActionDescription::TypeGlobal, "useProxyAction",
		this, SLOT(useProxyActionActivated(QAction *, bool)),
		"kadu_icons/kadu-proxy", "kadu_icons/kadu-proxy_off", tr("Use Proxy"), true, tr("Don't Use Proxy")
	);
	connect(UseProxy, SIGNAL(actionCreated(Action *)), this, SLOT(useProxyActionCreated(Action *)));

	connect(StatusChangerManager::instance(), SIGNAL(statusChanged(StatusContainer *, Status)), this, SLOT(statusChanged(StatusContainer *, Status)));
}

KaduWindowActions::~KaduWindowActions()
{
}

void KaduWindowActions::statusChanged(StatusContainer *container, Status status)
{
	if (!container)
		return;

	// TODO: 0.6.6, this really SUXX
	QIcon icon = container->statusIcon(status).pixmap(16, 16);
	foreach (Action *action, ShowStatus->actions())
		action->setIcon(icon);
}

void KaduWindowActions::inactiveUsersActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parent());
	if (!window)
		return;
	if (!window->buddiesListView())
		return;

	bool enabled = config_file.readBoolEntry("General", "ShowOffline");
	OfflineBuddyFilter *ofcf = new OfflineBuddyFilter(action);
	ofcf->setEnabled(!enabled);

	action->setData(QVariant::fromValue(ofcf));
	action->setChecked(enabled);

	window->buddiesListView()->addFilter(ofcf);
}

void KaduWindowActions::descriptionUsersActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parent());
	if (!window)
		return;
	if (!window->buddiesListView())
		return;

	bool enabled = !config_file.readBoolEntry("General", "ShowWithoutDescription");
	HasDescriptionBuddyFilter *hdcf = new HasDescriptionBuddyFilter(action);
	hdcf->setEnabled(enabled);

	action->setData(QVariant::fromValue(hdcf));
	action->setChecked(enabled);

	window->buddiesListView()->addFilter(hdcf);
}

void KaduWindowActions::showDescriptionsActionCreated(Action *action)
{
	bool enabled = config_file.readBoolEntry("Look", "ShowDesc");
	action->setChecked(enabled);
}

void KaduWindowActions::onlineAndDescUsersActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parent());
	if (!window)
		return;
	if (!window->buddiesListView())
		return;

	bool enabled = config_file.readBoolEntry("General", "ShowOnlineAndDescription");
	OnlineAndDescriptionBuddyFilter *oadcf = new OnlineAndDescriptionBuddyFilter(action);
	oadcf->setEnabled(enabled);

	action->setData(QVariant::fromValue(oadcf));
	action->setChecked(enabled);

	window->buddiesListView()->addFilter(oadcf);
}

void KaduWindowActions::editUserActionCreated(Action *action)
{
	Buddy buddy = action->buddy();
	if (buddy.isAnonymous())
	{
		action->setIcon(IconsManager::instance()->iconByPath("contact-new"));
		action->setText(tr("Add Buddy"));
	}
}

void KaduWindowActions::showStatusActionCreated(Action *action)
{
	Account account = AccountManager::instance()->defaultAccount();

	if (account.protocolHandler())
		action->setIcon(account.protocolHandler()->statusIcon().pixmap(16,16));
}

void KaduWindowActions::useProxyActionCreated(Action *action)
{
	action->setChecked(config_file.readBoolEntry("Network", "UseProxy", false));
}

void KaduWindowActions::showInfoPanelActionCreated(Action *action)
{
	action->setChecked(config_file.readBoolEntry("Look", "ShowInfoPanel"));
}

void KaduWindowActions::showBlockedActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parent());
	if (!window)
		return;
	if (!window->buddiesListView())
		return;

	bool enabled = config_file.readBoolEntry("General", "ShowBlocked");
	BlockedBuddyFilter *ibf = new BlockedBuddyFilter(action);
	ibf->setEnabled(!enabled);

	action->setData(QVariant::fromValue(ibf));
	action->setChecked(enabled);

	window->buddiesListView()->addFilter(ibf);
}

void KaduWindowActions::configurationActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	MainConfigurationWindow::instance()->show();
}

void KaduWindowActions::yourAccountsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	YourAccounts::instance()->show();
}

void KaduWindowActions::exitKaduActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	kdebugf();

	// TODO: 0.6.6
	//if (measureTime)
	//{
	//	time_t sec;
	//	int msec;
	//	getTime(&sec, &msec);
	//	endingTime = (sec % 1000) * 1000 + msec;
	//}
	qApp->quit();
}

void KaduWindowActions::addUserActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	Buddy buddy = action->buddy();
	AddBuddyWindow *addBuddyWindow = new AddBuddyWindow(action->parentWidget());

	if (buddy.isAnonymous())
		addBuddyWindow->setBuddy(buddy);

	addBuddyWindow->show();

 	kdebugf2();
}

void KaduWindowActions::mergeContactActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	Buddy buddy = action->buddy();
	if (buddy)
	{
		MergeBuddiesWindow *mergeBuddiesWindow = new MergeBuddiesWindow(buddy);
		mergeBuddiesWindow->show();
	}

	kdebugf2();
}

void KaduWindowActions::addGroupActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	bool ok;
	QString newGroupName = QInputDialog::getText(sender->parentWidget(), tr("New Group"),
				tr("Please enter the name for the new group:"), QLineEdit::Normal,
				QString::null, &ok);

	if (ok && !newGroupName.isEmpty() && GroupManager::instance()->acceptableGroupName(newGroupName))
		GroupManager::instance()->byName(newGroupName);
}

void KaduWindowActions::openSearchActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	(new SearchWindow(sender->parentWidget()))->show();
}

void KaduWindowActions::helpActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (config_file.readEntry("General", "Language") == "pl")
		UrlOpener::openUrl("http://www.kadu.net/w/Pomoc_online");
	else
		UrlOpener::openUrl("http://www.kadu.net/w/English:Kadu:Help_online");
}

void KaduWindowActions::bugsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (config_file.readEntry("General", "Language") == "pl")
		UrlOpener::openUrl("http://www.kadu.net/w/Bledy");
	else
		UrlOpener::openUrl("http://www.kadu.net/w/English:Bugs");
}

void KaduWindowActions::supportActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (config_file.readEntry("General", "Language") == "pl")
		UrlOpener::openUrl("http://www.kadu.net/w/Kadu:Site_support");
	else
		UrlOpener::openUrl("http://www.kadu.net/w/English:Kadu:Site_support");
}

void KaduWindowActions::getInvolvedActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (config_file.readEntry("General", "Language") == "pl")
		UrlOpener::openUrl("http://www.kadu.net/w/Dolacz");
	else
		UrlOpener::openUrl("http://www.kadu.net/w/English:GetInvolved");
}

void KaduWindowActions::aboutActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	(new ::About(Core::instance()->kaduWindow()))->show();
}

void KaduWindowActions::translateActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	UrlOpener::openUrl("http://www.kadu.net/forum/viewforum.php?f=19");
}

void KaduWindowActions::showInfoPanelActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	Core::instance()->kaduWindow()->infoPanel()->setVisible(toggled);

	config_file.writeEntry("Look", "ShowInfoPanel", toggled);
}

void KaduWindowActions::showBlockedActionActivated(QAction *sender, bool toggled)
{
	QVariant v = sender->data();
	if (v.canConvert<BlockedBuddyFilter *>())
	{
		BlockedBuddyFilter *bbf = v.value<BlockedBuddyFilter *>();
		bbf->setEnabled(!toggled);
		config_file.writeEntry("General", "ShowBlocked", toggled);
	}
}

void KaduWindowActions::writeEmailActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	Buddy buddy = action->buddy();
	if (!buddy)
		return;

	if (!buddy.email().isEmpty())
		UrlOpener::openEmail(buddy.email());

	kdebugf2();
}

void KaduWindowActions::copyDescriptionActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	Contact data = action->contact();

	if (!data)
		return;

	QString description = data.currentStatus().description();
	if (description.isEmpty())
		return;

	QApplication::clipboard()->setText(description, QClipboard::Selection);
	QApplication::clipboard()->setText(description, QClipboard::Clipboard);

	kdebugf2();
}

void KaduWindowActions::openDescriptionLinkActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	Contact data = action->contact();

	if (!data)
		return;

	QString description = data.currentStatus().description();
	if (description.isEmpty())
		return;

	QRegExp url = UrlHandlerManager::instance()->urlRegExp();
	int idx_start = url.indexIn(description);
	if (idx_start >= 0)
		UrlOpener::openUrl(description.mid(idx_start, url.matchedLength()));

	kdebugf2();
}

void KaduWindowActions::copyPersonalInfoActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	ContactSet contacts = action->contacts();

	QStringList infoList;
	QString copyPersonalDataSyntax = config_file.readEntry("General", "CopyPersonalDataSyntax", tr("Contact: %a[ (%u)]\n[First name: %f\n][Last name: %r\n][Mobile: %m\n]"));
	foreach (Contact contact, contacts)
		infoList.append(Parser::parse(copyPersonalDataSyntax, contact.ownerBuddy(), contact, false));

	QString info = infoList.join("\n");
	if (info.isEmpty())
		return;

	QApplication::clipboard()->setText(info, QClipboard::Selection);
	QApplication::clipboard()->setText(info, QClipboard::Clipboard);

	kdebugf2();
}

void KaduWindowActions::lookupInDirectoryActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	Buddy buddy = action->buddy();
	if (!buddy)
		return;

	SearchWindow *sd = new SearchWindow(Core::instance()->kaduWindow(), buddy);
	sd->show();
	sd->firstSearch();

	kdebugf2();
}

void KaduWindowActions::hideDescriptionActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	BuddySet buddies = action->buddies();

	foreach (const Buddy &buddy, buddies)
	{
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		BuddyKaduData *bkd = 0;
		if (buddy.data())
			bkd = buddy.data()->moduleStorableData<BuddyKaduData>("kadu", true);
		if (!bkd)
			continue;

		if (bkd->hideDescription() != toggled)
		{
			bkd->setHideDescription(toggled);
			bkd->store();
		}
	}

	foreach (Action *action, HideDescription->actions())
		if (action->buddies() == buddies)
			action->setChecked(toggled);

	kdebugf2();
}

void KaduWindowActions::deleteUsersActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	deleteUserActionActivated(action->dataSource());
}

void KaduWindowActions::deleteUserActionActivated(ActionDataSource *source)
{
	kdebugf();

	BuddySet buddySet = source->buddies();
	if (buddySet.empty())
		return;

	BuddyDeleteWindow *deleteWindow = new BuddyDeleteWindow(buddySet);
	deleteWindow->show();
}

void KaduWindowActions::inactiveUsersActionActivated(QAction *sender, bool toggled)
{
	QVariant v = sender->data();
	if (v.canConvert<OfflineBuddyFilter *>())
	{
		OfflineBuddyFilter *ofcf = v.value<OfflineBuddyFilter *>();
		ofcf->setEnabled(!toggled);
		config_file.writeEntry("General", "ShowOffline", toggled);
	}
}

void KaduWindowActions::descriptionUsersActionActivated(QAction *sender, bool toggled)
{
	QVariant v = sender->data();
	if (v.canConvert<HasDescriptionBuddyFilter *>())
	{
		HasDescriptionBuddyFilter *hdcf = v.value<HasDescriptionBuddyFilter *>();
		hdcf->setEnabled(toggled);
	}
}

void KaduWindowActions::showDescriptionsActionActivated(QAction *sender, bool toggled)
{
	config_file.writeEntry("Look", "ShowDesc", toggled);

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	MainWindow *window = qobject_cast<MainWindow *>(action->parent());
	if (!window)
		return;
	if (!window->buddiesListView())
		return;

	window->buddiesListView()->delegateConfiguration().configurationUpdated();
}

void KaduWindowActions::onlineAndDescUsersActionActivated(QAction *sender, bool toggled)
{
	config_file.writeEntry("General", "ShowOnlineAndDescription", toggled);

	QVariant v = sender->data();
	if (v.canConvert<OnlineAndDescriptionBuddyFilter *>())
	{
		OnlineAndDescriptionBuddyFilter *oadcf = v.value<OnlineAndDescriptionBuddyFilter *>();
		oadcf->setEnabled(toggled);
	}
}

void KaduWindowActions::editUserActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	editUserActionActivated(action->dataSource());
}

void KaduWindowActions::editUserActionActivated(ActionDataSource *source)
{
	kdebugf();

	BuddySet buddySet = source->buddies();
	if (1 != buddySet.count())
		return;

	Buddy buddy = *buddySet.begin();

	if (buddy.isAnonymous())
	{
		AddBuddyWindow *addBuddyWindow = new AddBuddyWindow(Core::instance()->kaduWindow());
		addBuddyWindow->setBuddy(buddy);
		addBuddyWindow->show();
	}
	else
		(new BuddyDataWindow(buddy, Core::instance()->kaduWindow()))->show();

	kdebugf2();
}

void KaduWindowActions::showStatusActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	StatusContainer *container = window->statusContainer();
	if (!container)
		container = StatusContainerManager::instance();

	QMenu *menu = new QMenu();
	new StatusMenu(container, menu);
	menu->exec(QCursor::pos());
	delete menu;
}

void KaduWindowActions::useProxyActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)

	config_file.writeEntry("Network", "UseProxy", toggled);

	foreach (Action *action, UseProxy->actions())
		action->setChecked(toggled);
}

void KaduWindowActions::configurationUpdated()
{
	if (ShowInfoPanel->action(Core::instance()->kaduWindow())->isChecked() != config_file.readBoolEntry("Look", "ShowInfoPanel"))
		ShowInfoPanel->action(Core::instance()->kaduWindow())->trigger();

	if (InactiveUsers->action(Core::instance()->kaduWindow())->isChecked() != config_file.readBoolEntry("General", "ShowOffline"))
		InactiveUsers->action(Core::instance()->kaduWindow())->trigger();

	if (ShowBlockedBuddies->action(Core::instance()->kaduWindow())->isChecked() != config_file.readBoolEntry("General", "ShowBlocked"))
		ShowBlockedBuddies->action(Core::instance()->kaduWindow())->trigger();

}

// void Kadu::setProxyActionsStatus() TODO: 0.6.6
// {
// 	setProxyActionsStatus(config_file.readBoolEntry("Network", "UseProxy", false));
// }
