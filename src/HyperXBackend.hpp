#ifndef HYPERX_BACKEND_HPP
#define HYPERX_BACKEND_HPP

#include <QObject>
#include <hidapi/hidapi.h>
#include <thread>

enum class HyperXCommand : int {
	STATUS_REQUEST = 0x21bb0b00,
	PING = 0x21bb0c00,
	GET_INDENTIFIER = 0x21bb0d00,
	CONNECTION_STATE = 0x21bb0300,
	MICROPHONE_STATE = 0x21bb0500,
	VOICE_STATE = 0x21bb0900,
	VOICE_PROMPTS = 0x21bb1301,
	VOICE_PROMPTS_OFF = 0x21bb1300,
	MIC_MONITOR_STATE = 0x21bb0a00,
	MICROPHONE_MONITOR = 0x21bb1001,
	MICROPHONE_MONITOR_OFF = 0x21bb1000,
	SLEEP_STATE = 0x21bb0700,
	SLEEP_TIMER_NONE = 0x21bb1200,
	SLEEP_TIMER_10 = 0x21bb120a,
	SLEEP_TIMER_20 = 0x21bb1214,
	SLEEP_TIMER_30 = 0x21bb121e
};
inline constexpr HyperXCommand operator|(HyperXCommand x, unsigned int y) {
	return static_cast<HyperXCommand>(static_cast<unsigned int>(x) | y);
}

class HyperXBackend : public QObject {
	Q_OBJECT
	Q_PROPERTY(unsigned int battery READ battery WRITE setBattery NOTIFY batteryChanged)
	Q_PROPERTY(bool voicePrompt READ voicePrompt WRITE setVoicePrompt NOTIFY voicePromptChanged)
	Q_PROPERTY(bool micMonitor READ micMonitor WRITE setMicMonitor NOTIFY micMonitorChanged)
	Q_PROPERTY(int sleepState READ sleepState WRITE setSleepState NOTIFY sleepStateChanged)
	Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectionChanged)

public:
	explicit HyperXBackend(QObject* parent = nullptr);
	~HyperXBackend();
	unsigned int battery() const;
	void setBattery(const unsigned int& battery);
	Q_SIGNAL void batteryChanged();

	bool voicePrompt() const;
	void setVoicePrompt(const bool& voicePrompt);
	Q_SIGNAL void voicePromptChanged();

	bool micMonitor() const;
	void setMicMonitor(const bool& micMonitor);
	Q_SIGNAL void micMonitorChanged();

	unsigned int sleepState() const;
	void setSleepState(const unsigned int& sleepState);
	Q_SIGNAL void sleepStateChanged();

	bool connected() const;
	void setConnected(const bool& connected);
	Q_SIGNAL void connectionChanged();

	Q_SLOT void sendBatteryStatus();

private:
	unsigned int m_Battery = 0;
	bool m_VoicePrompt = false;
	bool m_MicMonitor = false;
	int m_SleepState = 0;
	unsigned long m_Identifier = 0;
	bool m_Muted = false;
	hid_device* m_Handle = nullptr;
	int m_TimerID;
	bool m_ConnectionStatus = false;
	std::thread m_ReadThread;
	std::atomic<bool> m_Running;

	void sendCommand(HyperXCommand cmd);
	void readLoop();
	void onConnect();

protected:
	void timerEvent(QTimerEvent* _event) override;
};

#endif
