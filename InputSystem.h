#pragma once
#include <string>
#include <memory>
#include <Windows.h>
#include <chrono>
#include <vector>

#ifdef __DEBUG__
#include <iostream>
#endif

namespace IO
{
	class Input
	{
	public:

		Input()
			: m_HoldTime(0.50f), m_IsAnyKeyPressed(false)
		{
			for (int i = 0; i < 256; i++)
			{				
				m_KeyDown[i] = false;
				m_KeyRelease[i] = false;
				m_KeyPressed[i] = false;

				m_KeyHeld[i] = false;
				m_KeyHeldPrev[i] = false;
				m_KeyHeldOnce[i] = false;

				m_RepeatKeyDown[i] = false;
				

				m_KeyDownDuration[i] = 0.0f;
				m_KeyDownIntrevelDuration[i] = 0.0f;
			}

			m_Start = std::chrono::high_resolution_clock::now();

			m_GetKeys(m_KeyStates);
			memcpy(m_OldKeyStates, m_KeyStates, sizeof(unsigned char) * 256);
		}

		void update() //Updates keyboard snapshot
		{
			m_GetKeys(m_KeyStates);
			m_IsAnyKeyPressed = false;
			
			bool RepeatKeyPressed = false;
			m_PressedKeys.clear();

			for (int i = 0; i < 256; i++)
			{
				if (m_KeyStates[i] & 0x80) //KEY IS DOWN
				{
					m_IsAnyKeyPressed = true;

					m_PressedKeys.push_back(i);

					m_KeyDown[i] = true;
					
					/*----------DETECTS SINGLE KEY PRESS EVENT----------*/
					if (m_KeyStates[i] != m_OldKeyStates[i])
					{
						m_KeyPressed[i] = true;
						m_RepeatKeyDown[i] = true; // Key type that detects single key press... waits a little bit and then sends them as fast as possible
						RepeatKeyPressed = true;
					}
					else
					{
						m_KeyPressed[i] = false;
						m_RepeatKeyDown[i] = false; // Key type that detects single key press... waits a little bit and then sends them as fast as possible
					}
					
					m_KeyDownIntrevelDuration[i] += m_GetTime(m_Start, std::chrono::high_resolution_clock::now()).count();
					/*-----------DETECTS SINGLE KEY HELD EVENT----------*/
					m_KeyDownDuration[i] += m_GetTime(m_Start, std::chrono::high_resolution_clock::now()).count();
					
					if (m_KeyDownDuration[i] >= m_HoldTime)
					{
						m_KeyHeld[i] = true;
						m_RepeatKeyDown[i] = true;
						
						if (m_KeyHeld[i] != m_KeyHeldPrev[i])
						{
							m_KeyHeldOnce[i] = true;
						}
						else
						{
							m_KeyHeldOnce[i] = false;
						}
					}
				}
				else //KEY IS UP
				{
					m_KeyHeld[i] = false;
					m_KeyDown[i] = false;

					m_RepeatKeyDown[i] = false;

					m_KeyDownDuration[i] = 0.0f;

					if (m_KeyStates[i] != m_OldKeyStates[i])
					{
						m_KeyRelease[i] = true;
					}
					else
					{
						m_KeyRelease[i] = false;
					}
				}
			}

			memcpy(m_OldKeyStates, m_KeyStates, sizeof(unsigned char) * 256);
			memcpy(m_KeyHeldPrev, m_KeyHeld, sizeof(bool) * 256);
			m_Start = std::chrono::high_resolution_clock::now();
		}

		inline const bool GetKey(int KeyCode) const {return m_KeyDown[KeyCode]; }

		inline const bool GetKeyUp(int KeyCode) const {return m_KeyRelease[KeyCode]; }

		inline const bool GetKeyDown(int KeyCode) const{ return m_KeyPressed[KeyCode]; }
		
		inline const bool GetAnyKey() const { return m_IsAnyKeyPressed;}

		inline const bool GetKeyBeingHeld(int KeyCode, float seconds) const { return m_KeyDownDuration[KeyCode] >= seconds; }
		
		inline const bool GetKeyBeingHeld(int KeyCode) const { return m_KeyHeld[KeyCode]; }

		inline const float GetKeyHeldDur(int KeyCode) const { return m_KeyDownDuration[KeyCode]; }

		inline const bool GetKeyHeld(int KeyCode) const { return m_KeyHeldOnce[KeyCode]; }
		
		inline const bool GetRepeatKeyDown(int KeyCode) const { return m_RepeatKeyDown[KeyCode]; }

		inline const bool GetRepeatKeyDown(int KeyCode, float seconds)
		{
			if (m_KeyDownIntrevelDuration[KeyCode] >= seconds)
			{
				m_KeyDownIntrevelDuration[KeyCode] = 0.0f;
				return m_RepeatKeyDown[KeyCode];
			}

			return m_KeyPressed[KeyCode];
		}

		inline const std::vector<unsigned char>& GetPressedKeys() const { return m_PressedKeys; }

		inline const unsigned char* GetKeyStates() const { return m_KeyStates; }

