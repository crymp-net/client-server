#pragma once

#include <cstddef>
#include <string>

#include "CryCommon/CryInput/IInput.h"
#include "CryCommon/CrySystem/IFlashPlayer.h"

#include "HUDObject.h"

class CGameFlashAnimation;
class CHUD;

class CHUDTextChat : public CHUDObject, public IInputEventListener, public IFSCommandHandler
{
	// CryMP: default is 60
	static constexpr std::size_t MAX_MESSAGE_LENGTH = 75;

	CHUD* m_pHUD = nullptr;
	CGameFlashAnimation* m_flashChat;

	std::string m_inputText;
	std::string m_lastInputText;
	std::size_t m_cursor = 0;

	bool m_isListening = false;
	bool m_teamChat = false;
	bool m_showVirtualKeyboard = false;

	float m_repeatTimer = 0;
	SInputEvent m_repeatEvent;

public:
	explicit CHUDTextChat(CHUD* pHUD);
	~CHUDTextChat() override;

	void Init(CGameFlashAnimation* pFlashChat);

	// CHUDObject
	void Update(float deltaTime) override;
	// ~CHUDObject

	// IInputEventListener
	bool OnInputEvent(const SInputEvent& event) override;
	bool OnInputEventUI(const SInputEvent& event) override;
	// ~IInputEventListener

	// IFSCommandHandler
	void HandleFSCommand(const char* command, const char* args) override;
	// ~IFSCommandHandler

	void AddChatMessage(EntityId sourceId, const wchar_t* msg, int teamFaction, bool teamChat);
	void AddChatMessage(EntityId sourceId, const char* msg, int teamFaction, bool teamChat);
	void AddChatMessage(const char* nick, const wchar_t* msg, int teamFaction, bool teamChat);
	void AddChatMessage(const char* nick, const char* msg, int teamFaction, bool teamChat);

	void ShutDown() { this->Flush(); }

	void OpenChat(int type);
	void CloseChat() { this->Flush(); }

private:
	void Delete();
	void Backspace();
	void Left();
	void Right();
	void Insert(char ch);
	void Paste();
	void Flush(bool close = true);
	void ProcessInput(const SInputEvent& event);
	void VirtualKeyboardInput(const char* direction);
};
