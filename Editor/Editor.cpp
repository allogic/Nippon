#include <vector>
#include <filesystem>

#include <Common/Debug.h>
#include <Common/Types.h>

#include <Editor/Actor.h>
#include <Editor/Event.h>
#include <Editor/Scene.h>
#include <Editor/Window.h>
#include <Editor/UI.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>

#include <Editor/Renderer/DebugRenderer.h>

#include <Editor/UI/MainMenu.h>
#include <Editor/UI/FileInspector.h>
#include <Editor/UI/SceneOutline.h>

#include <Common/Utils/FileUtils.h>

#include <Vendor/GLAD/glad.h>

#include <Vendor/GLFW/glfw3.h>

#include <Vendor/ImGui/imgui.h>
#include <Vendor/ImGui/imgui_impl_glfw.h>
#include <Vendor/ImGui/imgui_impl_opengl3.h>

#include <Vendor/rapidjson/rapidjson.h>
#include <Vendor/rapidjson/document.h>

namespace fs = std::filesystem;

/*
 * General Stuff:
 * ==============
 *
 * Links:
 *  - https://okami.speedruns.wiki/Reverse_Engineering
 * 
 * File Coverage:
 *  - acf     1        4704
 *  - adx     3     4726664
 *  - afd   150       42272
 *  - afs2  240  1020865788
 *  - bin  1461  2532045488
 *  - chp     1        6480
 *  - cip     1          32
 *  - csv     1      418688
 *  - dat  2648  4547452104
 *  - dds   327  1164346120
 *  - idd   140   860071200
 *  - pac    16    72560640
 *  - ses   259    62250096
 *  - usm    75 18330692672
 *  - vsq    12        2192
 */

/*
 * Reverse Engineering:
 * ====================
 * 
 * Main Loop:
 *  - 0x1804b63b0 -> void sym.main.dll_bool___cdecl_flower_tick_void();
 *     - void fcn.1804b5cd0();
 *     - 0x1804b6240 -> void sym.main.dll_bool___cdecl_flower_startup_bool();
 * 
 * Resource Loading:
 *  - void fcn.1801ae440(int64_t arg1, const char *arg2, int64_t arg3, int64_t arg4, int64_t arg_550h);
 * 
 * Classes:
 *  - hx::StreamBuffer
 *  - hx::File
 *  - hx::FileDDS
 *  - hx::Texture
 */

/*
 * Hints:
 * ======
 * 
 * Encyption/Decyption:
 *  - Algorithm: Blowfish 256
 *  - Key: YaKiNiKuM2rrVrPJpGMkfe3EK4RbpbHw
 * 
 * x64 Parameter Passing:
 *  - floating-point                               stack XMM3 XMM2 XMM1 XMM0
 *  - integer                                      stack   R9   R8  RDX  RCX
 *  - Aggregates (8, 16, 32, or 64 bits) and __m64 stack   R9   R8  RDX  RCX
 *  - Other aggregates, as pointers                stack   R9   R8  RDX  RCX
 *  - __m128, as a pointer                         stack   R9   R8  RDX  RCX
 */

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

rapidjson::Document gConfig = {};
rapidjson::Document gWorld = {};

std::vector<ark::UI*> gUI = {};

ark::DebugRenderer* gDebugRenderer = nullptr;
ark::Scene* gScene = nullptr;

///////////////////////////////////////////////////////////
// Locals
///////////////////////////////////////////////////////////

static GLFWwindow* sGlfwContext = nullptr;

static ark::R32 sTime = 0.0F;
static ark::R32 sTimePrev = 0.0F;
static ark::R32 sTimeDelta = 0.0F;

///////////////////////////////////////////////////////////
// Glfw Callbacks
///////////////////////////////////////////////////////////

static void GlfwDebugProc(ark::I32 Error, char const* Msg)
{
  LOG("%s\n", Msg);
}

static void GlfwResizeProc(GLFWwindow* Context, ark::I32 Width, ark::I32 Height)
{
  ark::Window::SetWidth((ark::R32)Width);
  ark::Window::SetHeight((ark::R32)Height);
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
  gConfig.Parse(ark::FileUtils::ReadText("Config.json").c_str());
  gWorld.Parse(ark::FileUtils::ReadText("World.json").c_str());

  if (!fs::exists(gConfig["unpackDir"].GetString()))
  {
    LOG("Please specify valid unpackDir\n");
    return 0;
  }

  gUI.emplace_back(new ark::MainMenu);
  gUI.emplace_back(new ark::FileInspector);
  gUI.emplace_back(new ark::SceneOutline);

  glfwSetErrorCallback(GlfwDebugProc);

  if (glfwInit())
  {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    sGlfwContext = glfwCreateWindow((ark::I32)ark::Window::GetWidth(), (ark::I32)ark::Window::GetHeight(), "", nullptr, nullptr);

    if (sGlfwContext)
    {
      glfwSetWindowSizeCallback(sGlfwContext, GlfwResizeProc);
      glfwSetCursorPosCallback(sGlfwContext, GlfwMouseProc);
      glfwMakeContextCurrent(sGlfwContext);
      glfwSwapInterval(0);

      if (gladLoadGL())
      {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(GlDebugCallback, 0);

        gDebugRenderer = new ark::DebugRenderer{ 65535, 65535 * 2 };

        IMGUI_CHECKVERSION();
        ImGuiContext* imGuiContext{ ImGui::CreateContext() };

        ImGuiIO& imGuiIo{ ImGui::GetIO() };
        imGuiIo.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        imGuiIo.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        imGuiIo.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGuiStyle& imGuiStyle{ ImGui::GetStyle() };
        imGuiStyle.WindowRounding = 0.0f;
        imGuiStyle.FrameBorderSize = 0.0f;
        imGuiStyle.Colors[ImGuiCol_WindowBg].w = 1.0f;

        ImGui_ImplGlfw_InitForOpenGL(sGlfwContext, 1);
        ImGui_ImplOpenGL3_Init("#version 450 core");

        for (auto& ui : gUI)
        {
          ui->Update();
        }

        while (!glfwWindowShouldClose(sGlfwContext))
        {
          sTime = (ark::R32)glfwGetTime();
          sTimeDelta = sTime - sTimePrev;
          sTimePrev = sTime;

          glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
          glClear(GL_COLOR_BUFFER_BIT);
          glViewport(0, 0, (ark::I32)ark::Window::GetWidth(), (ark::I32)ark::Window::GetHeight());

          ImGui_ImplGlfw_NewFrame();
          ImGui_ImplOpenGL3_NewFrame();

          ImGui::NewFrame();
          ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

          if (gScene)
          {
            for (ark::Actor* actor : *gScene)
            {
              actor->Update(sTimeDelta);
            }
          }

          gDebugRenderer->Render();

          for (auto& ui : gUI)
          {
            ui->Draw();
          }

          ImGui::Render();

          ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

          ImGui::UpdatePlatformWindows();
          ImGui::RenderPlatformWindowsDefault();

          glfwMakeContextCurrent(sGlfwContext);
          glfwSwapBuffers(sGlfwContext);

          ark::Event::Poll(sGlfwContext);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext(imGuiContext);

        delete gDebugRenderer;
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

  for (auto& ui : gUI)
  {
    delete ui;
    ui = nullptr;
  }

  return 0;
}