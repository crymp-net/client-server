#pragma once

struct ISystem;

class TimeDemoRecorder
{
	// m_reserved_<32-bit-offset>_<64-bit-offset>
	void* m_reserved_0x0_0x0[6] = {};
	bool m_isRecording = false;  // m_reserved_0x18_0x30
	bool m_isPlaying = false;  // m_reserved_0x19_0x31
	unsigned int m_reserved_0x1c_0x38[52] = {};
	void* m_reserved[15] = {};

public:
	explicit TimeDemoRecorder(ISystem* pSystem);

	bool IsRecording() const { return m_isRecording; }
	bool IsPlaying() const { return m_isPlaying; }

	void PreUpdate();
	void Update();
};
