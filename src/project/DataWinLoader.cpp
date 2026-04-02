#include "ogm/project/DataWinLoader.hpp"
#include "ogm/common/Trace.hpp"
#include <cstring>
#include <iostream>
#include <cstdio>

#ifdef __3DS__
#include <3ds.h>
#endif

namespace ogm { namespace project {

DataWinLoader::DataWinLoader(const char* path)
    : m_path(path)
{ }

std::string DataWinLoader::read_string_at(FILE* f, size_t offset) {
    long cur_pos = ftell(f);

    // In GM, the offset points to the first character of the string.
    // The 32-bit length is located 4 bytes BEFORE the characters.
    fseek(f, offset - 4, SEEK_SET);

    uint32_t str_len;
    fread(&str_len, 4, 1, f);

    // Safety check: cap string length to prevent bad_alloc on corrupt data
    if (str_len > 1024 * 1024) {
        str_len = 1024 * 1024;
    }

    std::string s(str_len, '\0');
    if (str_len > 0) {
        fread(&s[0], 1, str_len, f);
    }

    fseek(f, cur_pos, SEEK_SET);
    return s;
}

bool DataWinLoader::load() {
    TRACE();
    FILE* file = fopen(m_path.c_str(), "rb");

    #ifdef __3DS__
    if (!file) {
        SD_PRINT("CRITICAL: fopen failed to open data.win!");
        return false;
    }
    #else
    if (!file) return false;
    #endif

    try {
        char form_id[4];
        fread(form_id, 1, 4, file);

        if (std::strncmp(form_id, "FORM", 4) != 0) {
            SD_PRINT("DataWinLoader: Not a valid IFF FORM file.");
            fclose(file);
            return false;
        }

        uint32_t form_size;
        fread(&form_size, 4, 1, file);

        while (true) {
            char chunk_id[4];
            if (fread(chunk_id, 1, 4, file) != 4) break;

            uint32_t chunk_size;
            fread(&chunk_size, 4, 1, file);

            std::string chunk_name(chunk_id, 4);
            SD_PRINT("CHUNK: " + chunk_name + " (Size: " + std::to_string(chunk_size) + ")");

            long next_chunk_pos = ftell(file) + chunk_size;

            if (chunk_name == "GEN8") {
                long chunk_data_start = ftell(file);
                fseek(file, chunk_data_start + 0x28, SEEK_SET);

                uint32_t name_offset;
                fread(&name_offset, 4, 1, file);
                m_meta.game_name = read_string_at(file, name_offset);
                SD_PRINT("  -> Game Name: " + m_meta.game_name);

                fseek(file, chunk_data_start + 0x3C, SEEK_SET);
                fread(&m_meta.window_width, 4, 1, file);
                fread(&m_meta.window_height, 4, 1, file);
                SD_PRINT("  -> Window: " + std::to_string(m_meta.window_width) + "x" + std::to_string(m_meta.window_height));

                fseek(file, chunk_data_start + 0x70, SEEK_SET);
                fread(&m_meta.fps, 4, 1, file);
                SD_PRINT("  -> FPS: " + std::to_string(m_meta.fps));
            }
            else if (chunk_name == "STRG") {
                uint32_t string_count;
                fread(&string_count, 4, 1, file);
                SD_PRINT("  -> String Count: " + std::to_string(string_count));

                std::vector<uint32_t> offsets(string_count);
                fread(offsets.data(), 4, string_count, file);

                m_strings.reserve(string_count);
                for (uint32_t i = 0; i < string_count; ++i) {
                    uint32_t str_len;
                    fseek(file, offsets[i], SEEK_SET);
                    fread(&str_len, 4, 1, file);

                    if (str_len < 1024 * 1024) {
                        std::string s(str_len, '\0');
                        if (str_len > 0) fread(&s[0], 1, str_len, file);
                        m_strings.push_back(s);
                    } else {
                        m_strings.push_back("<too_long>");
                    }
                }
            }
            else if (chunk_name == "VARI") {
                uint32_t var_count;
                fread(&var_count, 4, 1, file);
                fseek(file, 8, SEEK_CUR); // Skip 8-byte studio header
                
                SD_PRINT("  -> Variable Count: " + std::to_string(var_count));
                m_variables.reserve(var_count);
                for (uint32_t i = 0; i < var_count; ++i) {
                    uint32_t name_offset, scope, id, count, address;
                    fread(&name_offset, 4, 1, file);
                    fread(&scope, 4, 1, file);
                    fread(&id, 4, 1, file);
                    fread(&count, 4, 1, file);
                    fread(&address, 4, 1, file);

                    m_variables.push_back({read_string_at(file, name_offset), id});
                }
            }
            else if (chunk_name == "OBJT") {
                uint32_t obj_count;
                fread(&obj_count, 4, 1, file);
                SD_PRINT("  -> Object Count: " + std::to_string(obj_count));

                std::vector<uint32_t> offsets(obj_count);
                if (obj_count > 0) {
                    fread(offsets.data(), 4, obj_count, file);
                }

                m_objects.reserve(obj_count);
                for (uint32_t i = 0; i < obj_count; ++i) {
                    ObjectDef obj;
                    obj.id = i;

                    fseek(file, offsets[i], SEEK_SET);

                    uint32_t name_offset;
                    fread(&name_offset, 4, 1, file);
                    obj.name = read_string_at(file, name_offset);

                    fread(&obj.sprite_index, 4, 1, file);
                    uint32_t vis;
                    fread(&vis, 4, 1, file);
                    obj.visible = (vis != 0);

                    uint32_t solid;
                    fread(&solid, 4, 1, file);
                    obj.solid = (solid != 0);

                    fread(&obj.depth, 4, 1, file);

                    uint32_t pers;
                    fread(&pers, 4, 1, file);
                    obj.persistent = (pers != 0);

                    fread(&obj.parent_id, 4, 1, file);
                    fread(&obj.mask_index, 4, 1, file);

                    m_objects.push_back(std::move(obj));
                }
            }
            else if (chunk_name == "ROOM") {
                uint32_t room_count;
                fread(&room_count, 4, 1, file);
                SD_PRINT("  -> Room Count: " + std::to_string(room_count));

                std::vector<uint32_t> offsets(room_count);
                if (room_count > 0) {
                    fread(offsets.data(), 4, room_count, file);
                }

                m_rooms.reserve(room_count);
                for (uint32_t i = 0; i < room_count; ++i) {
                    RoomDef room;
                    room.id = i;

                    fseek(file, offsets[i], SEEK_SET);

                    uint32_t name_offset;
                    fread(&name_offset, 4, 1, file);
                    room.name = read_string_at(file, name_offset);

                    fseek(file, offsets[i] + 0x08, SEEK_SET);
                    fread(&room.width, 4, 1, file);
                    fread(&room.height, 4, 1, file);
                    fread(&room.fps, 4, 1, file);

                    uint32_t pers;
                    fread(&pers, 4, 1, file);
                    room.persistent = (pers != 0);

                    fread(&room.background_color, 4, 1, file);

                    uint32_t draw_bg;
                    fread(&draw_bg, 4, 1, file);
                    room.draw_background_color = (draw_bg != 0);

                    fseek(file, offsets[i] + 0x2C, SEEK_SET);
                    uint32_t instances_list_offset;
                    fread(&instances_list_offset, 4, 1, file);

                    if (instances_list_offset > 0) {
                        fseek(file, instances_list_offset, SEEK_SET);
                        uint32_t instance_count;
                        fread(&instance_count, 4, 1, file);

                        std::vector<uint32_t> inst_offsets(instance_count);
                        if (instance_count > 0) {
                            fread(inst_offsets.data(), 4, instance_count, file);
                        }

                        room.instances.reserve(instance_count);
                        for (uint32_t j = 0; j < instance_count; ++j) {
                            InstanceDef inst;
                            fseek(file, inst_offsets[j], SEEK_SET);

                            fread(&inst.x, 4, 1, file);
                            fread(&inst.y, 4, 1, file);
                            fread(&inst.object_index, 4, 1, file);
                            fread(&inst.id, 4, 1, file);

                            fseek(file, inst_offsets[j] + 0x14, SEEK_SET);
                            fread(&inst.scale_x, 4, 1, file);
                            fread(&inst.scale_y, 4, 1, file);
                            fread(&inst.image_speed, 4, 1, file);
                            fread(&inst.image_index, 4, 1, file);
                            fread(&inst.blend, 4, 1, file);

                            room.instances.push_back(inst);
                        }
                    }

                    m_rooms.push_back(std::move(room));
                }
            }
            else if (chunk_name == "FUNC") {
                uint32_t func_count;
                fread(&func_count, 4, 1, file);
                
                SD_PRINT("  -> Function Count: " + std::to_string(func_count));
                m_functions.reserve(func_count);
                for (uint32_t i = 0; i < func_count; ++i) {
                    uint32_t name_offset, id, count;
                    fread(&name_offset, 4, 1, file);
                    fread(&id, 4, 1, file);
                    fread(&count, 4, 1, file);

                    m_functions.push_back({read_string_at(file, name_offset), id});
                }
            }
            else if (chunk_name == "TXTR") {
                uint32_t texture_count;
                fread(&texture_count, 4, 1, file);
                SD_PRINT("  -> Texture Count: " + std::to_string(texture_count));
                std::vector<uint32_t> offsets(texture_count);
                fread(offsets.data(), 4, texture_count, file);

                m_images.reserve(texture_count);
                for (uint32_t i = 0; i < texture_count; ++i) {
                    if (offsets[i] >= form_size || offsets[i] < 8) {
                        SD_PRINT("    -> ERROR: Texture offset " + std::to_string(offsets[i]) + " is invalid (File size: " + std::to_string(form_size) + ")");
                        continue;
                    }
                    fseek(file, offsets[i] + 4, SEEK_SET); // Skip 4-byte dummy
                    uint32_t png_offset;
                    fread(&png_offset, 4, 1, file);

                    uint32_t next_png_offset = 0;
                    if (i < texture_count - 1) {
                        long cur = ftell(file);
                        fseek(file, offsets[i + 1] + 4, SEEK_SET);
                        fread(&next_png_offset, 4, 1, file);
                        fseek(file, cur, SEEK_SET);
                    } else {
                        long cur = ftell(file);
                        fseek(file, 0, SEEK_END);
                        next_png_offset = ftell(file);
                        fseek(file, cur, SEEK_SET);
                    }

                    uint32_t blob_size = next_png_offset - png_offset;
                    
                    if (i < 5 || i == texture_count - 1) {
                        SD_PRINT("    -> Tex[" + std::to_string(i) + "] Offset=" + std::to_string(png_offset) + " Size=" + std::to_string(blob_size));
                    }

                    if (blob_size > 0 && blob_size < 10 * 1024 * 1024) { // 10MB safety cap
                        std::vector<uint8_t> buffer(blob_size);
                        fseek(file, png_offset, SEEK_SET);
                        fread(buffer.data(), 1, blob_size, file);
                        ogm::asset::Image img;
                        img.load_from_memory(buffer.data(), blob_size);
                        m_images.push_back(std::move(img));
                    } else if (blob_size > 0) {
                        SD_PRINT("    -> WARNING: Skipping oversized texture (" + std::to_string(blob_size) + " bytes)");
                    }
                }
            }

            fseek(file, next_chunk_pos, SEEK_SET);
        }
    } catch (...) {
        SD_PRINT("CRASH DETECTED IN DataWinLoader!");
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

}}
