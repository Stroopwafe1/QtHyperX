# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>

add_subdirectory(settings)

qt_add_library(qthyperx_static STATIC)
ecm_add_qml_module(qthyperx_static
    URI org.kde.qthyperx
    GENERATE_PLUGIN_SOURCE
    VERSION 1.0
    QML_FILES
        qml/Main.qml
    DEPENDENCIES
        org.kde.kirigamiaddons.formcard
        org.kde.kirigamiaddons.statefulapp
        org.kde.kirigami
        org.kde.qthyperx.settings
)

target_sources(qthyperx_static PUBLIC
    qthyperxapplication.cpp
    qthyperxapplication.h
)

pkg_check_modules(HIDAPI REQUIRED IMPORTED_TARGET hidapi-hidraw)

target_link_libraries(qthyperx_static PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
    KF6::I18n
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigGui
    KirigamiAddonsStatefulApp
)
target_include_directories(qthyperx_static PUBLIC ${CMAKE_BINARY_DIR})

if (ANDROID)
    kirigami_package_breeze_icons(ICONS
        list-add
        help-about
        application-exit
        applications-graphics
    )
else()
    target_link_libraries(qthyperx_static PUBLIC Qt::Widgets)
endif()

kconfig_add_kcfg_files(qthyperx_static GENERATE_MOC qthyperxconfig.kcfgc)

add_executable(qthyperx
	main.cpp
	HyperXBackend.cpp
    HyperXBackend.hpp
)
target_link_libraries(qthyperx PUBLIC
    qthyperx_static
    qthyperx_staticplugin
    qthyperx_settings
    qthyperx_settingsplugin
    ${HIDAPI_LIBRARIES}
)
install(TARGETS qthyperx ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
