#pragma once

#include <Common/Types.h>

#include <Editor/Forward.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class InterfaceManager
  {
  public:

    static EntityBrowser* GetEntityBrowser();
    static Inspector* GetInspector();
    static MainMenu* GetMainMenu();
    static ModelBrowser* GetModelBrowser();
    static ObjectBrowser* GetObjectBrowser();
    static Outline* GetOutline();
    static TextureBrowser* GetTextureBrowser();

  public:

    static void Create();
    static void Draw();
    static void Destroy();
  };
}