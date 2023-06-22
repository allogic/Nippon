#include <string>
#include <vector>
#include <filesystem>

#include <Common/Debug.h>
#include <Common/Types.h>

#include <Common/Utils/FsUtils.h>

#include <Editor/Actor.h>
#include <Editor/Event.h>
#include <Editor/Interface.h>
#include <Editor/Scene.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>

#include <Editor/Scenes/EntityScene.h>
#include <Editor/Scenes/LevelScene.h>

#include <Editor/Interface/MainMenu.h>
#include <Editor/Interface/Inspector.h>
#include <Editor/Interface/ModelBrowser.h>
#include <Editor/Interface/ObjectBrowser.h>
#include <Editor/Interface/TextureBrowser.h>
#include <Editor/Interface/EntityBrowser.h>
#include <Editor/Interface/Outline.h>
#include <Editor/Interface/Viewport.h>

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

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

rj::Document gArchive = {};
rj::Document gConfig = {};

GLFWwindow* gGlfwContext = nullptr;

std::vector<ark::LevelScene*> gLevelScenes = {};
std::vector<ark::EntityScene*> gEntityScenes = {};

ark::MainMenu* gMainMenu = nullptr;
ark::Inspector* gInspector = nullptr;
ark::ModelBrowser* gModelBrowser = nullptr;
ark::ObjectBrowser* gObjectBrowser = nullptr;
ark::TextureBrowser* gTextureBrowser = nullptr;
ark::EntityBrowser* gEntityBrowser = nullptr;
ark::Outline* gOutline = nullptr;

ark::R32 gTimeDelta = 0.0F;

///////////////////////////////////////////////////////////
// Locals
///////////////////////////////////////////////////////////

static ark::R32 sTime = 0.0F;
static ark::R32 sTimePrev = 0.0F;

static const ark::U32 sWidth = 1920;
static const ark::U32 sHeight = 1080;

///////////////////////////////////////////////////////////
// Glfw Callbacks
///////////////////////////////////////////////////////////

static void GlfwDebugProc(ark::I32 Error, char const* Msg)
{
  LOG("Error:%d Message:%s\n", Error, Msg);
}

static void GlfwResizeProc(GLFWwindow* Context, ark::I32 Width, ark::I32 Height)
{

}

static void GlfwMouseProc(GLFWwindow* Context, ark::R64 X, ark::R64 Y)
{
  ark::Event::SetMouseX((ark::R32)X);
  ark::Event::SetMouseY((ark::R32)Y);
}

///////////////////////////////////////////////////////////
// Gl Callbacks
///////////////////////////////////////////////////////////

static void GlDebugCallback(ark::U32 Source, ark::U32 Type, ark::U32 Id, ark::U32 Severity, ark::I32 Length, char const* Msg, void const* UserParam)
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
// Entry Point
///////////////////////////////////////////////////////////

ark::I32 main()
{
  gArchive.Parse(ark::FsUtils::ReadText("Archive.json").c_str());
  gConfig.Parse(ark::FsUtils::ReadText("Config.json").c_str());

  gMainMenu = new ark::MainMenu;
  gInspector = new ark::Inspector;
  gModelBrowser = new ark::ModelBrowser;
  gObjectBrowser = new ark::ObjectBrowser;
  gTextureBrowser = new ark::TextureBrowser;
  gEntityBrowser = new ark::EntityBrowser;
  gOutline = new ark::Outline;

  glfwSetErrorCallback(GlfwDebugProc);

  if (glfwInit())
  {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);

    gGlfwContext = glfwCreateWindow((ark::I32)sWidth, (ark::I32)sHeight, "Nippon", nullptr, nullptr);

    if (gGlfwContext)
    {
      glfwSetWindowSizeCallback(gGlfwContext, GlfwResizeProc);
      glfwSetCursorPosCallback(gGlfwContext, GlfwMouseProc);
      glfwMakeContextCurrent(gGlfwContext);
      glfwSwapInterval(0);

      if (gladLoadGL())
      {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(GlDebugCallback, 0);

        IMGUI_CHECKVERSION();
        ImGuiContext* imGuiContext{ ImGui::CreateContext() };

        ImGuiIO& imGuiIo{ ImGui::GetIO() };
        imGuiIo.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        imGuiIo.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        imGuiIo.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        imGuiIo.ConfigWindowsMoveFromTitleBarOnly = true;

        ImGuiStyle& imGuiStyle{ ImGui::GetStyle() };
        imGuiStyle.WindowRounding = 0.0F;
        imGuiStyle.FrameBorderSize = 0.0F;
        imGuiStyle.Colors[ImGuiCol_WindowBg].w = 1.0F;

        ImGui_ImplGlfw_InitForOpenGL(gGlfwContext, 1);
        ImGui_ImplOpenGL3_Init("#version 450 core");

        while (!glfwWindowShouldClose(gGlfwContext))
        {
          sTime = (ark::R32)glfwGetTime();
          gTimeDelta = sTime - sTimePrev;
          sTimePrev = sTime;

          glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
          glClear(GL_COLOR_BUFFER_BIT);

          ImGui_ImplGlfw_NewFrame();
          ImGui_ImplOpenGL3_NewFrame();
          
          ImGui::NewFrame();
          ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
          
          gMainMenu->Draw();
          gInspector->Draw();
          gModelBrowser->Draw();
          gObjectBrowser->Draw();
          gTextureBrowser->Draw();
          gEntityBrowser->Draw();
          gOutline->Draw();
          
          for (const auto& scene : gLevelScenes)
          {
            scene->GetViewport()->Draw();
          }

          for (const auto& scene : gEntityScenes)
          {
            scene->GetViewport()->Draw();
          }

          ImGui::Render();
          
          ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
          
          ImGui::UpdatePlatformWindows();
          ImGui::RenderPlatformWindowsDefault();

          glfwMakeContextCurrent(gGlfwContext);
          glfwSwapBuffers(gGlfwContext);

          ark::Event::Poll(gGlfwContext);
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

  delete gMainMenu;
  delete gInspector;
  delete gModelBrowser;
  delete gObjectBrowser;
  delete gTextureBrowser;
  delete gEntityBrowser;
  delete gOutline;

  return 0;
}