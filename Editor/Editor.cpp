#include <Common/Version.h>
#include <Common/Memory.h>

#include <Common/Misc/CommandLine.h>
#include <Common/Misc/BlowFish.h>

#include <Editor/Editor.h>
#include <Editor/Event.h>

#include <Editor/Database/Database.h>

#include <Editor/Font/MaterialDesignIcons.h>

#include <Editor/Glad/glad.h>

#include <Editor/Glfw/glFw3.h>

#include <Editor/Interface/ArchiveBrowser.h>
#include <Editor/Interface/Inspector.h>
#include <Editor/Interface/Log.h>
#include <Editor/Interface/MainMenu.h>
#include <Editor/Interface/Outline.h>

#include <Editor/ImGui/imgui.h>
#include <Editor/ImGui/imgui_impl_glfw.h>
#include <Editor/ImGui/imgui_impl_opengl3.h>

#include <Editor/Renderer/Renderer.h>

#include <Editor/Scene/SceneManager.h>

#define UPDATE_FPS (/*1.0F / 120.0F*/ 0.0F)
#define RENDER_FPS (/*1.0F / 120.0F*/ 0.0F)

namespace Nippon
{
	R32 gTime = 0.0F;
	R32 gPrevTime = 0.0F;
	R32 gPrevUpdateTime = 0.0F;
	R32 gPrevRenderTime = 0.0F;
	R32 gDeltaTime = 0.0F;
}

static GLFWwindow* sGlfwContext = nullptr;

using namespace Nippon;

static void GlfwCursorCallback(GLFWwindow* Context, R64 X, R64 Y)
{
	Event::SetMouseX((R32)X);
	Event::SetMouseY((R32)Y);
}

static void GlDebugCallback(U32 Source, U32 Type, U32 Id, U32 Severity, I32 Length, char const* Message, void const* UserParam)
{
	switch (Severity)
	{
		case GL_DEBUG_SEVERITY_NOTIFICATION: break;
		case GL_DEBUG_SEVERITY_LOW: std::printf("[GL] Severity:Low Type:0x%x Message:%s\n", Type, Message); break;
		case GL_DEBUG_SEVERITY_MEDIUM: std::printf("[GL] Severity:Medium Type:0x%x Message:%s\n", Type, Message); break;
		case GL_DEBUG_SEVERITY_HIGH: std::printf("[GL] Severity:High Type:0x%x Message:%s\n", Type, Message); break;
	}
}

