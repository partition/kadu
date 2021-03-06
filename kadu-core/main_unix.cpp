/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
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

#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "configuration/xml-configuration-file.h"
#include "core/core.h"
#include "core/crash-aware-object.h"
#include "gui/windows/kadu-window.h"

#include "debug.h"
#include "kadu-config.h"
#include "misc/path-conversion.h"
#include "modules.h"

#ifdef SIG_HANDLING_ENABLED
#include <signal.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>

#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif // HAVE_EXECINFO

static void kadu_signal_handler(int signal)
{
	kdebugmf(KDEBUG_WARNING, "%d\n", signal);

	static int sigsegvCount = 0;

	if (sigsegvCount > 1)
	{
		kdebugmf(KDEBUG_WARNING, "sigsegv recursion: %d\n", sigsegvCount);
		abort();
	}

	if (signal == SIGSEGV)
	{
		++sigsegvCount;
		kdebugm(KDEBUG_PANIC, "Kadu crashed :(\n");

		CrashAwareObject::notifyCrash();

		QString backupFileName = QString("kadu.conf.xml.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
		QString backtraceFileName = QString("kadu.backtrace.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));

#ifdef HAVE_EXECINFO
		void *backtraceArray[100];
		char **backtraceStrings;
		int numEntries;

		if ((numEntries = backtrace(backtraceArray, 100)) < 0)
		{
			kdebugm(KDEBUG_PANIC, "could not generate backtrace\n");
			abort();
		}
		if (!(backtraceStrings = backtrace_symbols(backtraceArray, numEntries)))
		{
			kdebugm(KDEBUG_PANIC, "could not get symbol names for backtrace\n");
			abort();
		}

		fprintf(stderr, "\n======= BEGIN OF BACKTRACE =====\n");
		for (int i = 0; i < numEntries; ++i)
			fprintf(stderr, "[%d] %s\n", i, backtraceStrings[i]);
		fprintf(stderr, "======= END OF BACKTRACE  ======\n");
		fflush(stderr);

		FILE *backtraceFile = fopen(qPrintable(profilePath(backtraceFileName)), "w");
		if (backtraceFile)
		{
			fprintf(backtraceFile, "======= BEGIN OF BACKTRACE =====\n");
			for (int i = 0; i < numEntries; ++i)
				fprintf(backtraceFile, "[%d] %s\n", i, backtraceStrings[i]);
			fprintf(backtraceFile, "======= END OF BACKTRACE  ======\n");

			fprintf(backtraceFile, "static modules:\n");
			QStringList modules = ModulesManager::instance()->staticModules();

			foreach (const QString &module, modules)
				fprintf(backtraceFile, "> %s\n", qPrintable(module));

			fprintf(backtraceFile, "loaded modules:\n");
			modules = ModulesManager::instance()->loadedModules();
			foreach (const QString &module, modules)
				fprintf(backtraceFile, "> %s\n", qPrintable(module));
			fprintf(backtraceFile, "Kadu version: %s\n", qPrintable(Core::version()));
			fprintf(backtraceFile, "Qt compile time version: %s\nQt runtime version: %s\n",
					QT_VERSION_STR, qVersion());
#ifdef __GNUC__
			fprintf(backtraceFile, "GCC version: %d.%d.%d\n",
					__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif // __GNUC__
			fprintf(backtraceFile, "EOF\n");

			fclose(backtraceFile);
		}

		free(backtraceStrings);
#else // HAVE_EXECINFO
		kdebugm(KDEBUG_PANIC, "backtrace not available\n");
#endif // HAVE_EXECINFO

		xml_config_file->saveTo(profilePath(backupFileName.toLatin1()));
		abort();
	}
	else if (signal == SIGUSR1)
	{
		kdebugm(KDEBUG_INFO, "ok, got a signal to show up\n");
		Core::instance()->kaduWindow()->show();
	}
	else if (signal == SIGINT || signal == SIGTERM)
		qApp->quit();
}

#endif // SIG_HANDLING_ENABLED

void enableSignalHandling()
{
#ifdef SIG_HANDLING_ENABLED
	char *d = getenv("SIGNAL_HANDLING");
	bool signalHandlingEnabled = d ? (atoi(d) != 0) : true;

	if (signalHandlingEnabled)
	{
		signal(SIGSEGV, kadu_signal_handler);
		signal(SIGINT, kadu_signal_handler);
		signal(SIGTERM, kadu_signal_handler);
		signal(SIGUSR1, kadu_signal_handler);
		signal(SIGPIPE, SIG_IGN);
	}
#endif // SIG_HANDLING_ENABLED
}
