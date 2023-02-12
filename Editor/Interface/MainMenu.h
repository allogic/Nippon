#pragma once

#include <string>

#include <Common/Types.h>

#include <Vendor/rapidjson/rapidjson.h>
#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace rj = rapidjson;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class MainMenu
  {
  public:

    void Draw();

  private:

    void DrawMapMenu();
    void DrawPackerMenu();

  private:

    using MenuItemProc = void(*)(const std::string&, const std::string&);

    static void MapItem(const std::string& Entry, const std::string& SubEntry);

    static void DecryptItem(const std::string& Entry, const std::string& SubEntry);
    static void EncryptItem(const std::string& Entry, const std::string& SubEntry);
    static void UnpackItem(const std::string& Entry, const std::string& SubEntry);
    static void RepackItem(const std::string& Entry, const std::string& SubEntry);

  private:

    void DrawMenuTree(const std::string& Name, rj::Value& Entry, MenuItemProc Procedure);
    void DoProcFor(rj::Value& Entry, MenuItemProc Procedure);
  };
}