static void ImGuiSetupIo()
{
	ImFontConfig fontConfig = {};

	fontConfig.OversampleH = 1;
	fontConfig.OversampleV = 1;
	fontConfig.PixelSnapH = true;
	fontConfig.MergeMode = true;

	static ImWchar glyphRanges[] =
	{
		0xF06D0, 0xF06D0, // ICON_MDI_EYE_OUTLINE
		0xF06D1, 0xF06D1, // ICON_MDI_EYE_OFF_OUTLINE
		0xF1398, 0xF1398, // ICON_MDI_CLOSE_THICK
		0xF0415, 0xF0415, // ICON_MDI_PLUS
		0xF0374, 0xF0374, // ICON_MDI_MINUS
		0xF1C18, 0xF1C18, // ICON_MDI_RENAME
		0xF0207, 0xF0207, // ICON_MDI_EXPORT
		0xF0493, 0xF0493, // ICON_MDI_COG
		0xF1064, 0xF1064, // ICON_MDI_TOOLS
		0xF003C, 0xF003C, // ICON_MDI_ARCHIVE
		0xF01BC, 0xF01BC, // ICON_MDI_DATABASE
		0xF0645, 0xF0645, // ICON_MDI_FILE_TREE
		0xF0900, 0xF0900, // ICON_MDI_PLAYLIST_EDIT
		0xF09AA, 0xF09AA, // ICON_MDI_TEXT_LONG
		0xF0574, 0xF0574, // ICON_MDI_VIEW_QUILT
		0,
	};

	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.ConfigWindowsMoveFromTitleBarOnly = true;

	io.Fonts->AddFontFromFileTTF("CommitMono-400-Regular.otf", 14.0f);
	io.Fonts->AddFontFromFileTTF("MaterialDesignIcons-Webfont.ttf", 14.0f, &fontConfig, glyphRanges);
	io.Fonts->Build();
}
static void ImGuiSetupStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0F;
	style.DisabledAlpha = 0.6000000238418579F;
	style.WindowPadding = ImVec2{ 8.0F, 8.0F };
	style.WindowRounding = 0.0F;
	style.WindowBorderSize = 0.0F;
	style.WindowMinSize = ImVec2{ 32.0F, 32.0F };
	style.WindowTitleAlign = ImVec2{ 0.0F, 0.5F };
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 0.0F;
	style.ChildBorderSize = 1.0F;
	style.PopupRounding = 0.0F;
	style.PopupBorderSize = 1.0F;
	style.FramePadding = ImVec2{ 7.5F, 7.5F };
	style.FrameRounding = 0.0F;
	style.FrameBorderSize = 1.0F;
	style.ItemSpacing = ImVec2{ 8.0F, 4.0F };
	style.ItemInnerSpacing = ImVec2{ 4.0F, 4.0F };
	style.CellPadding = ImVec2{ 4.0F, 2.0F };
	style.IndentSpacing = 21.0F;
	style.ColumnsMinSpacing = 6.0F;
	style.ScrollbarSize = 25.0F;
	style.ScrollbarRounding = 0.0F;
	style.GrabMinSize = 25.0F;
	style.GrabRounding = 0.0F;
	style.TabRounding = 0.0F;
	style.TabBorderSize = 1.0F;
	style.TabMinWidthForCloseButton = 0.0F;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2{ 0.5F, 0.5F };
	style.SelectableTextAlign = ImVec2{ 0.0F, 0.0F };

	style.Colors[ImGuiCol_Text] = ImVec4{ 1.0F, 1.0F, 1.0F, 1.0F };
	style.Colors[ImGuiCol_TextDisabled] = ImVec4{ 0.4980392158031464F, 0.4980392158031464F, 0.4980392158031464F, 1.0F };
	style.Colors[ImGuiCol_WindowBg] = ImVec4{ 0.1764705926179886F, 0.1764705926179886F, 0.1764705926179886F, 1.0F };
	style.Colors[ImGuiCol_ChildBg] = ImVec4{ 0.2784313857555389F, 0.2784313857555389F, 0.2784313857555389F, 0.0F };
	style.Colors[ImGuiCol_PopupBg] = ImVec4{ 0.3098039329051971F, 0.3098039329051971F, 0.3098039329051971F, 1.0F };
	style.Colors[ImGuiCol_Border] = ImVec4{ 0.2627451121807098F, 0.2627451121807098F, 0.2627451121807098F, 1.0F };
	style.Colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0F, 0.0F, 0.0F, 0.0F };
	style.Colors[ImGuiCol_FrameBg] = ImVec4{ 0.1568627506494522F, 0.1568627506494522F, 0.1568627506494522F, 1.0F };
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.2000000029802322F, 0.2000000029802322F, 0.2000000029802322F, 1.0F };
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.2784313857555389F, 0.2784313857555389F, 0.2784313857555389F, 1.0F };
	style.Colors[ImGuiCol_TitleBg] = ImVec4{ 0.1450980454683304F, 0.1450980454683304F, 0.1450980454683304F, 1.0F };
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.1450980454683304F, 0.1450980454683304F, 0.1450980454683304F, 1.0F };
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.1450980454683304F, 0.1450980454683304F, 0.1450980454683304F, 1.0F };
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.1921568661928177F, 0.1921568661928177F, 0.1921568661928177F, 1.0F };
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.1568627506494522F, 0.1568627506494522F, 0.1568627506494522F, 1.0F };
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.2745098173618317F, 0.2745098173618317F, 0.2745098173618317F, 1.0F };
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.2980392277240753F, 0.2980392277240753F, 0.2980392277240753F, 1.0F };
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 1.0F, 0.3882353007793427F, 0.0F, 1.0F };
	style.Colors[ImGuiCol_CheckMark] = ImVec4{ 1.0F, 1.0F, 1.0F, 1.0F };
	style.Colors[ImGuiCol_SliderGrab] = ImVec4{ 0.3882353007793427F, 0.3882353007793427F, 0.3882353007793427F, 1.0F };
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4{ 1.0F, 0.3882353007793427F, 0.0F, 1.0F };
	style.Colors[ImGuiCol_Button] = ImVec4{ 1.0F, 1.0F, 1.0F, 0.0F };
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4{ 1.0F, 1.0F, 1.0F, 0.1560000032186508F };
	style.Colors[ImGuiCol_ButtonActive] = ImVec4{ 1.0F, 1.0F, 1.0F, 0.3910000026226044F };
	style.Colors[ImGuiCol_Header] = ImVec4{ 0.3098039329051971F, 0.3098039329051971F, 0.3098039329051971F, 1.0F };
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.4666666686534882F, 0.4666666686534882F, 0.4666666686534882F, 1.0F };
	style.Colors[ImGuiCol_HeaderActive] = ImVec4{ 0.4666666686534882F, 0.4666666686534882F, 0.4666666686534882F, 1.0F };
	style.Colors[ImGuiCol_Separator] = ImVec4{ 0.2627451121807098F, 0.2627451121807098F, 0.2627451121807098F, 1.0F };
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.3882353007793427F, 0.3882353007793427F, 0.3882353007793427F, 1.0F };
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4{ 1.0F, 0.3882353007793427F, 0.0F, 1.0F };
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.0F, 0.0F, 0.0F, 0.0F };
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.0F, 0.0F, 0.0F, 0.0F };
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.0F, 0.0F, 0.0F, 0.0F };
	style.Colors[ImGuiCol_Tab] = ImVec4{ 0.09411764889955521F, 0.09411764889955521F, 0.09411764889955521F, 1.0F };
	style.Colors[ImGuiCol_TabHovered] = ImVec4{ 0.3490196168422699F, 0.3490196168422699F, 0.3490196168422699F, 1.0F };
	style.Colors[ImGuiCol_TabActive] = ImVec4{ 0.1921568661928177F, 0.1921568661928177F, 0.1921568661928177F, 1.0F };
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.09411764889955521F, 0.09411764889955521F, 0.09411764889955521F, 1.0F };
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.1921568661928177F, 0.1921568661928177F, 0.1921568661928177F, 1.0F };
	style.Colors[ImGuiCol_PlotLines] = ImVec4{ 0.4666666686534882F, 0.4666666686534882F, 0.4666666686534882F, 1.0F };
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4{ 1.0F, 0.3882353007793427F, 0.0F, 1.0F };
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4{ 0.5843137502670288F, 0.5843137502670288F, 0.5843137502670288F, 1.0F };
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ 1.0F, 0.3882353007793427F, 0.0F, 1.0F };
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4{ 0.1882352977991104F, 0.1882352977991104F, 0.2000000029802322F, 1.0F };
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4{ 0.3098039329051971F, 0.3098039329051971F, 0.3490196168422699F, 1.0F };
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4{ 0.2274509817361832F, 0.2274509817361832F, 0.2470588237047195F, 1.0F };
	style.Colors[ImGuiCol_TableRowBg] = ImVec4{ 0.0F, 0.0F, 0.0F, 0.0F };
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4{ 1.0F, 1.0F, 1.0F, 0.05999999865889549F };
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4{ 1.0F, 1.0F, 1.0F, 0.1560000032186508F };
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4{ 1.0F, 0.3882353007793427F, 0.0F, 1.0F };
	style.Colors[ImGuiCol_NavHighlight] = ImVec4{ 1.0F, 0.3882353007793427F, 0.0F, 1.0F };
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4{ 1.0F, 0.3882353007793427F, 0.0F, 1.0F };
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4{ 0.0F, 0.0F, 0.0F, 0.5860000252723694F };
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4{ 0.0F, 0.0F, 0.0F, 0.5860000252723694F };
}

