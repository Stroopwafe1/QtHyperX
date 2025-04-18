// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QQmlEngine>
#include <AbstractKirigamiApplication>

using namespace Qt::StringLiterals;

class QtHyperXApplication : public AbstractKirigamiApplication
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit QtHyperXApplication(QObject *parent = nullptr);

Q_SIGNALS:
    void sendBatteryStatus();

private:
    void setupActions() override;
};
