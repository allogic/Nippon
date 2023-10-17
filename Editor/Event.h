#pragma once

#include <Common/Types.h>

struct GLFWwindow;

namespace ark
{
	typedef enum
	{
		eKeyCodeA = 65,
		eKeyCodeB = 66,
		eKeyCodeC = 67,
		eKeyCodeD = 68,
		eKeyCodeE = 69,
		eKeyCodeF = 70,
		eKeyCodeG = 71,
		eKeyCodeH = 72,
		eKeyCodeI = 73,
		eKeyCodeJ = 74,
		eKeyCodeK = 75,
		eKeyCodeL = 76,
		eKeyCodeM = 77,
		eKeyCodeN = 78,
		eKeyCodeO = 79,
		eKeyCodeP = 80,
		eKeyCodeQ = 81,
		eKeyCodeR = 82,
		eKeyCodeS = 83,
		eKeyCodeT = 84,
		eKeyCodeU = 85,
		eKeyCodeV = 86,
		eKeyCodeW = 87,
		eKeyCodeX = 88,
		eKeyCodeY = 89,
		eKeyCodeZ = 90,

		eKeyCodeLeftShift = 340,
		eKeyCodeLeftCtrl = 241,

		eKeyCodeRightShift = 344,
		eKeyCodeRightCtrl = 245,
	} KeyCode;

	typedef enum
	{
		eMouseCode1 = 0,
		eMouseCode2 = 1,
		eMouseCode3 = 2,
		eMouseCode4 = 3,
		eMouseCode5 = 4,
		eMouseCode6 = 5,
		eMouseCode7 = 6,
		eMouseCode8 = 7,
		eMouseCodeLeft = 0,
		eMouseCodeRight = 1,
		eMouseCodeMiddle = 2,
	} MouseCode;

	class Event
	{
	private:

		enum EventState
		{
			eEventStateNone,
			eEventStateDown,
			eEventStateHeld,
			eEventStateUp,
		};

		struct EventRecord
		{
			EventState Curr;
			EventState Prev;
		};

	public:

		static void Poll(GLFWwindow* Context);

	public:

		static inline U32 KeyDown(U32 Key) { return sKeyboardKeys[Key].Curr == eEventStateDown; }
		static inline U32 KeyHeld(U32 Key) { return sKeyboardKeys[Key].Curr == eEventStateHeld; }
		static inline U32 KeyUp(U32 Key) { return sKeyboardKeys[Key].Curr == eEventStateUp; }

	public:

		static inline U32 MouseDown(U32 Key) { return sMouseKeys[Key].Curr == eEventStateDown; }
		static inline U32 MouseHeld(U32 Key) { return sMouseKeys[Key].Curr == eEventStateHeld; }
		static inline U32 MouseUp(U32 Key) { return sMouseKeys[Key].Curr == eEventStateUp; }

	public:

		static inline R32 GetMouseX() { return sMouseX; }
		static inline R32 GetMouseY() { return sMouseY; }
		static inline R32V2 GetMousePosition() { return { sMouseX, sMouseY }; }

	public:

		static inline void SetMouseX(R32 X) { sMouseX = X; }
		static inline void SetMouseY(R32 Y) { sMouseY = Y; }

	private:

		static inline EventRecord sKeyboardKeys[348] = {};
		static inline EventRecord sMouseKeys[7] = {};

		static inline R32 sMouseX = 0.0F;
		static inline R32 sMouseY = 0.0F;
	};
}
