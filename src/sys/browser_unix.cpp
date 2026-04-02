#if defined(__unix__) || defined(__APPLE__)

#include "ogm/sys/util_sys.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

namespace ogm
{

void browser_open_url(const char* url, const char* target, const char* opts)
{
    std::string safe_url = encode_url(url);
    pid_t pid = fork();
    if (pid == 0)
    {
        // First child process
        pid_t pid2 = fork();
        if (pid2 == 0)
        {
            // Second child process (grandchild)
            #if defined(__APPLE__)
            execlp("open", "open", safe_url.c_str(), (char*)NULL);
            #else
            execlp("xdg-open", "xdg-open", safe_url.c_str(), (char*)NULL);
            #endif

            // If execlp fails, exit safely without calling atexit handlers
            _exit(1);
        }
        // First child exits immediately, making grandchild an orphan adopted by init, preventing zombie processes.
        _exit(0);
    }
    else if (pid > 0)
    {
        // Parent process waits for first child (which exits almost immediately)
        waitpid(pid, NULL, 0);
    }
}

std::string browser_get_url() { return ""; }
std::string browser_get_domain() { return ""; }

}
#endif