		/*----------GETS THE KEY NAME OF KEY CODE----------*/
		std::string GetKeyName(int KeyCode)
		{
			if (KeyCode >= 'A' && KeyCode <= 'Z' ||
				KeyCode >= '0' && KeyCode <= '9') 
			{
				return std::string(1, char(KeyCode));
			}

			else
			{
				switch (KeyCode)
				{
				case VK_LBUTTON:	return "Mouse1";
				case VK_RBUTTON:	return "Mouse2";
				case VK_CANCEL:		return "Cancel";
				case VK_MBUTTON:	return "Mouse3";
				case VK_BACK:		return "Backspace";
				case VK_TAB:		return "Tab";
				case VK_RETURN:		return "Enter";
				case VK_SHIFT:		return "Shift";
				case VK_CONTROL:	return "Ctrl";
				case VK_MENU:		return "Alt";
				case VK_PAUSE:		return "Pause";
				case VK_CAPITAL:	return "Caps lock";
				case VK_ESCAPE:		return "Escape";
				case VK_SPACE:		return "Space";
				case VK_PRIOR:		return "Page up";
				case VK_NEXT:		return "Page down";
				case VK_END:		return "End";
				case VK_HOME:		return "Home";
				case VK_LEFT:		return "Left arrow";
				case VK_UP:			return "Up arrow";
				case VK_RIGHT:		return "Right arrow";
				case VK_DOWN:		return "Down arrow";
				case VK_PRINT:		return "Print";
				case VK_SNAPSHOT:	return "Prnt screen";
				case VK_INSERT:		return "Insert";
				case VK_DELETE:		return "Delete";
				case VK_LWIN:		return "Left window";
				case VK_RWIN:		return "Right window";
				case VK_NUMPAD0:	return "Num 0";
				case VK_NUMPAD1:	return "Num 1";
				case VK_NUMPAD2:	return "Num 2";
				case VK_NUMPAD3:	return "Num 3";
				case VK_NUMPAD4:	return "Num 4";
				case VK_NUMPAD5:	return "Num 5";
				case VK_NUMPAD6:	return "Num 6";
				case VK_NUMPAD7:	return "Num 7";
				case VK_NUMPAD8:	return "Num 8";
				case VK_NUMPAD9:	return "Num 9";
				case VK_MULTIPLY:	return "*";
				case VK_ADD:		return "+";
				case VK_SEPARATOR:	return "Seperator";
				case VK_SUBTRACT:	return "-";
				case VK_DECIMAL:	return ".";
				case VK_DIVIDE:		return "/";
				case VK_F1:			return "F1";
				case VK_F2:			return "F2";
				case VK_F3:			return "F3";
				case VK_F4:			return "F4";
				case VK_F5:			return "F5";
				case VK_F6:			return "F6";
				case VK_F7:			return "F7";
				case VK_F8:			return "F8";
				case VK_F9:			return "F9";
				case VK_F10:		return "F10";
				case VK_F11:		return "F11";
				case VK_F12:		return "F12";
				case VK_F13:		return "F13";
				case VK_F14:		return "F14";
				case VK_F15:		return "F15";
				case VK_F16:		return "F16";
				case VK_F17:		return "F17";
				case VK_F18:		return "F18";
				case VK_F19:		return "F19";
				case VK_F20:		return "F20";
				case VK_F21:		return "F21";
				case VK_F22:		return "F22";
				case VK_F23:		return "F23";
				case VK_F24:		return "F24";
				case VK_NUMLOCK:	return "Numlock";
				case VK_SCROLL:		return "Scroll lock";
				case VK_LSHIFT:		return "Left shift";
				case VK_RSHIFT:		return "Right shift";
				case VK_LCONTROL:	return "Left ctrl";
				case VK_RCONTROL:	return "Right ctrl";
				case VK_LMENU:		return "Left alt";
				case VK_RMENU:		return "Right alt";
				case 186:			return ";";
				case 187:			return "=";
				case 188:			return ",";
				case 189:			return "-";
				case 190:			return ".";
				case 191:			return "/";
				case 192:			return "`";
				case 219:			return "[";
				case 220:			return"\\";
				case 221:			return"]";
				case 222:			return "'";
				default: 			return std::to_string(KeyCode);
				}
			}
		}
		
		inline void ChangeHoldTime(float seconds) { m_HoldTime = seconds; }


	private:
		float m_HoldTime;

		bool m_IsAnyKeyPressed;

		unsigned char m_KeyStates[256];
		unsigned char m_OldKeyStates[256];

		bool m_KeyDown[256];
		bool m_KeyRelease[256];
		bool m_KeyPressed[256];

		bool m_KeyHeld[256];
		bool m_KeyHeldPrev[256];
		bool m_KeyHeldOnce[256];
		
		bool m_RepeatKeyDown[256]; // Key type that detects single key press... waits a little bit and then sends them as fast as possible
		
		float m_KeyDownDuration[256];
		float m_KeyDownIntrevelDuration[256];

		std::vector<unsigned char> m_PressedKeys;

		std::chrono::steady_clock::time_point m_Start;
		

		inline const void m_GetKeys(unsigned char arr[]) const 
			{ GetKeyState(0); GetKeyboardState(arr); }

		inline const std::chrono::duration<float> m_GetTime(const std::chrono::steady_clock::time_point& start, const std::chrono::steady_clock::time_point& end) 
			{ return end - start; }
	};

}