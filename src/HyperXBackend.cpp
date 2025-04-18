#include "HyperXBackend.hpp"

#include <iostream>
#include <iomanip>

#define VENDOR_ID 0x03f0
#define PRODUCT_ID 0x098d
#define MAX_STR 255

HyperXBackend::HyperXBackend(QObject* parent) : QObject(parent) {
	if (hid_init() < 0) {
		std::cerr << "HID COULD NOT INITIALISE" << std::endl;
		return;
	}
	hid_device_info* info = hid_enumerate(0, 0);
	std::cout << "Enumerating devices" << std::endl;
	while (info != nullptr) {
		if (info->manufacturer_string != nullptr)
			std::wcout << std::wstring(info->manufacturer_string) << " ";
		if (info->product_string != nullptr)
			std::wcout << std::wstring(info->product_string) << " ";
		if (info->serial_number != nullptr)
			std::wcout << std::wstring(info->serial_number);
		std::cout << std::endl << std::endl;
		info = info->next;
	}
	m_Handle = hid_open(VENDOR_ID, PRODUCT_ID, nullptr);
	if (m_Handle == nullptr) {
		std::cerr << "Headset could not be opened" << std::endl;
		return;
	}

	int res = 0;
	wchar_t man[MAX_STR], prod[MAX_STR], ser[MAX_STR];
	res += hid_get_manufacturer_string(m_Handle, man, MAX_STR);
	res += hid_get_product_string(m_Handle, prod, MAX_STR);
	res += hid_get_serial_number_string(m_Handle, ser, MAX_STR);
	if (res < 0) {
		std::cerr << "Could not read device strings" << std::endl;
	}
	std::wcout << "Manufacturer: " << std::wstring(man) << std::endl;
	std::wcout << "Product: " << std::wstring(prod) << std::endl;
	std::wcout << "Serial: " << std::wstring(ser) << std::endl;

	m_TimerID = startTimer(5 * 60 * 1000);
	m_Running = true;
	m_ReadThread = std::thread(&HyperXBackend::readLoop, this);
	sendCommand(HyperXCommand::CONNECTION_STATE);
}

HyperXBackend::~HyperXBackend() noexcept {
	m_Running = false;
	sendCommand(HyperXCommand::PING);
	m_ReadThread.join();
	hid_close(m_Handle);
	killTimer(m_TimerID);
	hid_exit();
	std::cout.flush();
}

unsigned int HyperXBackend::battery() const {
	return m_Battery;
}

void HyperXBackend::setBattery(const unsigned int& battery) {
	m_Battery = battery;
	Q_EMIT batteryChanged();
}

bool HyperXBackend::voicePrompt() const {
	return m_VoicePrompt;
}

void HyperXBackend::setVoicePrompt(const bool& voicePrompt) {
	m_VoicePrompt = voicePrompt;
	if (voicePrompt)
		sendCommand(HyperXCommand::VOICE_PROMPTS);
	else
		sendCommand(HyperXCommand::VOICE_PROMPTS_OFF);
	Q_EMIT voicePromptChanged();
}

bool HyperXBackend::micMonitor() const {
	return m_MicMonitor;
}

void HyperXBackend::setMicMonitor(const bool& micMonitor) {
	m_MicMonitor = micMonitor;
	if (micMonitor)
		sendCommand(HyperXCommand::MICROPHONE_MONITOR);
	else
		sendCommand(HyperXCommand::MICROPHONE_MONITOR_OFF);
	Q_EMIT micMonitorChanged();
}

unsigned int HyperXBackend::sleepState() const {
	return m_SleepState;
}

void HyperXBackend::setSleepState(const unsigned int& sleepState) {
	m_SleepState = sleepState;
	sendCommand(HyperXCommand::SLEEP_TIMER_NONE | sleepState);
	Q_EMIT sleepStateChanged();
}
bool HyperXBackend::connected() const {
	return m_VoicePrompt;
}

void HyperXBackend::setConnected(const bool& connection) {
	m_ConnectionStatus = connection;
	Q_EMIT connectionChanged();
}

void HyperXBackend::sendBatteryStatus() {
	sendCommand(HyperXCommand::STATUS_REQUEST);
}

