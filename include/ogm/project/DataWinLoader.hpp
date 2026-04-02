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
        uint32_t fps;
    } m_meta;

    struct ObjectDef {
        std::string name;
        uint32_t id;
        uint32_t sprite_index;
        bool visible;
        bool solid;
        int32_t depth;
        bool persistent;
        int32_t parent_id;
        uint32_t mask_index;
    };
    std::vector<ObjectDef> m_objects;

    struct InstanceDef {
        int32_t x;
        int32_t y;
        int32_t object_index;
        int32_t id;
        float scale_x;
        float scale_y;
        float image_speed;
        float image_index;
        uint32_t blend;
    };

    struct RoomDef {
        std::string name;
        uint32_t id;
        uint32_t width;
        uint32_t height;
        uint32_t fps;
        bool persistent;
        uint32_t background_color;
        bool draw_background_color;
        std::vector<InstanceDef> instances;
    };
    std::vector<RoomDef> m_rooms;

private:
    std::string m_path;

    std::string read_string_at(FILE* f, size_t offset);

    struct ChunkHeader {
        char id[4];
        uint32_t size;
    };
};

}}
