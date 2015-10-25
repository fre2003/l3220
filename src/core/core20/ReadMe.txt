---------------------------------------------------------
For Building this module, should define the switchs:
1. HAVE_NO_VERSIONINFO_LIB // Don't link this owner lib 

---------------------------------------------------------
TO Call this module, should define the following switchs.
1. HAVE_NO_CPP_APIS    // Skip the CPP Class define.
2. RTP_HAVE_STATIC_LIB // Have the static lib to link


---------------------------------------------------------
// in common.h define the platform 
#define _WIN32_WINNT   0x500    //Win2000 
#define _WIN32_WINDOWS 0x0500 

// #pragma comment( user, "Compiled on " __DATE__ " at " __TIME__ ) 
// #pragma comment(linker, "/WARN:4")
//#pragma comment(linker, "/NOENTRY")
// #pragma comment(linker, "/VERSION:1.2")
#pragma comment(exestr, "ITS ")
#pragma comment(exestr, "Compiled on " __DATE__ " at " __TIME__ )