I32 main(I32 Argc, char** Argv)
{
	if (glfwInit())
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_SAMPLES, 8);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);

		sGlfwContext = glfwCreateWindow(1920, 1080, "Editor", nullptr, nullptr);

		if (sGlfwContext)
		{
			glfwMakeContextCurrent(sGlfwContext);
			glfwSetCursorPosCallback(sGlfwContext, GlfwCursorCallback);
			glfwSwapInterval(0);

			gladLoadGL();

			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(GlDebugCallback, 0);

			IMGUI_CHECKVERSION();

			ImGuiContext* imGuiContext = ImGui::CreateContext();

			ImGuiSetupIo();
			ImGuiSetupStyle();

			ImGui_ImplGlfw_InitForOpenGL(sGlfwContext, 1);
			ImGui_ImplOpenGL3_Init("#version 460 core");

			BlowFish::Create(OKAMI_CIPHER_KEY);
			Database::Create();
			SceneManager::Create();

			ArchiveBrowser::Reset();
			Inspector::Reset();
			Log::Reset();
			MainMenu::Reset();
			Outline::Reset();

			Log::Add("Nippon - Okami HD Editor (C) Made by 0x616c\n");
			Log::Add("  - Version %s\n", NIPPON_VERSION_STR);
			Log::Add("  - https://github.com/allogic/Nippon\n");
			Log::Add("\n");

			while (!glfwWindowShouldClose(sGlfwContext))
			{
				gTime = (R32)glfwGetTime();
				gDeltaTime = gTime - gPrevTime;
				gPrevTime = gTime;

				R32 deltaUpdateTime = gTime - gPrevUpdateTime;
				R32 deltaRenderTime = gTime - gPrevRenderTime;

				SceneManager::FixedUpdate();

				if (deltaUpdateTime >= UPDATE_FPS)
				{
					gPrevUpdateTime = gTime;

					SceneManager::Update();
				}

				if (deltaRenderTime >= RENDER_FPS)
				{
					gPrevRenderTime = gTime;

					glClearColor(0.125F, 0.125F, 0.125F, 1.0F);
					glClear(GL_COLOR_BUFFER_BIT);

					ImGui_ImplGlfw_NewFrame();
					ImGui_ImplOpenGL3_NewFrame();

					ImGui::NewFrame();
					ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

					ArchiveBrowser::Render();
					Inspector::Render();
					Log::Render();
					MainMenu::Render();
					Outline::Render();

					SceneManager::Render();

					ImGui::Render();

					ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();

					glfwMakeContextCurrent(sGlfwContext);
					glfwSwapBuffers(sGlfwContext);
				}

				Event::Poll(sGlfwContext);
			}

			SceneManager::Destroy();
			Database::Destroy();

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();

			ImGui::DestroyContext(imGuiContext);

			glfwDestroyWindow(sGlfwContext);
			glfwTerminate();
		}
		else
		{
			std::printf("Failed creating window\n");
		}
	}
	else
	{
		std::printf("Failed initializing GLFW\n");
	}

	return 0;
}