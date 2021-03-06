/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>

#include "core/core.h"
#include "configuration/configuration-file.h"
#include "gui/hot-key.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/status-buttons.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-window.h"
#include "misc/path-conversion.h"

#include "modules/docking/docking.h"
#include "simpleview-config-ui.h"
#include "simpleview.h"

SimpleView *SimpleView::Instance = 0;

SimpleView::SimpleView() :
	SimpleViewActive(false)
{
	BuddiesListView *buddiesListViewHandle;

	SimpleViewConfigUi::createInstance();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/simpleview.ui"));
	MainConfigurationWindow::registerUiHandler(SimpleViewConfigUi::instance());

	DockAction = new QAction(IconsManager::instance()->iconByPath("view-refresh"), tr("Simple view"), this);
	DockAction->setCheckable(true);
	connect(DockAction, SIGNAL(triggered(bool)), this, SLOT(simpleViewToggle(bool)));
	DockingManager::instance()->registerModuleAction(DockAction);

	KaduWindowHandle = Core::instance()->kaduWindow();
	MainWindowHandle = KaduWindowHandle->findMainWindow(KaduWindowHandle);
	buddiesListViewHandle = MainWindowHandle->buddiesListView();
	BuddiesListWidgetHandle = qobject_cast<BuddiesListWidget *>(buddiesListViewHandle->parentWidget());
	GroupTabBarHandle = KaduWindowHandle->findChild<GroupTabBar *>();
	StatusButtonsHandle = KaduWindowHandle->findChild<StatusButtons *>();

	configurationUpdated();

	DiffRect = config_file.readRectEntry("Look", "SimpleViewGeometry");
	if (!DiffRect.isNull())
		simpleViewToggle(true);
}

SimpleView::~SimpleView()
{
	config_file.writeEntry("Look", "SimpleViewGeometry", DiffRect);

	simpleViewToggle(false);

	if (!Core::instance()->isClosing())
		DockingManager::instance()->unregisterModuleAction(DockAction);

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/simpleview.ui"));
	MainConfigurationWindow::unregisterUiHandler(SimpleViewConfigUi::instance());
	SimpleViewConfigUi::destroyInstance();
}

void SimpleView::createInstance()
{
	if (!Instance)
		Instance = new SimpleView();
}

void SimpleView::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

void SimpleView::simpleViewToggle(bool activate)
{
	/* This is very 'el hacha',
	 * but this way we do not change the main code.
	 *
	 * The parent-child tree is:
	 * MainWindow/KaduWindow
	 * +-QWidget (MainWidget)
	 *   =QVBoxLayout (MainLayout)
	 *   +-QSplitter (Split)
	 *     +-QWidget (hbox)
	 *     |=QHBoxLayout (hboxLayout)
	 *     |+-GroupTabBar (GroupBar)
	 *     |+-BuddiesListWidget (ContactsWidget)
	 *     |  =QVBoxLayout (layout)
	 *     |  +-BuddiesListView (ContactsWidget->view())
	 *     |  +-FilterWidget (nameFilterWidget())
	 *     +-BuddyInfoPanel (InfoPanel)
	 */
	if (activate != SimpleViewActive)
	{
		Qt::WindowFlags flags;
		QRect mr, r;

		SimpleViewActive = activate;

		flags = MainWindowHandle->windowFlags();
		mr = MainWindowHandle->geometry();

		if (SimpleViewActive)
		{
			if (DiffRect.isNull())
			{
				if (KeepSize)
				{
					r.setTopLeft(BuddiesListWidgetHandle->view()->mapToGlobal(BuddiesListWidgetHandle->view()->rect().topLeft()));
					r.setSize(BuddiesListWidgetHandle->view()->rect().size());
				}
				else
					r = MainWindowHandle->frameGeometry();

				DiffRect.setRect(mr.x() - r.x(), mr.y() - r.y(), mr.width() - r.width(), mr.height() - r.height());
			}
			else
				r.setRect(mr.x() - DiffRect.x(), mr.y() - DiffRect.y(), mr.width() - DiffRect.width(), mr.height() - DiffRect.height());

			if (Borderless)
				BuddiesListViewStyle = BuddiesListWidgetHandle->view()->styleSheet();

			MainWindowHandle->hide();

			/* Toolbars */
			foreach (QObject *object, MainWindowHandle->children())
			{
				QToolBar *toolBar = qobject_cast<QToolBar *>(object);
				if (toolBar)
					toolBar->setVisible(false);
			}

			/* Menu bar */
			KaduWindowHandle->menuBar()->hide();

			/* GroupBar */
			GroupTabBarHandle->setVisible(false);

			/* Filter */
			/* Note: filter hides/shows now automatically.
			 * BuddiesListWidgetHandle->nameFilterWidget()->hide();
			 */

			/* ScrollBar */
			if (NoScrollBar)
				BuddiesListWidgetHandle->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

			/* Info panel*/
			KaduWindowHandle->infoPanel()->hide();

			/* Status button */
			StatusButtonsHandle->hide();

			MainWindowHandle->setWindowFlags(flags | Qt::FramelessWindowHint);

			MainWindowHandle->setGeometry(r);

			if (Borderless)
				BuddiesListWidgetHandle->view()->setStyleSheet(QString("QTreeView { border-style: none; }") + BuddiesListViewStyle);
		}
		else
		{
			MainWindowHandle->hide();

			if (Borderless)
				BuddiesListWidgetHandle->view()->setStyleSheet(BuddiesListViewStyle);

			r.setRect(mr.x() + DiffRect.x(), mr.y() + DiffRect.y(), mr.width() + DiffRect.width(), mr.height() + DiffRect.height());

			MainWindowHandle->setWindowFlags(flags & ~(Qt::FramelessWindowHint));

			MainWindowHandle->setGeometry(r);

			/* Status button */
			StatusButtonsHandle->setVisible(config_file.readBoolEntry("Look", "ShowStatusButton"));

			/* Info panel*/
			if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
				KaduWindowHandle->infoPanel()->show();

			/* ScrollBar */
			BuddiesListWidgetHandle->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

			/* Filter */
			/* Note: filter hides/shows now automatically.
			 * BuddiesListWidgetHandle->nameFilterWidget()->show();
			 */

			/* GroupBar */
			if (config_file.readBoolEntry("Look", "DisplayGroupTabs"))
				GroupTabBarHandle->setVisible(true);

			/* Menu bar */
			KaduWindowHandle->menuBar()->show();

			/* Toolbars */
			foreach (QObject *object, MainWindowHandle->children())
			{
				QToolBar *toolBar = qobject_cast<QToolBar *>(object);
				if (toolBar)
					toolBar->setVisible(true);
			}

			DiffRect = QRect(0,0,0,0);
		}
		MainWindowHandle->show();

		if (!Core::instance()->isClosing())
			DockAction->setChecked(SimpleViewActive);
	}
}

void SimpleView::compositingEnabled()
{
	/* Give the kadu update the GUI */
	simpleViewToggle(false);
}
void SimpleView::compositingDisabled()
{
	/* Give the kadu update the GUI */
	simpleViewToggle(false);
}

void SimpleView::configurationUpdated()
{
	/* Give the kadu update the GUI with old configuration */
	simpleViewToggle(false);

	KeepSize = config_file.readBoolEntry("Look", "SimpleViewKeepSize", true);
	NoScrollBar = config_file.readBoolEntry("Look", "SimpleViewNoScrollBar", true);
	Borderless = config_file.readBoolEntry("Look", "SimpleViewBorderless", true);

}
