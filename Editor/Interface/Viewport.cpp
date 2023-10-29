#include <Editor/Event.h>
#include <Editor/Scene.h>
#include <Editor/Texture2D.h>
#include <Editor/SceneManager.h>
#include <Editor/InterfaceManager.h>
#include <Editor/FrameBuffer.h>

#include <Editor/Databases/FileDatabase.h>

#include <Editor/Interface/Viewport.h>
#include <Editor/Interface/Outline.h>

#include <Editor/ImGui/imgui.h>

namespace ark
{
	Viewport::Viewport(Scene* Scene)
		: mScene{ Scene }
	{

	}

	void Viewport::Reset()
	{
		mIsOpen = true;
	}

	void Viewport::Render()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0F, 0.0F });
		ImGui::Begin(mScene->GetFileContainer()->GetWindowName(), &mIsOpen);

		if (mIsOpen)
		{
			if (HasResized())
			{
				mScene->Resize(mWidth, mHeight);
				mScene->Invalidate();
			}

			if (HasGainedFocus())
			{
				SceneManager::SetActiveScene(mScene);

				mScene->Invalidate();
			}

			if (HasFocus())
			{
				HandleActorSelection();

				mScene->Render();
			}

			if (HasLostFocus())
			{

			}

			ImGui::Image((void*)(U64)FrameBuffer::GetColorTexture(mScene->GetFrameBuffer(), 0), ImVec2{ (R32)mWidth, (R32)mHeight }, ImVec2{ 0.0F, 1.0F }, ImVec2{ 1.0F, 0.0F });
		}
		else
		{
			mScene->MakeShouldBeDestroyed(true);
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void Viewport::SetFocused()
	{
		ImGui::SetWindowFocus(mScene->GetFileContainer()->GetWindowName());
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

	void Viewport::HandleActorSelection()
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

				I32 windowWidth =  FrameBuffer::GetWidth(frameBuffer);
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
					Actor* actor = mScene->FindActorByIdRecursive(actorId);

					InterfaceManager::GetOutline()->SetSelectedActor(actor);
				}
			}
		}
	}
}
