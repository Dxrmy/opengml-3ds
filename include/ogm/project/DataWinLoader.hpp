#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstdint>

#include "ogm/asset/Image.hpp"

namespace ogm { namespace project {

class DataWinLoader {
public:
    DataWinLoader(const char* path);

    // Parses the IFF structure and prints chunk IDs
    bool load();

    std::vector<std::string> m_strings;
    std::vector<ogm::asset::Image> m_images;

    struct VariableDef {
        std::string name;
        uint32_t id;
    };
    std::vector<VariableDef> m_variables;

    struct FunctionDef {
        std::string name;
        uint32_t id;
    };
    std::vector<FunctionDef> m_functions;

    struct GameMetadata {
        std::string game_name;
        uint32_t window_width;
        uint32_t window_height;
    } m_meta;

private:
    std::string m_path;

    std::string read_string_at(FILE* f, size_t offset);

    struct ChunkHeader {
        char id[4];
        uint32_t size;
    };
};

}}
