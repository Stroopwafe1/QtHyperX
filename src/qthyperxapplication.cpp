// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>

#include "qthyperxapplication.h"
#include <KAuthorized>
#include <KLocalizedString>
#include <iostream>

using namespace Qt::StringLiterals;

QtHyperXApplication::QtHyperXApplication(QObject *parent)
    : AbstractKirigamiApplication(parent)
{
    setupActions();
}

void QtHyperXApplication::setupActions()
{
    AbstractKirigamiApplication::setupActions();

    auto actionName = "send_battery_status"_L1;
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = mainCollection()->addAction(actionName, this, &QtHyperXApplication::sendBatteryStatus);
        action->setText(i18nc("@action:inmenu", "Get Battery"));
        action->setIcon(QIcon::fromTheme(u"list-add-symbolic"_s));
        mainCollection()->addAction(action->objectName(), action);
        mainCollection()->setDefaultShortcut(action, Qt::CTRL | Qt::Key_I);
    }

    readSettings();
}

#include "moc_qthyperxapplication.cpp"
