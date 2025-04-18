/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>
*/

#include <QtGlobal>
#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QUrl>

#include "version-qthyperx.h"
#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "qthyperxconfig.h"
#include "HyperXBackend.hpp"

using namespace Qt::Literals::StringLiterals;

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
#else
    QApplication app(argc, argv);

    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(u"org.kde.desktop"_s);
    }
#endif

#ifdef Q_OS_WINDOWS
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }

    QApplication::setStyle(QStringLiteral("breeze"));
    auto font = app.font();
    font.setPointSize(10);
    app.setFont(font);
#endif

    KLocalizedString::setApplicationDomain("qthyperx");
    QCoreApplication::setOrganizationName(u"KDE"_s);

    KAboutData aboutData(
        // The program name used internally.
        u"qthyperx"_s,
        // A displayable program name string.
        i18nc("@title", "QtHyperX"),
        // The program version string.
        QStringLiteral(QTHYPERX_VERSION_STRING),
        // Short description of what the app does.
        i18n("Application Description"),
        // The license this code is released under.
        KAboutLicense::GPL,
        // Copyright Statement.
        i18n("(c) %{CURRENT_YEAR}"));
    aboutData.addAuthor(i18nc("@info:credit", "%{AUTHOR}"),
                        i18nc("@info:credit", "Maintainer"),
                        u"%{EMAIL}"_s,
                        u"https://yourwebsite.com"_s);
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    KAboutData::setApplicationData(aboutData);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(u"org.kde.qthyperx"_s));

    QQmlApplicationEngine engine;

    auto config = QtHyperXConfig::self();

	HyperXBackend backend;
    qmlRegisterSingletonInstance("org.kde.qthyperx.private", 1, 0, "Config", config);
    qmlRegisterSingletonInstance("org.kde.qthyperx", 1, 0, "HyperXBackend", &backend);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.kde.qthyperx", u"Main"_s);

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
