/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_DCC_H
#define KADU_DCC_H

#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtNetwork/QHostAddress>

#include "configuration_aware_object.h"
#include "dcc_handler.h"
#include "gadu.h"
#include "main_configuration_window.h"

#include "dcc_exports.h"

class QCheckBox;
class QSocketNotifier;
class QWidget;

/**
 * @defgroup dcc Dcc
 * @{
 */

class DccSocket;

enum DccVersion {
	DccUnknow,
	Dcc6,
	Dcc7
};

class DCCAPI DccManager : public ConfigurationUiHandler, ConfigurationAwareObject, DccHandler
{
	Q_OBJECT

	friend class DccSocket;

	DccSocket *MainSocket;

	QList<DccSocket *> UnhandledSockets;
	QList<DccHandler *> SocketHandlers;

	QTimer TimeoutTimer;
	QMap<UinType, DccHandler *> requests;
	bool DccEnabled;

	QWidget *ipAddress;
	QCheckBox *forwarding;
	QWidget *forwardingExternalIp;
	QWidget *forwardingExternalPort;
	QWidget *forwardingLocalPort;

	void createDefaultConfiguration();

private slots:
	void startTimeout();
	void cancelTimeout();
	void setupDcc();
	void closeDcc();

	void dcc7New(struct gg_dcc7 *);

	/**
		Otrzymano wiadomo�� CTCP.
		Kto� nas prosi o po��czenie dcc, poniewa�
		jeste�my za NAT-em.
	**/
	void dccConnectionReceived(const UserListElement& sender);
	void timeout();
	void callbackReceived(DccSocket *socket);

	void onIpAutotetectToggled(bool toggled);

protected:
	virtual void configurationUpdated();

public:
	DccManager();
	virtual ~DccManager();

	bool addSocket(DccSocket *socket);
	void removeSocket(DccSocket *socket);

	void addHandler(DccHandler *handler);
	void removeHandler(DccHandler *handler);

	int dccType() { return 0; }

	bool socketEvent(DccSocket *socket, bool &lock);

	void connectionDone(DccSocket *socket) {}
	void connectionError(DccSocket *socket);

	void connectionAccepted(DccSocket *socket) {}
	void connectionRejected(DccSocket *socket) {}

	void getFileTransferSocket(uint32_t ip, uint16_t port, UinType myUin, UinType peerUin, DccHandler *handler, bool request = false);
	void getVoiceSocket(uint32_t ip, uint16_t port, UinType myUin, UinType peerUin, DccHandler *handler, bool request = false);

	bool dccEnabled() const;

	bool acceptClient(UinType uin, UinType peerUin, int remoteAddr);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

signals:
	void socketDestroying(DccSocket* socket);
	void dcc7IncomingFileTransfer(DccSocket *socket);

};

extern DCCAPI DccManager* dcc_manager;

/** @} */

#endif