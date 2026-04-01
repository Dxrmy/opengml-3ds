#pragma once
#include <iostream>
#include <cstdio>
#include <cstring>

#ifdef __3DS__
#include <3ds.h>
inline void sd_log(const char* msg) {
    FILE* f = fopen("sdmc:/3ds/OpenGML/loader_log.txt", "a");
    if (f) {
        fprintf(f, "%s", msg);
        fclose(f);
    }
}
#define TRACE() \
    do { \
        char _tr_buf[256]; \
        snprintf(_tr_buf, sizeof(_tr_buf), "[TRACE] %s:%d (%s)\n", __FILE__, __LINE__, __FUNCTION__); \
        std::cout << _tr_buf; \
        sd_log(_tr_buf); \
    } while(0)

#define SD_PRINT(msg) \
    do { \
        std::cout << msg << std::endl; \
        std::string _cp_str = std::string(msg) + "\n"; \
        sd_log(_cp_str.c_str()); \
        svcOutputDebugString(_cp_str.c_str(), _cp_str.length()); \
    } while(0)
#else
#define TRACE() std::cout << "[TRACE] " << __FILE__ << ":" << __LINE__ << " (" << __FUNCTION__ << ")" << std::endl;
#define SD_PRINT(msg) std::cout << msg << std::endl;
#endif