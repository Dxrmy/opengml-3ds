#include "unzip.hpp"

#ifdef LIBZIP_ENABLED

#include "ogm/common/util.hpp"

#include <stdio.h>
#include <stdlib.h>

namespace ogm
{

static std::string escape_shell(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == ';' || c == '&' || c == '|' || c == '>' || c == '<' ||
            c == '$' || c == '\\' || c == '"' || c == '\'' || c == ' ' ||
            c == '(' || c == ')' || c == '[' || c == ']' || c == '{' ||
            c == '}' || c == '*' || c == '?' || c == '~' || c == '`' ||
            c == '#' || c == '\n' || c == '\r' || c == '\t') {
            result += '\\';
        }
        result += c;
    }
    return result;
}

bool unzip(const std::string& source, const std::string& dst)
{
    using namespace std;
    std::string command = "7z x " + escape_shell(source) + " -o" + escape_shell(dst);
    if (system(command.c_str()))
    {
        return false;
    }

    return true;
}

}

#else

namespace ogm
{

bool unzip(const std::string& source, const std::string& dst)
{
    return false;
}

}
#endif
