/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "contacts/contact.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "protocols/protocol.h"

#include "services/jabber-subscription-service.h"
#include "jabber-protocol.h"

#include "jabber-actions.h"

void disableNoRosterContact(Action *action)
{
	action->setEnabled(false);

	Contact contact = action->contact();
	if (!contact)
		return;

	Account account = contact.contactAccount();
	if (!account || !account.protocolHandler() || !account.protocolHandler()->rosterService())
		return;

	if (!account.protocolHandler()->isConnected())
		return;

	action->setEnabled(true);
}

JabberActions * JabberActions::Instance = 0;

void JabberActions::registerActions()
{
	if (!Instance)
		Instance = new JabberActions();
}

void JabberActions::unregisterActions()
{
	delete Instance;
	Instance = 0;
}

JabberActions::JabberActions()
{
	ResendSubscription = new ActionDescription(this, ActionDescription::TypeUser, "rosterResendSubscription",
			this, SLOT(resendSubscriptionActionActivated(QAction*,bool)), "", tr("Resend Subscription"), false,
			disableNoRosterContact);
	RemoveSubscription = new ActionDescription(this, ActionDescription::TypeUser, "rosterRemoveSubscription",
			this, SLOT(removeSubscriptionActionActivated(QAction*,bool)), "", tr("Remove Subscription"), false,
			disableNoRosterContact);
	AskForSubscription = new ActionDescription(this, ActionDescription::TypeUser, "rosterAskForSubscription",
			this, SLOT(askForSubscriptionActionActivated(QAction*,bool)), "", tr("Ask for Subscription"), false,
			disableNoRosterContact);
}

JabberActions::~JabberActions()
{
}

Contact JabberActions::contactFromAction(QAction *action)
{
	Action *kaduAction = qobject_cast<Action *>(action);
	if (!kaduAction)
		return Contact::null;

	return kaduAction->contact();
}

JabberSubscriptionService * JabberActions::subscriptionServiceFromContact(const Contact& contact)
{
	Protocol *protocolHandler = contact.contactAccount().protocolHandler();
	if (!protocolHandler)
		return 0;

	JabberProtocol *jabberProtocolHandler = qobject_cast<JabberProtocol *>(protocolHandler);
	if (!jabberProtocolHandler)
		return 0;

	return jabberProtocolHandler->subscriptionService();
}

void JabberActions::resendSubscriptionActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	Contact contact = contactFromAction(sender);
	if (!contact)
		return;

	JabberSubscriptionService *subscriptionService = subscriptionServiceFromContact(contact);
	if (!subscriptionService)
		return;

	subscriptionService->resendSubscription(contact);
}

void JabberActions::removeSubscriptionActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	Contact contact = contactFromAction(sender);
	if (!contact)
		return;

	JabberSubscriptionService *subscriptionService = subscriptionServiceFromContact(contact);
	if (!subscriptionService)
		return;

	subscriptionService->removeSubscription(contact);
}

void JabberActions::askForSubscriptionActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	Contact contact = contactFromAction(sender);
	if (!contact)
		return;

	JabberSubscriptionService *subscriptionService = subscriptionServiceFromContact(contact);
	if (!subscriptionService)
		return;

	subscriptionService->requestSubscription(contact);
}