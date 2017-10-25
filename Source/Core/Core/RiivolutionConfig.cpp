#include "Core/RiivolutionConfig.h"
#include <iostream>
#include <pugixml.hpp>
#include <unordered_map>
#include <algorithm>

namespace rii {

  std::vector<std::string> Split(const std::string& str, int splitLength)
  {
    int NumSubstrings = static_cast<int>(str.length()) / splitLength;
    std::vector<std::string> ret;

    for (auto i = 0; i < NumSubstrings; i++)
    {
      ret.push_back(str.substr(i * splitLength, splitLength));
    }

    // If there are leftover characters, create a shorter item at the end.
    if (str.length() % splitLength != 0)
    {
      ret.push_back(str.substr(splitLength * NumSubstrings));
    }

    return ret;
  }

  static std::unordered_map<std::string, patch_part_type> stringToType =
    std::unordered_map<std::string, patch_part_type>({
      { "savegame",patch_part_type::savegame },
      { "memory",patch_part_type::memory },
      { "folder",patch_part_type::folder },
      { "file",patch_part_type::file }
    });

  void loadPatches(pugi::xpath_node_set& xpatches, std::map<std::string, GamePatch>& patches)
  {
    for (auto xpatch : xpatches)
    {
      GamePatch patch;
      patch.id = xpatch.node().attribute("id").as_string();
      pugi::xml_attribute root_attr;
      if (root_attr = xpatch.node().attribute("root"))
      {
        patch.root = root_attr.as_string();
      }
      else
      {
        patch.root = "/";
      }

      for (auto xpart : xpatch.node().select_nodes("savegame|folder|memory|file"))
      {
        GamePatchPart part;
        auto node = xpart.node();
        part.type = stringToType[node.name()];
        //std::cout << node.name() << std::endl;

        switch (part.type)
        {
          case patch_part_type::savegame: {
            part.external_location = node.attribute("external").as_string();
            part.clone = false;

            pugi::xml_attribute a_clone;
            if (a_clone = node.attribute("clone"))
              part.clone = a_clone.as_bool();
          } break;

          case patch_part_type::folder: {
            part.external_location = node.attribute("external").as_string();

            if (auto attr = node.attribute("disc"))
              part.disc_location = attr.as_string();

            part.recursive = false;
            pugi::xml_attribute a_recurs;
            if (a_recurs = node.attribute("recursive"))
              part.recursive = a_recurs.as_bool();

            part.create = false;
            pugi::xml_attribute a_create;
            if (a_create = node.attribute("create"))
              part.create = a_create.as_bool();

            part.resize = true;
            if (auto attr = node.attribute("resize"))
              part.resize = attr.as_bool();

            part.length = -1;
            if (auto attr = node.attribute("length"))
              part.length = std::stoul(attr.as_string(), nullptr, 16);
          } break;

          case patch_part_type::memory: { // No Ocarina codes yet :(

            if (auto attr = node.attribute("search"))
              part.search = attr.as_bool();

            if (!part.search)
              part.offset = std::stoul(std::string(node.attribute("offset").as_string()), nullptr, 16);
            else
            {
              part.align = 1;
              if (auto attr = node.attribute("align"))
                part.align = attr.as_int();
            }

            if (auto attr = node.attribute("value"))
            {
              std::string s_value = std::string(attr.as_string());

              auto s_bytes = Split(s_value, 2);
              for (auto byt_seq : s_bytes)
              {
                uint8_t byte = 0;
                if (byt_seq.length() < 2) continue;

                byte = std::stoul("0x" + byt_seq, nullptr, 16);

                part.value.push_back(byte);
              }
            }
            else
            {
              auto valfile = node.attribute("valuefile");
              part.valuefile = valfile.as_string();
            }

            if (auto attr = node.attribute("original"))
            {
              std::string s_value = std::string(attr.as_string());

              auto s_bytes = Split(s_value, 2);
              for (auto byt_seq : s_bytes)
              {
                uint8_t byte = 0;
                if (byt_seq.length() < 2) continue;

                byte = std::stoul("0x" + byt_seq, nullptr, 16);

                part.original.push_back(byte);
              }
            }


          } break;

          case patch_part_type::file: {
            part.external_location = node.attribute("external").as_string();

            if (auto attr = node.attribute("disc"))
              part.disc_location = attr.as_string();

            part.create = false;
            pugi::xml_attribute a_create;
            if (a_create = node.attribute("create"))
              part.create = a_create.as_bool();

            part.resize = true;
            if (auto attr = node.attribute("resize"))
              part.resize = attr.as_bool();

            part.length = -1;
            if (auto attr = node.attribute("length"))
              part.length = std::stoul(attr.as_string(), nullptr, 16);

            part.offset = 0;
            if (auto attr = node.attribute("offset"))
              part.offset = std::stoul(attr.as_string(), nullptr, 16);

          } break;
        }

        patch.parts.push_back(part);
      }

      patches[patch.id] = patch;
      //patches.push_back(patch);
    }
  }

