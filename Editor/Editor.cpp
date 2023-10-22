#include <Common/Macros.h>
#include <Common/Types.h>
#include <Common/BlowFish.h>
#include <Common/CommandLine.h>

#include <Common/Utilities/FsUtils.h>

#include <Editor/Event.h>
#include <Editor/InterfaceManager.h>
#include <Editor/SceneManager.h>

#include <Editor/Renderer/DebugRenderer.h>
#include <Editor/Renderer/DefaultRenderer.h>

#include <Editor/ImGui/imgui.h>
#include <Editor/ImGui/imgui_internal.h>
#include <Editor/ImGui/imgui_impl_glfw.h>
#include <Editor/ImGui/imgui_impl_opengl3.h>

#include <Editor/Glad/glad.h>

#include <Vendor/GLFW/glfw3.h>

using namespace ark;

namespace ark
{
	fs::path gDataDir = "";

	BlowFish* gBlowFish = nullptr;

	R32 gTimeDelta = 0.0F;

	DebugRenderer* gDebugRenderer = nullptr;
	DefaultRenderer* gDefaultRenderer = nullptr;
}

static GLFWwindow* sGlfwContext = nullptr;

static R32 sTime = 0.0F;
static R32 sTimePrev = 0.0F;

static void GlfwDebugProc(I32 Error, char const* Msg)
{
	LOG("Error:%d Message:%s\n", Error, Msg);
}
static void GlfwMouseProc(GLFWwindow* Context, R64 X, R64 Y)
{
	Event::SetMouseX((R32)X);
	Event::SetMouseY((R32)Y);
}

static void GlDebugCallback(U32 Source, U32 Type, U32 Id, U32 Severity, I32 Length, char const* Msg, void const* UserParam)
{
	switch (Severity)
	{
		case GL_DEBUG_SEVERITY_NOTIFICATION: break;
		case GL_DEBUG_SEVERITY_LOW: LOG("Severity:Low Type:0x%x Message:%s\n", Type, Msg); break;
		case GL_DEBUG_SEVERITY_MEDIUM: LOG("Severity:Medium Type:0x%x Message:%s\n", Type, Msg); break;
		case GL_DEBUG_SEVERITY_HIGH: LOG("Severity:High Type:0x%x Message:%s\n", Type, Msg); break;
	}
}

static void ImGuiSetupIo()
{
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.ConfigWindowsMoveFromTitleBarOnly = true;
}
static void ImGuiSetupStyle()
{	
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 4.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 2.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(7.5f, 7.5f);
	style.FrameRounding = 2.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 13.0f;
	style.ScrollbarRounding = 12.0f;
	style.GrabMinSize = 7.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 0.0f;
	style.TabBorderSize = 1.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.2627451121807098f, 0.2627451121807098f, 0.2627451121807098f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2745098173618317f, 0.2745098173618317f, 0.2745098173618317f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.2980392277240753f, 0.2980392277240753f, 0.2980392277240753f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3882353007793427f, 0.3882353007793427f, 0.3882353007793427f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.1560000032186508f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.3910000026226044f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.2627451121807098f, 0.2627451121807098f, 0.2627451121807098f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.3882353007793427f, 0.3882353007793427f, 0.3882353007793427f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.09411764889955521f, 0.09411764889955521f, 0.09411764889955521f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.3490196168422699f, 0.3490196168422699f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.09411764889955521f, 0.09411764889955521f, 0.09411764889955521f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.5843137502670288f, 0.5843137502670288f, 0.5843137502670288f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.1560000032186508f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5860000252723694f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5860000252723694f);
}

