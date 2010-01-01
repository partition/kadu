/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_ACCOUNT_DETAILS
#define TLEN_ACCOUNT_DETAILS

#include <QtCore/QString>

#include "accounts/account-details.h"
#include "open-chat-with/tlen-open-chat-with-runner.h"
#include "protocols/protocol.h"

class TlenAccountDetails : public AccountDetails
{
	TlenOpenChatWithRunner *OpenChatRunner;

protected:
	virtual void load();

public:
	explicit TlenAccountDetails(AccountShared *data);
	virtual ~TlenAccountDetails();

	virtual void store();

};

#endif // TLEN_ACCOUNT_DETAILS