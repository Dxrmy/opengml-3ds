#pragma once

#include "external.h"
#include "ogm/common/util.hpp"
#include "ogm/sys/util_sys.hpp"

namespace ogm::interpreter::ffi
{
    struct SharedLibraryType
    {
        enum {
            OS_ERROR,
            OS_WINDOWS,
            OS_UNIX,
            OS_APPLE,
        } os = OS_ERROR;
        
        enum {
            UNKNOWN,
            x86,
            x64
        } arch = UNKNOWN;
    
        bool archmatch() const
        {
            return (arch == x86 && is_32_bit()) || (arch == x64 && is_64_bit());
        }
        
        bool osmatch() const
        {
            #if defined(__unix__)
            return os == OS_UNIX;
            #endif
            
            #if defined(_WIN32) || defined(WIN32)
            return os == OS_WINDOWS;
            #endif
            
            #if defined(__APPLE__)
            return os == OS_APPLE;
            #endif
            
            return false;
        }
        
        bool platmatch() const
        {
            return archmatch() && osmatch();
        }
        
        bool compatible() const
        {
            if (platmatch()) return true;
            
            #if defined(EMBED_ZUGBRUECKE) && defined(PYTHON)
            if (os == OS_WINDOWS && zugbruecke_init())
            {
                return true;
            }
            #endif
            
            return false;
        }
    };
    
    SharedLibraryType getSharedLibraryTypeFromPath(const std::string& path);
    
    void path_transform(std::string& path);
}
