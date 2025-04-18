// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.statefulapp as StatefulApp
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.qthyperx
import org.kde.qthyperx.settings as Settings

StatefulApp.StatefulWindow {
	id: root

	title: i18nc("@title:window", "QtHyperX")

	windowName: "QtHyperX"

	minimumWidth: Kirigami.Units.gridUnit * 20
	minimumHeight: Kirigami.Units.gridUnit * 20

	application: QtHyperXApplication {
		configurationView: Settings.QtHyperXConfigurationView {}
	}

	globalDrawer: Kirigami.GlobalDrawer {
		isMenu: !Kirigami.Settings.isMobile
		actions: [
			Kirigami.Action {
				id: getBatteryAction
				fromQAction: root.application.action("send_battery_status")
			},
			Kirigami.Action {
				separator: true
			},
			Kirigami.Action {
				fromQAction: root.application.action("options_configure")
			},
			Kirigami.Action {
				fromQAction: root.application.action("options_configure_keybinding")
			},
			Kirigami.Action {
				separator: true
			},
			Kirigami.Action {
				id: aboutAction
				fromQAction: root.application.action("open_about_page")
			},
			Kirigami.Action {
				fromQAction: root.application.action("open_about_kde_page")
			},
			Kirigami.Action {
				id: quitAction
				fromQAction: root.application.action("file_quit")
			}
		]
	}

	pageStack.initialPage: FormCard.FormCardPage {
		id: page

		title: i18nc("@title", "QtHyperXbelp")

		//actions: [incrementCounterAction]

		Kirigami.Icon {
			source: "applications-development"
			implicitWidth: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
			implicitHeight: Math.round(Kirigami.Units.iconSizes.huge * 1.5)

			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Kirigami.Units.largeSpacing * 4
		}

		Kirigami.Heading {
			text: i18nc("@info:status", "%3" + "\n" + "Battery: %1\% -- %2 Hours remaining", HyperXBackend.battery, HyperXBackend.battery * 3, HyperXBackend.connected ? "Connected" : "Disconnected")
			horizontalAlignment: Qt.AlignHCenter

			Layout.topMargin: Kirigami.Units.largeSpacing
			Layout.fillWidth: true
		}

		QQC2.ComboBox {
			Layout.alignment: Qt.AlignHCenter
			textRole: "text"
			valueRole: "value"
			onActivated: HyperXBackend.sleepState = currentValue
			Component.onCompleted: currentIndex = indexOfValue(HyperXBackend.sleepState)
			model: [
				{ value: 0, text: i18nc("@info:sleep:never", "Never") },
				{ value: 0x0a, text: i18nc("@info:sleep:30", "10 Minutes") },
				{ value: 0x14, text: i18nc("@info:sleep:20", "20 Minutes") },
				{ value: 0x1e, text: i18nc("@info:sleep:10", "30 Minutes") },
			]
		}

		QQC2.Switch {
			Layout.alignment: Qt.AlignHCenter
			text: "Voice Prompt"
			checked: HyperXBackend.voicePrompt
			onToggled: HyperXBackend.voicePrompt = checked
		}

		QQC2.Switch {
			Layout.alignment: Qt.AlignHCenter
			text: "Mic Monitor"
			checked: HyperXBackend.micMonitor
			onToggled: HyperXBackend.micMonitor = checked
		}
	}
}
