#pragma once

#include <Common/Types.h>

#include <Editor/Forward.h>

namespace Nippon
{
	enum KeyCode : U32
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
	};

	enum MouseCode : U32
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
	};

	class Event
	{
	public:

		static void Poll(GLFWwindow* Context);

	public:

		static U32 KeyDown(U32 Key);
		static U32 KeyHeld(U32 Key);
		static U32 KeyUp(U32 Key);

	public:

		static U32 MouseDown(U32 Key);
		static U32 MouseHeld(U32 Key);
		static U32 MouseUp(U32 Key);

	public:

		static R32 GetMouseX();
		static R32 GetMouseY();
		static R32V2 GetMousePosition();

	public:

		static void SetMouseX(R32 X);
		static void SetMouseY(R32 Y);
	};
}