  void loadOptions(pugi::xml_node& options, std::vector<Section>& sections)
  {
    for (auto xsection : options.select_nodes("section[@name]"))
    {
      Section section;
      section.name = xsection.node().attribute("name").as_string();

      for (auto xoption : xsection.node().select_nodes("option[@name]"))
      {
        Option option;
        option.name = xoption.node().attribute("name").as_string();
        if (auto attr = xoption.node().attribute("id"))
          option.id = attr.as_string();
        if (auto attr = xoption.node().attribute("default"))
          option.default_choice = attr.as_int();

        for (auto xchoice : xoption.node().select_nodes("choice[@name]"))
        {
          Choice choice;
          choice.name = xchoice.node().attribute("name").as_string();

          for (auto xpatch : xchoice.node().select_nodes("patch[@id]"))
            choice.patches.push_back(xpatch.node().attribute("id").as_string());

          option.choices.push_back(choice);
        }

        section.options.push_back(option);
      }

      sections.push_back(section);
    }
  }

  RiivolutionConfig::RiivolutionConfig(const wchar_t* filename)
  {
    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file(filename);

    auto node_wiidisc = doc.select_node("/wiidisc[@version]").node();
    auto node_id = doc.select_node("/wiidisc[@version]/id").node();
    auto xpatches = doc.select_nodes("/wiidisc[@version]/patch[@id]");
    auto node_options = doc.select_node("/wiidisc[@version]/options").node();

    /* START wiidisc ATTRIBUTES */
    wiidisc_version = node_wiidisc.attribute("version").as_string();

    shiftfiles = false;
    if (auto attr = node_wiidisc.attribute("shiftfiles"))
      shiftfiles = attr.as_bool();

    logevents = false;
    if (auto attr = node_wiidisc.attribute("log"))
      logevents = attr.as_bool();

    root_path = "/";
    if (auto attr = node_wiidisc.attribute("root"))
      root_path = attr.as_string();
    /* END wiidisc ATTRIBUTES */

    /* START node_id ATTRIBUTES */
    game_id = "";
    if (auto attr = node_id.attribute("game"))
      game_id = attr.as_string();

    developer_id = "";
    if (auto attr = node_id.attribute("developer"))
      developer_id = attr.as_string();

    disc_number = "";
    if (auto attr = node_id.attribute("disc"))
      disc_number = attr.as_string();

    game_version = "";
    if (auto attr = node_id.attribute("version"))
      game_version = attr.as_string();

    for (auto xreg : node_id.select_nodes("region"))
      regions.push_back(xreg.node().attribute("type").as_string()[0]);
    /* END node_id ATTRIBUTES */

    std::cout << "Ver:" << wiidisc_version << std::endl;

    loadPatches(xpatches, patches);
    loadOptions(node_options, sections);

  }

  RiivolutionConfig::~RiivolutionConfig()
  {
  }

}
