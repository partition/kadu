/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status/status.h"

#include "autoaway-status-changer.h"

AutoAwayStatusChanger::AutoAwayStatusChanger() :
		StatusChanger(900), changeStatusTo(NoChangeStatus), changeDescriptionTo(NoChangeDescription)
{
}

AutoAwayStatusChanger::~AutoAwayStatusChanger()
{
}
#include <stdio.h>
void AutoAwayStatusChanger::changeStatus(StatusContainer *container, Status &status)
{
	if (changeStatusTo == NoChangeStatus)
		return;

	if (status.isDisconnected())
		return;

	QString description = status.description();
	switch (changeDescriptionTo)
	{
		case NoChangeDescription:
			break;

		case ChangeDescriptionPrepend:
			description = descriptionAddon + description;
			break;

		case ChangeDescriptionReplace:
			description = descriptionAddon;
			break;

		case ChangeDescriptionAppend:
			description = description + descriptionAddon;
			break;
	}

	if (changeStatusTo == ChangeStatusToOffline)
	{
		status.setType("Offline");
		status.setDescription(description);
		return;
	}

	if (status.group() == "Invisible")
		return;

	if (changeStatusTo == ChangeStatusToInvisible)
	{
		status.setType("Invisible");
		status.setDescription(description);
		return;
	}

	if (status.group() == "Away")
		return;

	if (changeStatusTo == ChangeStatusToBusy)
	{
		status.setType("Away");
		status.setDescription(description);
		return;
	}
}

void AutoAwayStatusChanger::setChangeStatusTo(ChangeStatusTo newChangeStatusTo)
{
	changeStatusTo = newChangeStatusTo;
	emit statusChanged(0); // for all status containers
}

void AutoAwayStatusChanger::setChangeDescriptionTo(ChangeDescriptionTo newChangeDescriptionTo, const QString &newDescriptionAddon)
{
	changeDescriptionTo = newChangeDescriptionTo;
	descriptionAddon = newDescriptionAddon;
}
