/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_FILE_TRANSFER_SERVICE_H
#define JABBER_FILE_TRANSFER_SERVICE_H

#include "protocols/services/file-transfer-service.h"
#include "file-transfer/jabber-file-transfer-handler.h"

class JabberProtocol;

class JabberFileTransferService : public FileTransferService
{
	Q_OBJECT

	JabberProtocol *Protocol;

public:
	JabberFileTransferService(JabberProtocol *protocol);

	virtual FileTransferHandler * createFileTransferHandler(FileTransfer fileTransfer);

public slots:
	void incomingFile(FileTransfer transfer);

};

#endif // JABBER_FILE_TRANSFER_SERVICE_H
