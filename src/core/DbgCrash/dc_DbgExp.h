#if !defined(__DBGEXP_H__)
#define __DBGEXP_H__

#define ITS_HAVE_EXPORT_DLL

#ifdef ITS_HAVE_EXPORT_DLL 
  #define ITSAPI _declspec(dllexport) 
#else
  #define ITSAPI _declspec(dllimport) 
#endif 

#endif // __DBGEXP_H__
