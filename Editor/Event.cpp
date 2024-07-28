#include <Editor/Event.h>

#include <Editor/Glfw/glfw3.h>

namespace Nippon
{
	enum EventState : U32
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

	static EventRecord sKeyboardKeys[348] = {};
	static EventRecord sMouseKeys[7] = {};

	static R32 sMouseX = 0.0F;
	static R32 sMouseY = 0.0F;

	void Event::Poll(GLFWwindow* Context)
	{
		glfwPollEvents();

		for (U32 i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST; i++)
		{
			I32 action = glfwGetKey(Context, i);

			sKeyboardKeys[i].Prev = sKeyboardKeys[i].Curr;

			if (action == GLFW_PRESS)
			{
				if (sKeyboardKeys[i].Curr != eEventStateDown && sKeyboardKeys[i].Prev != eEventStateHeld)
				{
					sKeyboardKeys[i].Curr = eEventStateDown;
				}
				else
				{
					sKeyboardKeys[i].Curr = eEventStateHeld;
				}
			}

			if (action == GLFW_RELEASE)
			{
				if (sKeyboardKeys[i].Curr != eEventStateUp && sKeyboardKeys[i].Prev == eEventStateHeld)
				{
					sKeyboardKeys[i].Curr = eEventStateUp;
				}
				else
				{
					sKeyboardKeys[i].Curr = eEventStateNone;
				}
			}
		}

		for (U32 i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
		{
			I32 action = glfwGetMouseButton(Context, i);

			sMouseKeys[i].Prev = sMouseKeys[i].Curr;

			if (action == GLFW_PRESS)
			{
				if (sMouseKeys[i].Curr != eEventStateDown && sMouseKeys[i].Prev != eEventStateHeld)
				{
					sMouseKeys[i].Curr = eEventStateDown;
				}
				else
				{
					sMouseKeys[i].Curr = eEventStateHeld;
				}
			}

			if (action == GLFW_RELEASE)
			{
				if (sMouseKeys[i].Curr != eEventStateUp && sMouseKeys[i].Prev == eEventStateHeld)
				{
					sMouseKeys[i].Curr = eEventStateUp;
				}
				else
				{
					sMouseKeys[i].Curr = eEventStateNone;
				}
			}
		}
	}

	U32 Event::KeyDown(U32 Key)
	{
		return sKeyboardKeys[Key].Curr == eEventStateDown;
	}

	U32 Event::KeyHeld(U32 Key)
	{
		return sKeyboardKeys[Key].Curr == eEventStateHeld;
	}

	U32 Event::KeyUp(U32 Key)
	{
		return sKeyboardKeys[Key].Curr == eEventStateUp;
	}

	U32 Event::MouseDown(U32 Key)
	{
		return sMouseKeys[Key].Curr == eEventStateDown;
	}

	U32 Event::MouseHeld(U32 Key)
	{
		return sMouseKeys[Key].Curr == eEventStateHeld;
	}

	U32 Event::MouseUp(U32 Key)
	{
		return sMouseKeys[Key].Curr == eEventStateUp;
	}

	R32 Event::GetMouseX()
	{
		return sMouseX;
	}

	R32 Event::GetMouseY()
	{
		return sMouseY;
	}

	R32V2 Event::GetMousePosition()
	{
		return { sMouseX, sMouseY };
	}

	void Event::SetMouseX(R32 X)
	{
		sMouseX = X;
	}

	void Event::SetMouseY(R32 Y)
	{
		sMouseY = Y;
	}
}