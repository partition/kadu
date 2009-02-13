#ifndef KADU_PROTOCOL_H
#define KADU_PROTOCOL_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtGui/QIcon>

#ifdef __sun__
#include <sys/types.h>
#endif /* __sun__ */

#ifdef _MSC_VER
#include "kinttypes.h"
#endif

#include "exports.h"

#include "contacts/contact.h"
#include "contacts/contact-list.h"

#include "status.h"

typedef uint32_t UinType;

class QPixmap;

class AccountData;
class ChatImageService;
class ChatService;
class ContactListService;
class Message;
class ProtocolFactory;
class Status;

class KADUAPI Protocol : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Protocol)

public:
	enum NetworkState {
		NetworkDisconnected,
		NetworkConnecting,
		NetworkConnected,
		NetworkDisconnecting
	};

private:
	ProtocolFactory *Factory;

	Account *CurrentAccount;

	NetworkState State;
	Status CurrentStatus;
	Status NextStatus;

protected:
	void setAllOffline();

	virtual void changeStatus(Status status) = 0;
	void statusChanged(Status status);

	void networkStateChanged(NetworkState state);

public:
	Protocol(Account *account, ProtocolFactory *factory);
	virtual ~Protocol();

	ProtocolFactory * protocolFactory() const { return Factory; }
	Account * account() const { return CurrentAccount; }

	virtual ChatImageService * chatImageService() { return 0; }
	virtual ChatService * chatService() { return 0; }
	virtual ContactListService * contactListService() { return 0; }

	virtual void setAccount(Account *account); 
	virtual bool validateUserID(QString &uid) = 0;

	NetworkState state() { return State; }
	bool isConnected() { return (State == NetworkConnected); }

	void setStatus(Status status);
	const Status & status() const { return CurrentStatus; }
	const Status & nextStatus() const { return NextStatus; }

	virtual QPixmap statusPixmap(Status status) = 0;
	QPixmap statusPixmap() { return statusPixmap(CurrentStatus); }

	QIcon icon();

signals:
	void connecting(Account *account);
	void connected(Account *account);
	void disconnecting(Account *account);
	void disconnected(Account *account);

	void statusChanged(Account *account, Status newStatus);
	void contactStatusChanged(Account *account, Contact contact, Status oldStatus);


// TODO: REVIEW
	void connectionError(Account *account, const QString &server, const QString &reason);

};

#endif
