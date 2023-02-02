#pragma once

#include <string>
#include <map>

#include <Common/Types.h>

#include <Editor/Interface.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace rj = rapidjson;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class MainMenu : public Interface
  {
  public:

    virtual void Draw() override;

  private:

    void SwitchScene(const std::string& EntryDir, const std::string& SubEntryDir, const std::string& EntryType);
  };
}