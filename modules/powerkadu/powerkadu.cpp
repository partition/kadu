/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kadu.h"
#include "misc.h"
#include "modules.h"
#include "debug.h"
#include "icons_manager.h"
#include "config_file.h"

#include "powerkadu.h"
#include "about_dialog.h"

#include "exports.h"

PowerKadu* powerKadu;

extern "C" KADU_EXPORT int powerkadu_init()
{
	kdebugf();

	powerKadu = new PowerKadu();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/powerkadu.ui"), powerKadu);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void powerkadu_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/powerkadu.ui"), powerKadu);
	delete powerKadu;
	powerKadu = 0;

	kdebugf2();
}

//-----------------------------------------------------------------------------------

PowerKadu::PowerKadu()
 : QObject()
{
	kdebugf();

	powerKaduMenuActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "powerKaduMenuAction",
		this, SLOT(onAboutPowerKadu()),
		dataPath("kadu/modules/data/powerkadu/powerkadu_32x32.png"), tr("&About PowerKadu...")
	);
	kadu->insertMenuActionDescription(0, powerKaduMenuActionDescription);

	aboutDialog = 0;

	kdebugf2();
}

PowerKadu::~PowerKadu()
{
	kdebugf();

	kadu->removeMenuActionDescription(powerKaduMenuActionDescription);
	delete powerKaduMenuActionDescription;

	if (aboutDialog != 0) {
		delete aboutDialog;
		aboutDialog = 0;
	}

	if (!Kadu::closing() && config_file.readBoolEntry("PowerKadu", "unload_modules", true)) {
		unloadModules();
	}

	kdebugf2();
}

void PowerKadu::onAboutPowerKadu()
{
	kdebugf();

	if (aboutDialog == 0) {
		aboutDialog = new AboutDialog("about_dialog", true);
		aboutDialog->exec();
	} else
		aboutDialog->show();

	kdebugf2();
}

void PowerKadu::unloadModules()
{
	kdebugf();
	ModuleInfo info;
	QString module;
	if (modules_manager->moduleInfo("powerkadu", info))
	{
		foreach (module, info.depends)
		{
			if (modules_manager->moduleIsActive(module))
				modules_manager->deactivateModule(module, false);
		}
	}
	kdebugf2();
}

void PowerKadu::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
}

void PowerKadu::configurationUpdated()
{
}