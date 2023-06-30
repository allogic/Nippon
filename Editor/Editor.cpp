#include <string>
#include <vector>
#include <map>
#include <filesystem>

#include <Common/Debug.h>
#include <Common/Types.h>

#include <Common/Utils/FsUtils.h>

#include <Editor/Actor.h>
#include <Editor/Event.h>
#include <Editor/Interface.h>
#include <Editor/Scene.h>
#include <Editor/InterfaceManager.h>
#include <Editor/SceneManager.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>

#include <Vendor/GLAD/glad.h>

#include <Vendor/GLFW/glfw3.h>

#include <Vendor/ImGui/imgui.h>
#include <Vendor/ImGui/imgui_impl_glfw.h>
#include <Vendor/ImGui/imgui_impl_opengl3.h>

#include <Vendor/rapidjson/rapidjson.h>
#include <Vendor/rapidjson/document.h>

/*
 * Unextracted File Coverage:
 * ==========================
 * 
 * Ext  Count       Bytes
 * ----------------------
 * acf      1        4704
 * adx      3     4726664
 * afd    150       42272
 * afs2   240  1020865788
 * bin   1461  2532045488
 * chp      1        6480
 * cip      1          32
 * csv      1      418688
 * dat   2648  4547452104
 * dds    327  1164346120
 * idd    140   860071200
 * pac     16    72560640
 * ses    259    62250096
 * usm     75 18330692672
 * vsq     12        2192
 */

/*
 * Extension Infos:
 * ================
 * 
 * AFD - unknown purpose but are small and contain no multimedia.
 * AFS - if an AFS file is a subfile than it only contains audio, usually a few. Game Extractor can extract these but if you have try after you exact the big one (if some one can optimize the script then this should be displayed as a folder)
 * BIN - These are sub archives that Game Extractor can't really extract properly. The format is easy the first 4bytes are the number of files. then the next 4 bytes are all of the file offsets, then there are text extensions (4bytea for all of the files possible extensions include:
 * ANS - These are blank 16bytes strange
 * AKT - Contain Collision Maps and Terrain type Maps (ie is is dirt, water mud ect)
 * BMH - Contain mostly floats possibly level terrain related contain the header BMH
 * CAM - Unknown purpose contain the header mtb3 (in lowercase)
 * CCH - Contain mostly floats probably map related no real header
 * DRH - Related or identical to the SCH files, no real header mostly floats
 * EFF - are similar or identical to the BIN archives
 * FI2 - These contain object placement data
 * ITS - Contain no floats possibly map related no real header
 * LI3 - Contain some floats unknown purpose
 * MEH - Are small contain mostly floats have the header MEH
 * MOT - Unknown Contain the header mtb3 (in lowercase)
 * MSD - which is itself a sub-sub file archive. It contains no header and begins with the relative positions of the files it contains. Maybe it is collision map data, but it does not use floats
 * MSA - Contains no header other than the size of the whole file, no floats maybe texture meta data
 * MST - really small no header no file size no floats ect
 * RNI - These are small, Unknown purpose no real header
 * SCH - These contain mostly Floats probably related to map boundary lines-contain no real header
 * SCR - These contain the Header scr (in lowercase) these are model archives for level terrain that use 16bytesigned vertexes similar to MD archives that contain character meshes
 * SEQ - Probably animation data
 * T32 - These are some sort of Tim2 textures but DO NOT contain the text header TIM2
 * T3L - same as BIN archive
 * TM2 - These are regular Tim2 textures that DO contain the standard TIM2 text header
 * TRE - Unknown related to TSC contain no real header
 * TS  - Possibly related to camera movement these contain the header TS
 * TSC - Unknown releated to TSC contain no real header
 * TST - Same as the BIN archive
 * BMT - These are small files unknown purpose
 * D   - The ones on the demo are archives that only contain 1 of each of the following
 * MPD - contain Meta data for the textures no real header
 * T32 - see above
 * DAT - similar to BIN archives contain many files and models:
 * MD  - Contain the Header scr (in lowercase) + five 00 bytes+ the number of models (4bytes)+ four 00 bytes + The offsets to the models
 * MDB - These are the models packages and contain the submeshes+yet another type of Tim2 textures
 * DIR - a small text file with the name of the path to the video files
 * S   - some type of audio or video adx format??? this file might be the promational video for the full version of okami
 * SES - Audio
 * SFD - Audio
 * TBL - (map.tbl) This looks like a source code map, or at least ALL of the functions in the ELF file (PS2 Excutable file) this could easily be into hacks, but the file might not be included in the full version of the game
 * TM2 - (moji8.tm2) a Tim2 texture of the letters used in for menus and character speech.
 */

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;
namespace rj = rapidjson;

