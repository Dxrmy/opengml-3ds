#include "ogm/project/DataWinLoader.hpp"
#include <cstring>
#include <cstdio>

#ifdef __3DS__
#include <3ds.h>
#endif

namespace ogm { namespace project {

DataWinLoader::DataWinLoader(const char* path)
    : m_path(path)
{ }

bool DataWinLoader::load() {
    std::cout << "STEP 1: OPENING FILE" << std::endl;
    std::cout << "DataWinLoader: Opening " << m_path << "..." << std::endl;
    
    FILE* f = fopen(m_path.c_str(), "rb");
    
    #ifdef __3DS__
    std::cout << "DEBUG: fopen returned " << (void*)f << std::endl;
    if (f == NULL) {
        std::cout << "CRITICAL: fopen returned NULL!" << std::endl;
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
    if (f == NULL) return false;
    #endif

    try {
        std::cout << "DataWinLoader: Reading FORM header..." << std::endl;
        char form_id[4];
        if (fread(form_id, 1, 4, f) != 4) {
            std::cerr << "DataWinLoader: Failed to read 4 bytes for header." << std::endl;
            fclose(f);
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
            fclose(f);
            return false;
        }

        std::cout << "STEP 2: FORM FOUND" << std::endl;
        
        uint32_t form_size;
        fread(&form_size, 4, 1, f);
        std::cout << "DataWinLoader: Found FORM chunk, size indicator: " << form_size << std::endl;

        std::cout << "DataWinLoader: Entering chunk loop..." << std::endl;
        while (true) {
            #ifdef __3DS__
            gfxSwapBuffers();
            svcSleepThread(50 * 1000 * 1000); // 50ms delay
            #endif

            char chunk_id[4];
            if (fread(chunk_id, 1, 4, f) != 4) {
                if (feof(f)) {
                    std::cout << "DataWinLoader: Reached end of file." << std::endl;
                } else {
                    std::cerr << "DataWinLoader: Error reading chunk ID." << std::endl;
                }
                break;
            }
            
            uint32_t chunk_size;
            fread(&chunk_size, 4, 1, f);
            
            std::string chunk_name(chunk_id, 4);
            std::cout << "CHUNK: " << chunk_name << " (Size: " << chunk_size << ")" << std::endl;

            // Skip chunk data using fseek
            if (fseek(f, chunk_size, SEEK_CUR) != 0) {
                std::cerr << "DataWinLoader: fseek failed!" << std::endl;
                break;
            }
        }
    } catch (...) {
        std::cerr << "CRASH DETECTED!" << std::endl;
        #ifdef __3DS__
        while (1) {
            gspWaitForVBlank();
            gfxSwapBuffers();
        }
        #endif
        fclose(f);
        return false;
    }

    fclose(f);

    #ifdef __3DS__
    std::cout << "\n--- End of DataWin Scan ---\n" << std::endl;
    std::cout << "Press START to continue.\n" << std::endl;
    while (aptMainLoop()) {
        gspWaitForVBlank();
        gfxSwapBuffers();
        hidScanInput();
        if (hidKeysDown() & KEY_START) break;
    }
    #endif

    return true;
}

}}
