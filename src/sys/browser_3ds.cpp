#ifdef __3DS__

#include "ogm/sys/util_sys.hpp"

namespace ogm
{
    
void browser_open_url(const char* url, const char* target, const char* opts)
{
    // Not supported on 3DS
}

std::string browser_get_url() { return ""; }
std::string browser_get_domain() { return ""; }

}
#endif