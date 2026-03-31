#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace ogm { namespace project {

class DataWinLoader {
public:
    DataWinLoader(const char* path);
    
    // Parses the IFF structure and prints chunk IDs
    bool load();

private:
    std::string m_path;
    
    struct ChunkHeader {
        char id[4];
        uint32_t size;
    };
};

}}