I32 main(I32 Argc, char** Argv)
{
	CommandLine::Init(Argc, Argv);

	U32 width = 1920;
	U32 height = 1080;

	std::string dataDirStr = "";
	std::string widthStr = "";
	std::string heightStr = "";

	if (CommandLine::HasFirstArgument("Help"))
	{
		LOG("\n");
		LOG("Editor DataDir [Width|Height]\n");
		LOG("\n");

		return 0;
	}

	if (CommandLine::HasArgumentWithValue("DataDir", dataDirStr, "Data directory is missing"))
	{
		gDataDir = dataDirStr;
	}

	if (CommandLine::HasArgumentWithValue("Width", widthStr))
	{
		width = std::atoi(widthStr.c_str());
	}

	if (CommandLine::HasArgumentWithValue("Height", heightStr))
	{
		height = std::atoi(heightStr.c_str());
	}

	gBlowFish = new BlowFish{ CIPHER_KEY };

	glfwSetErrorCallback(GlfwDebugProc);

	if (glfwInit())
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_SAMPLES, 8);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);

		sGlfwContext = glfwCreateWindow((I32)width, (I32)height, "Editor", nullptr, nullptr);

		if (sGlfwContext)
		{
			glfwSetCursorPosCallback(sGlfwContext, GlfwMouseProc);
			glfwMakeContextCurrent(sGlfwContext);
			glfwSwapInterval(0);

			if (gladLoadGL())
			{
				glEnable(GL_DEBUG_OUTPUT);
				glDebugMessageCallback(GlDebugCallback, 0);

				IMGUI_CHECKVERSION();
				
				ImGuiContext* imGuiContext = ImGui::CreateContext();

				ImGuiSetupIo();
				ImGuiSetupStyle();

				ImGui_ImplGlfw_InitForOpenGL(sGlfwContext, 1);
				ImGui_ImplOpenGL3_Init("#version 450 core");

				InterfaceManager::Create();
				SceneManager::Create();

				gDebugRenderer = new DebugRenderer{ 65535, 65535 * 2 };
				gDefaultRenderer = new DefaultRenderer{};

				while (!glfwWindowShouldClose(sGlfwContext))
				{
					sTime = (R32)glfwGetTime();
					gTimeDelta = sTime - sTimePrev;
					sTimePrev = sTime;

					glClearColor(0.125F, 0.125F, 0.125F, 1.0F);
					glClear(GL_COLOR_BUFFER_BIT);

					ImGui_ImplGlfw_NewFrame();
					ImGui_ImplOpenGL3_NewFrame();
					
					ImGui::NewFrame();

					ImGuiViewport* viewport = ImGui::GetMainViewport();
					ImGuiID dockspaceId = ImGui::DockSpaceOverViewport(viewport, ImGuiDockNodeFlags_PassthruCentralNode);
				
					// TODO: This feature is still experimental or maybe I just don't know how to use it..

					/*
					if (dockspaceId) {
						ImGuiID dockMainId = dockspaceId;

						ImGui::DockBuilderRemoveNode(dockMainId);
						ImGui::DockBuilderAddNode(dockMainId, ImGuiDockNodeFlags_DockSpace);
						ImGui::DockBuilderSetNodeSize(dockMainId, viewport->Size);

						ImGuiID dockLeftId;
						ImGuiID dockRightId;
						ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Left, 0.2f, &dockLeftId, &dockRightId);
						ImGui::DockBuilderDockWindow("Left Pane", dockLeftId);
						ImGui::DockBuilderDockWindow("Right Pane", dockRightId);

						ImGui::DockBuilderFinish(dockMainId);
					}

					ImGui::Begin("Left Pane");
					ImGui::End();

					ImGui::Begin("Right Pane");
					ImGui::End();
					*/

					InterfaceManager::Render();

					SceneManager::PreUpdate();
					SceneManager::PreRender();

					SceneManager::Update();
					SceneManager::Render();

					SceneManager::PostUpdate();
					SceneManager::PostRender();

					ImGui::Render();
					
					ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
					
					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();

					glfwMakeContextCurrent(sGlfwContext);
					glfwSwapBuffers(sGlfwContext);

					Event::Poll(sGlfwContext);
				}

				delete gDebugRenderer;
				delete gDefaultRenderer;

				SceneManager::Destroy();
				InterfaceManager::Destroy();

				ImGui_ImplOpenGL3_Shutdown();
				ImGui_ImplGlfw_Shutdown();

				ImGui::DestroyContext(imGuiContext);
			}
			else
			{
				LOG("Failed initializing GL\n");
			}

			glfwDestroyWindow(sGlfwContext);
			glfwTerminate();
		}
		else
		{
			LOG("Failed creating window\n");
		}
	}
	else
	{
		LOG("Failed initializing GLFW\n");
	}

	return 0;
}
