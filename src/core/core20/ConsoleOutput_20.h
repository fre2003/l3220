#if !defined(__CONSOLE_H_)
#define __CONSOLE_H_

// or use _tcscpy in tchar.h
#ifdef UNICODE
#define STRCPY lstrcpy 
#define STRLEN lstrlen
#define SPLITPATH  _wsplitpath 
#else
#define STRCPY strcpy 
#define STRLEN strlen
#define SPLITPATH  _splitpath 
#endif 


class CConsole
{
public:
	CConsole() 
	{ 
		hConsole = NULL; 
	};

public:
	// create the console
	BOOLEAN   Create(const TCHAR* szTitle, BOOLEAN bNoClose = false);
	
	// set color for output
	void   Color(WORD wColor = NULL);
	// write output to console
	void   Output(const TCHAR* szOutput = NULL, ...);
	
	// set and get title of console
	void   SetTitle(const TCHAR* szTitle);
	TCHAR*  GetTitle();

	// get HWND and/or HANDLE of console
	HWND   GetHWND();
	HANDLE GetHandle();

	// show/hide the console
	void   Show(BOOLEAN bShow = true);
	// disable the [x] button of the console
	void   DisableClose();
	// clear all output
	void   Clear();

	// close the console and delete it
	void   Close();

private:
	HANDLE hConsole;
};

#endif 
