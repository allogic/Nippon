#include <Editor/Event.h>

#include <Vendor/GLFW/glfw3.h>

namespace ark
{
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
}
