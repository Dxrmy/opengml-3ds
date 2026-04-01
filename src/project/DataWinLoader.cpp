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
    fread(&s[0], 1, str_len, f);

    fseek(f, cur_pos, SEEK_SET);
    return s;
}

bool DataWinLoader::load() {
    TRACE();
    std::cout << "STEP 1: OPENING FILE" << std::endl;
    std::cout << "DataWinLoader: Opening " << m_path << "..." << std::endl;

    FILE* file = fopen(m_path.c_str(), "rb");

    #ifdef __3DS__
    if (!file) {
        std::cout << "CRITICAL: fopen failed to open file!" << std::endl;
        std::cout << "Press START to exit." << std::endl;
        while (aptMainLoop()) {
            gspWaitForVBlank();
            gfxSwapBuffers();
            hidScanInput();
            if (hidKeysDown() & KEY_START) break;
        }
        return false;
    }

    std::cout << "File opened. Press START to begin parsing." << std::endl;
    while (aptMainLoop()) {
        gspWaitForVBlank();
        gfxSwapBuffers();
        hidScanInput();
        if (hidKeysDown() & KEY_START) break;
    }
    #else
    if (!file) return false;
    #endif

    try {
        TRACE();
        std::cout << "DataWinLoader: Reading FORM header..." << std::endl;
        char form_id[4];
        TRACE();
        size_t read_bytes = fread(form_id, 1, 4, file);
        TRACE();
        if (read_bytes != 4) {
            std::cerr << "DataWinLoader: Failed to read 4 bytes for header." << std::endl;
            fclose(file);
            return false;
        }

        if (std::strncmp(form_id, "FORM", 4) != 0) {
            std::cerr << "DataWinLoader: Not a valid IFF FORM file." << std::endl;
            printf("Found: %02x %02x %02x %02x\n", (unsigned char)form_id[0], (unsigned char)form_id[1], (unsigned char)form_id[2], (unsigned char)form_id[3]);
            #ifdef __3DS__
            std::cout << "Press START to exit." << std::endl;
            while (aptMainLoop()) {
                gspWaitForVBlank();
                gfxSwapBuffers();
                hidScanInput();
                if (hidKeysDown() & KEY_START) break;
            }
            #endif
            fclose(file);
            return false;
        }

        std::cout << "STEP 2: FORM FOUND" << std::endl;

        uint32_t form_size;
        TRACE();
        fread(&form_size, 4, 1, file);
        TRACE();
        std::cout << "DataWinLoader: Found FORM chunk, size indicator: " << form_size << std::endl;

        std::cout << "DataWinLoader: Entering chunk loop..." << std::endl;
        while (true) {
            char chunk_id[4];
            read_bytes = fread(chunk_id, 1, 4, file);
            if (read_bytes != 4) {
                if (feof(file)) {
                    SD_PRINT("DataWinLoader: Reached end of file.");
                } else {
                    SD_PRINT("DataWinLoader: Error reading chunk ID.");
                }
                break;
            }

            uint32_t chunk_size;
            fread(&chunk_size, 4, 1, file);

            std::string chunk_name(chunk_id, 4);
            std::string log_msg = "CHUNK: " + chunk_name + " (Size: " + std::to_string(chunk_size) + ")";
            SD_PRINT(log_msg);

            // Save position to safely jump to the next chunk after parsing
            long next_chunk_pos = ftell(file) + chunk_size;

            if (chunk_name == "GEN8") {
                long chunk_data_start = next_chunk_pos - chunk_size;
                fseek(file, chunk_data_start + 0x28, SEEK_SET);

                uint32_t name_offset;
                fread(&name_offset, 4, 1, file);

                if (name_offset > 0 && name_offset < 100 * 1024 * 1024) {
                    m_meta.game_name = read_string_at(file, name_offset);
                    SD_PRINT("  -> Game Name: " + m_meta.game_name);
                }

                fseek(file, chunk_data_start + 0x3C, SEEK_SET);
                fread(&m_meta.window_width, 4, 1, file);
                fread(&m_meta.window_height, 4, 1, file);

                SD_PRINT("  -> Window: " + std::to_string(m_meta.window_width) + "x" + std::to_string(m_meta.window_height));
            }
            else if (chunk_name == "STRG") {
                uint32_t string_count;
                fread(&string_count, 4, 1, file);
                SD_PRINT("  -> String Count: " + std::to_string(string_count));

                std::vector<uint32_t> offsets(string_count);
                fread(offsets.data(), 4, string_count, file);

                m_strings.reserve(string_count);

                for (uint32_t i = 0; i < string_count; ++i) {
                    fseek(file, offsets[i], SEEK_SET);
                    uint32_t str_len;
                    fread(&str_len, 4, 1, file);

                    if (str_len < 1024 * 1024) { // Safety cap
                        std::string s(str_len, '\0');
                        if (str_len > 0) {
                            fread(&s[0], 1, str_len, file);
                        }
                        m_strings.push_back(s);

                        // Log periodically for sanity
                        if (i % 10000 == 0 || i == string_count - 1) {
                            SD_PRINT("  -> Loaded String[" + std::to_string(i) + "] = " + s);
                        }
                    } else {
                        m_strings.push_back("<error: string too long>");
                    }
                }
            }
            else if (chunk_name == "CODE") {
                uint32_t code_count;
                fread(&code_count, 4, 1, file);
                SD_PRINT("  -> Code Block Count: " + std::to_string(code_count));

                std::vector<uint32_t> offsets(code_count);
                fread(offsets.data(), 4, code_count, file);

                for (uint32_t i = 0; i < code_count; ++i) {
                    fseek(file, offsets[i], SEEK_SET);

                    uint32_t name_offset, length, locals_count;

                    fread(&name_offset, 4, 1, file);
                    fread(&length, 4, 1, file);
                    fread(&locals_count, 4, 1, file);

                    // Skip arg_count (2), flags (1), unused (1) = 4 bytes
                    fseek(file, 4, SEEK_CUR);

                    uint32_t bytecode_offset;
                    fread(&bytecode_offset, 4, 1, file);

                    // Bytecode start is determined by the absolute offset pointer
                    long code_start = bytecode_offset;

                    // Simple logging for first few blocks to show Proof of Concept
                    if (i < 3) {
                        std::string bname = read_string_at(file, name_offset);
                        SD_PRINT("  -> Parsing CodeBlock[" + std::to_string(i) + "]: " + bname + " (Len: " + std::to_string(length) + ")");

                        fseek(file, code_start, SEEK_SET);
                        for (uint32_t j = 0; j < length / 4; j++) {
                            uint32_t inst;
                            fread(&inst, 4, 1, file);

                            uint8_t opcode = (inst >> 24) & 0xFF;

                            // 0x41: push
                            // 0x42: pop
                            // 0x89: call
                            // 0x8A: ret

                            // Or GM1.4 common formats
                            // 192 (0xC0) = pushi
                            // 153 (0x99) = call

                            if (opcode == 0xC0 || opcode == 0x41 || opcode == 0x84 || opcode == 129) SD_PRINT("    [" + std::to_string(j) + "] PUSH");
                            else if (opcode == 0x42 || opcode == 0x85 || opcode == 130) SD_PRINT("    [" + std::to_string(j) + "] POP");
                            else if (opcode == 0x99 || opcode == 0x89 || opcode == 153) SD_PRINT("    [" + std::to_string(j) + "] CALL");
                            else if (opcode == 0x9C || opcode == 0x8A || opcode == 156) SD_PRINT("    [" + std::to_string(j) + "] RET");
                        }
                    }
                }
            }

            // Skip to next chunk
            if (fseek(file, next_chunk_pos, SEEK_SET) != 0) {
                SD_PRINT("DataWinLoader: fseek failed!");
                break;
            }
        }
    } catch (...) {
        SD_PRINT("CRASH DETECTED IN DataWinLoader!");
        #ifdef __3DS__
        while (1) {
            gspWaitForVBlank();
            gfxSwapBuffers();
        }
        #endif
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

}}
