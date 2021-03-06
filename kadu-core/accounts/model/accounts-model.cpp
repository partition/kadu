/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include "accounts/account-manager.h"
#include "model/roles.h"
#include "protocols/protocol.h"

#include "accounts-model.h"

AccountsModel::AccountsModel(QObject *parent) :
		QAbstractListModel(parent), IncludeIdInDisplay(false)
{
	connect(AccountManager::instance(), SIGNAL(accountUpdated(Account)),
			this, SLOT(accountUpdated(Account)));
	connect(AccountManager::instance(), SIGNAL(accountAboutToBeRegistered(Account)),
			this, SLOT(accountAboutToBeRegistered(Account)));
	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)),
			this, SLOT(accountRegistered(Account)));
	connect(AccountManager::instance(), SIGNAL(accountAboutToBeUnregistered(Account)),
			this, SLOT(accountAboutToBeUnregistered(Account)));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)),
			this, SLOT(accountUnregistered(Account)));
}

AccountsModel::~AccountsModel()
{
	disconnect(AccountManager::instance(), SIGNAL(accountUpdated(Account)),
			this, SLOT(accountUpdated(Account)));
	disconnect(AccountManager::instance(), SIGNAL(accountAboutToBeRegistered(Account)),
			this, SLOT(accountAboutToBeRegistered(Account)));
	disconnect(AccountManager::instance(), SIGNAL(accountRegistered(Account)),
			this, SLOT(accountRegistered(Account)));
	disconnect(AccountManager::instance(), SIGNAL(accountAboutToBeUnregistered(Account)),
			this, SLOT(accountAboutToBeUnregistered(Account)));
	disconnect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)),
			this, SLOT(accountUnregistered(Account)));
}

int AccountsModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : 1;
}

int AccountsModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : AccountManager::instance()->count();
}

QVariant AccountsModel::data(const QModelIndex &index, int role) const
{
	Account acc = account(index);
	if (acc.isNull())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			if (IncludeIdInDisplay)
				return QString("%1 (%2)").arg(acc.accountIdentity().name(), acc.id());
			else
				return acc.accountIdentity().name();
		case Qt::DecorationRole:
			return acc.protocolHandler()
					? acc.protocolHandler()->icon()
					: QVariant();

		case AccountRole:
			return QVariant::fromValue<Account>(acc);

		case ItemTypeRole:
			return AccountRole;

		default:
			return QVariant();
	}
}

QVariant AccountsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

Account AccountsModel::account(const QModelIndex &index) const
{
	if (!index.isValid())
		return Account::null;

	if (index.row() < 0 || index.row() >= rowCount())
		return Account::null;

	return AccountManager::instance()->byIndex(index.row());
}

int AccountsModel::accountIndex(Account account) const
{
	return AccountManager::instance()->indexOf(account);
}

QModelIndex AccountsModel::indexForValue(const QVariant &value) const
{
	return createIndex(accountIndex(value.value<Account>()), 0, 0);
}

void AccountsModel::accountUpdated(Account account)
{
	QModelIndex index = indexForValue(account);
	emit dataChanged(index, index);
}

void AccountsModel::accountAboutToBeRegistered(Account account)
{
	Q_UNUSED(account)

	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void AccountsModel::accountRegistered(Account account)
{
	Q_UNUSED(account)

	endInsertRows();
}

void AccountsModel::accountAboutToBeUnregistered(Account account)
{
	int index = accountIndex(account);
	beginRemoveRows(QModelIndex(), index, index);
}

void AccountsModel::accountUnregistered(Account account)
{
	Q_UNUSED(account)

	endRemoveRows();
}

void AccountsModel::setIncludeIdInDisplay(bool includeIdInDisplay)
{
	if (IncludeIdInDisplay == includeIdInDisplay)
		return;

	IncludeIdInDisplay = includeIdInDisplay;
	emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}