using namespace ark;

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

rj::Document gArchive = {};
rj::Document gConfig = {};

GLFWwindow* gGlfwContext = nullptr;

R32 gTimeDelta = 0.0F;

///////////////////////////////////////////////////////////
// Locals
///////////////////////////////////////////////////////////

static R32 sTime = 0.0F;
static R32 sTimePrev = 0.0F;

///////////////////////////////////////////////////////////
// Glfw Callbacks
///////////////////////////////////////////////////////////

static void GlfwDebugProc(I32 Error, char const* Msg)
{
	LOG("Error:%d Message:%s\n", Error, Msg);
}

static void GlfwMouseProc(GLFWwindow* Context, R64 X, R64 Y)
{
	Event::SetMouseX((R32)X);
	Event::SetMouseY((R32)Y);
}

///////////////////////////////////////////////////////////
// Gl Callbacks
///////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////
// ImGui Setup
///////////////////////////////////////////////////////////

static void ImGuiSetupIo()
{
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
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
	style.FramePadding = ImVec2(4.0f, 3.0f);
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

///////////////////////////////////////////////////////////
// Entry Point
///////////////////////////////////////////////////////////

I32 main()
{
	gArchive.Parse(FsUtils::ReadText("Archive.json").c_str());
	gConfig.Parse(FsUtils::ReadText("Config.json").c_str());

	U32 width = gConfig["editorWidth"].GetInt();
	U32 height = gConfig["editorHeight"].GetInt();

	glfwSetErrorCallback(GlfwDebugProc);

	if (glfwInit())
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_SAMPLES, 8);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);

		gGlfwContext = glfwCreateWindow(width, height, "Nippon", nullptr, nullptr);

		if (gGlfwContext)
		{
			glfwSetCursorPosCallback(gGlfwContext, GlfwMouseProc);
			glfwMakeContextCurrent(gGlfwContext);
			glfwSwapInterval(0);

			if (gladLoadGL())
			{
				glEnable(GL_DEBUG_OUTPUT);
				glDebugMessageCallback(GlDebugCallback, 0);

				IMGUI_CHECKVERSION();
				ImGuiContext* imGuiContext{ ImGui::CreateContext() };

				ImGuiSetupIo();
				ImGuiSetupStyle();

				ImGui_ImplGlfw_InitForOpenGL(gGlfwContext, 1);
				ImGui_ImplOpenGL3_Init("#version 450 core");

				InterfaceManager::Create();

				while (!glfwWindowShouldClose(gGlfwContext))
				{
					sTime = (R32)glfwGetTime();
					gTimeDelta = sTime - sTimePrev;
					sTimePrev = sTime;

					glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
					glClear(GL_COLOR_BUFFER_BIT);

					ImGui_ImplGlfw_NewFrame();
					ImGui_ImplOpenGL3_NewFrame();
					
					ImGui::NewFrame();
					ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
					
					InterfaceManager::Draw();
					SceneManager::Draw();

					ImGui::Render();
					
					ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
					
					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();

					glfwMakeContextCurrent(gGlfwContext);
					glfwSwapBuffers(gGlfwContext);

					Event::Poll(gGlfwContext);
				}

				ImGui_ImplOpenGL3_Shutdown();
				ImGui_ImplGlfw_Shutdown();

				ImGui::DestroyContext(imGuiContext);
			}
			else
			{
				LOG("Failed initializing GL\n");
			}

			glfwDestroyWindow(gGlfwContext);
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

	InterfaceManager::Destroy();

	return 0;
}