void HyperXBackend::sendCommand(HyperXCommand cmd) {
	std::cout << "Sending command 0x" << std::hex << (int)cmd << std::endl;
	if (m_Handle == nullptr) {
		std::cerr << "Error sending command: Handle is null" << std::endl;
		return;
	}
	unsigned char bytes[4];
	bytes[0] = ((long)cmd >> 24) & 0xff;
	bytes[1] = ((long)cmd >> 16) & 0xff;
	bytes[2] = ((long)cmd >> 8) & 0xff;
	bytes[3] = (long)cmd & 0xff;
	int written = hid_write(m_Handle, bytes, 4);
	if (written == -1) {
		std::cerr << "Error on sending command 0x" << std::hex << (int)cmd << ": " << hid_error(m_Handle) << std::endl;
	}
}

void HyperXBackend::timerEvent(QTimerEvent* _event) {
	if (m_ConnectionStatus) {
		sendCommand(HyperXCommand::STATUS_REQUEST);
		sendCommand(HyperXCommand::PING);
	} else {
		sendCommand(HyperXCommand::CONNECTION_STATE);
	}
}

void HyperXBackend::onConnect() {
	sendCommand(HyperXCommand::VOICE_STATE);
	sendCommand(HyperXCommand::MIC_MONITOR_STATE);
	sendCommand(HyperXCommand::SLEEP_STATE);
	sendCommand(HyperXCommand::MICROPHONE_STATE);
	sendCommand(HyperXCommand::STATUS_REQUEST);
	setConnected(true);
}

void HyperXBackend::readLoop() {
	unsigned char buffer[32];
	memset(buffer, 0, 32);
	while (m_Running) {
		if (m_Handle == nullptr) {
			std::cerr << "Error while reading: Handle is null" << std::endl;
			return;
		}
		int bytesRead = hid_read(m_Handle, buffer, 32);
		std::cout << std::dec << "Bytes read: " << bytesRead << " \t(0x" << std::hex;
		for (int i = 0; i < bytesRead; i++) {
			std::cout << std::setfill('0') << std::setw(2) <<  (int)buffer[i];
		}
		std::cout << ")" << std::endl;
		std::cout.flush();
		if (bytesRead < 3) {
			//std::cout << "Bytes read were less than 3" << std::endl;
			continue;
		}
		if (buffer[0] != 0x21 || buffer[1] != 0xbb) {
			//std::cout << "Error while reading: Bytes were unexpected start" << std::endl;
			continue;
		}
		switch (buffer[2]) {
			case 0x03:
				if (buffer[3] == 0x01) {
					setConnected(false);
				} else if (buffer[3] == 0x02) {
					onConnect();
				}
				break;

			// READ SLEEP STATE SETTTING
			case 0x07:
				m_SleepState = buffer[3];
				Q_EMIT sleepStateChanged();
				break;

			// VOICE PROMPTS
			case 0x09:
			case 0x13: // Response
				m_VoicePrompt = buffer[3] == 0x01;
				Q_EMIT voicePromptChanged();
				break;

			// Battery Check
			case 0x0b:
				m_Battery = (unsigned int)buffer[3];
				Q_EMIT batteryChanged();
				break;

			case 0x0d:
				m_Identifier =
				(unsigned long)buffer[3] << 40 | (unsigned long)buffer[4] << 32 |
				(unsigned long)buffer[5] << 24 | (unsigned long)buffer[6] << 16 |
				(unsigned long)buffer[7] << 8 | (unsigned long)buffer[8];
				break;

			// RESPONSE TO SLEEP TIMER SET
			case 0x12:
				m_SleepState = buffer[3];
				Q_EMIT sleepStateChanged();
				break;

			// MICMONIITOR RESPONSE
			case 0x22:
				m_MicMonitor = buffer[3] == 0x01;
				Q_EMIT micMonitorChanged();
				break;

			// MIC mute
			case 0x23:
				m_Muted = buffer[3] == 0x01;
				break;

			// POWER OFF
			case 0x24:
				if (buffer[3] == 0x01) {
					setConnected(false);
				} else if (buffer[3] == 0x02) {
					onConnect();
				}
				break;
		}
	}
}

