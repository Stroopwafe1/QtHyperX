// SPDX-FileCopyrightText: 2026 Lilith Houtjes <%{EMAIL}>
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigamiaddons.settings as KirigamiSettings

KirigamiSettings.ConfigurationView {
    id: root

    modules: [
        KirigamiSettings.ConfigurationModule {
            moduleId: "general"
            text: i18nc("@action:button", "General")
            icon.name: "preferences-system-symbolic"
            page: () => Qt.createComponent("org.kde.qthyperx.settings", "GeneralPage")
        }
    ]
}
