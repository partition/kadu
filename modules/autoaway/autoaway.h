#ifndef AUTOAWAY_H
#define AUTOAWAY_H

#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtCore/QObject>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"
#include "status_changer.h"
#include "gadu.h"

class QLineEdit;
class QSpinBox;
class QCheckBox;

/**
 * @defgroup autoaway Autoaway
 * @{
 */
class AutoAwayStatusChanger : public StatusChanger
{
	Q_OBJECT

	friend class AutoAway;

public:
	enum ChangeStatusTo {
		NoChangeStatus,
		ChangeStatusToBusy,
		ChangeStatusToInvisible,
		ChangeStatusToOffline,
		ChangeStatusToTalkWithMe,
		ChangeStatusToDoNotDisturb
	};

	enum ChangeDescriptionTo {
		NoChangeDescription,
		ChangeDescriptionReplace,
		ChangeDescriptionPrepend,
		ChangeDescriptionAppend
	};

private:
	ChangeStatusTo changeStatusTo;
	ChangeDescriptionTo changeDescriptionTo;
	QString descriptionAddon;

public:
	AutoAwayStatusChanger();
	virtual ~AutoAwayStatusChanger();

	virtual void changeStatus(UserStatus &status);

	void setChangeStatusTo(ChangeStatusTo newChangeStatusTo);
	void setChangeDescriptionTo(ChangeDescriptionTo newChangeDescriptionTo, const QString &newDescriptionAddon);

};

class AutoAway : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	AutoAwayStatusChanger *autoAwayStatusChanger;
	QTimer *timer;

	unsigned int checkInterval;

	unsigned int autoAwayTime;
	unsigned int autoDisconnectTime;
	unsigned int autoInvisibleTime;
	unsigned int autoDNDTime;

	bool autoAwayEnabled;
	bool autoInvisibleEnabled;
	bool autoDisconnectEnabled;
	bool autoDNDEnabled;
	bool parseAutoStatus;

	bool updateDescripion;

	unsigned int idleTime;
	unsigned int refreshStatusTime;
	unsigned int refreshStatusInterval;

	QSpinBox *autoAwaySpinBox;
	QSpinBox *autoInvisibleSpinBox;
	QSpinBox *autoOfflineSpinBox;
	QSpinBox *autoDNDSpinBox;
	QSpinBox *autoRefreshSpinBox;

	QLineEdit *descriptionTextLineEdit;

	QCheckBox *parseStatusCheckBox;

	QString autoStatusText;

	AutoAwayStatusChanger::ChangeDescriptionTo changeTo;

	QString parseDescription(const QString &parseDescription);

	void createDefaultConfiguration();

private slots:
	void checkIdleTime();

	void autoAwaySpinBoxValueChanged(int value);
	void autoInvisibleSpinBoxValueChanged(int value);
	void autoOfflineSpinBoxValueChanged(int value);
	void autoDNDSpinBoxValueChanged(int value);

	void descriptionChangeChanged(int index);

	void on();
	void off();

protected:
	bool eventFilter(QObject *, QEvent *);
	virtual void configurationUpdated();

public:
	AutoAway();
	virtual ~AutoAway();

	QString changeDescription(const QString &oldDescription);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

/** @} */

#endif
