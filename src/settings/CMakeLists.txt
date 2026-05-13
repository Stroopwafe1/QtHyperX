# SPDX-FileCopyrightText: 2026 Lilith Houtjes <%{EMAIL}>
# SPDX-License-Identifier: BSD-2-Clause

qt_add_library(qthyperx_settings STATIC)
ecm_add_qml_module(qthyperx_settings
    GENERATE_PLUGIN_SOURCE
    URI org.kde.qthyperx.settings
    QML_FILES
        QtHyperXConfigurationView.qml
        GeneralPage.qml
)
