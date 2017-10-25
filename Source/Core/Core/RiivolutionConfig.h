#pragma once

#include <vector>
#include <map>

namespace rii
{

  enum patch_part_type : uint8_t {
    none, savegame, file, folder, memory
  };

  typedef struct GamePatchPart {
    patch_part_type type : 3;
    bool recursive : 1;
    bool create : 1;
    bool clone : 1;
    bool resize : 1;
    bool search : 1;
    const char* external_location;
    const char* disc_location;
    int align;
    uint32_t length;
    uint32_t offset;
    std::string valuefile;
    std::vector<uint8_t> value;
    std::vector<uint8_t> original;

    GamePatchPart() {
      type = patch_part_type::none;
      external_location = "";
      disc_location = "";
      recursive = false;
      create = false;
      clone = false;
      resize = false;
      search = false;
      length = 0;
      offset = 0;
      align = 0;
    }
  } GamePatchPart;
  typedef struct GamePatch {
    const char* id;
    const char* root;
    std::vector<GamePatchPart> parts;
  } GamePatch;

  typedef struct Choice {
    const char* name;
    std::vector<const char*> patches;
    Choice() {
      name = "";
    }
  } Choice;
  typedef struct Option {
    const char* name;
    const char* id;
    int default_choice;
    std::vector<Choice> choices;
    Option() {
      default_choice = 0;
      id = "";
      name = "";
    }
  } Option;
  typedef struct Section {
    const char* name;
    std::vector<Option> options;
    Section() {
      name = "";
    }
  } Section;

  class RiivolutionConfig
  {
  private:
    const wchar_t *GetWC(const char *c)
    {
      const size_t cSize = strlen(c) + 1;
      wchar_t* wc = new wchar_t[cSize];
      mbstowcs(wc, c, cSize);

      return wc;
    }

    const char* wiidisc_version;
    const char* root_path;
    bool shiftfiles : 1;
    bool logevents : 1;

    const char* game_id;
    const char* developer_id;
    const char* game_version;
    const char* disc_number;

    std::vector<char> regions;
    std::map<std::string, GamePatch> patches;
    std::vector<Section> sections;

  public:
    RiivolutionConfig(const char* filename) {
      RiivolutionConfig(GetWC(filename));
    }
    RiivolutionConfig(const wchar_t* filename);
    ~RiivolutionConfig();
  };

}
