#include <Event.h>

#include <Ecs/Registry.h>

#include <Font/MaterialDesignIcons.h>

#include <Interface/Viewport.h>
#include <Interface/Outline.h>

#include <ImGui/imgui.h>

#include <OpenGl/FrameBuffer.h>

#include <Scene/Scene.h>
#include <Scene/SceneManager.h>

namespace Nippon
{
	Viewport::Viewport(Scene* Scene)
	{
		mScene = Scene;
	}

	Viewport::~Viewport()
	{

	}

	void Viewport::Reset()
	{
		
	}

	void Viewport::Render()
	{
		static std::string windowNameBuffer = "";

		windowNameBuffer = std::string{ ICON_MDI_VIEW_QUILT " " } + mScene->GetWindowName();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0F, 0.0F });
		ImGui::Begin(windowNameBuffer.data(), &mIsOpen);

		if (mIsOpen)
		{
			if (HasResized())
			{
				mScene->Resize(mWidth, mHeight);
				mScene->Invalidate();
			}

			if (HasGainedFocus())
			{
				SceneManager::SetCurrentScene(mScene);

				mScene->Invalidate();
			}

			if (HasFocus())
			{
				HandleEntitySelection();

				mScene->Render();
			}

			if (HasLostFocus())
			{

			}

			ImGui::Image((ImTextureID)(U64)FrameBuffer::GetColorTexture(mScene->GetFrameBuffer(), 0), ImVec2{ (R32)mWidth, (R32)mHeight }, ImVec2{ 0.0F, 1.0F }, ImVec2{ 1.0F, 0.0F });
		}
		else
		{
			mScene->SetShouldBeDestroyed();
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void Viewport::SetFocused()
	{
		ImGui::SetWindowFocus(mScene->GetWindowName().data());
	}

	bool Viewport::HasResized()
	{
		ImVec2 view = ImGui::GetContentRegionAvail();

		if ((U32)view.x != mWidth || (U32)view.y != mHeight)
		{
			if ((U32)view.x == 0 || (U32)view.y == 0)
			{
				return false;
			}

			mWidth = (U32)view.x;
			mHeight = (U32)view.y;

			return true;
		}

		return false;
	}

	bool Viewport::HasGainedFocus()
	{
		return !mIsFocused && ImGui::IsWindowFocused();
	}

	bool Viewport::HasFocus()
	{
		return mIsFocused = ImGui::IsWindowFocused();
	}

	bool Viewport::HasLostFocus()
	{
		return mIsFocused && !ImGui::IsWindowFocused();
	}

	void Viewport::HandleEntitySelection()
	{
		if (!Event::MouseHeld(eMouseCodeRight))
		{
			if (Event::MouseDown(eMouseCodeLeft))
			{
				ImGuiIO& io = ImGui::GetIO();
				ImGuiStyle& style = ImGui::GetStyle();

				ImVec2 mousePosition = io.MousePos;
				ImVec2 windowPosition = ImGui::GetWindowPos();

				U32 frameBuffer = mScene->GetFrameBuffer();

				I32 windowWidth = FrameBuffer::GetWidth(frameBuffer);
				I32 windowHeight = FrameBuffer::GetHeight(frameBuffer);

				windowPosition.y += ImGui::GetFontSize() + style.FramePadding.y * 2;
				windowPosition.y += windowHeight;

				I32 mousePositionX = glm::abs((I32)(mousePosition.x - windowPosition.x));
				I32 mousePositionY = glm::abs((I32)(mousePosition.y - windowPosition.y));

				if (mousePositionX < 0) mousePositionX = 0;
				if (mousePositionY < 0) mousePositionY = 0;

				if (mousePositionX > windowWidth) mousePositionX = windowWidth;
				if (mousePositionY > windowHeight) mousePositionY = windowHeight;

				U32 actorId = FrameBuffer::ReadInteger(frameBuffer, mousePositionX, mousePositionY);

				if (actorId > 0)
				{
					Registry* registry = mScene->GetRegistry();
					Entity* entity = registry->FindEntityByUniqueId(actorId, nullptr, true);

					Outline::SetSelectedEntity(entity);
				}
			}
		}
	}
}