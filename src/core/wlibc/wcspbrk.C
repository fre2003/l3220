/***
*wcscmp.c - routine to compare two wchar_t strings (for equal, less, or greater)
*
*       Copyright (c) 1985-1997, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Compares two wide-character strings, determining their lexical order.
*
*******************************************************************************/


#include <cs_type.h>
#include <C:\CoolStudio\crosscompiler\jade\include\string.h>

/***
*wchar_t *wcspbrk(string, control) - scans string for a character from control
*
*Purpose:
*	Returns pointer to the first wide-character in
*	a wide-character string in the control string.
*
*Entry:
*	wchar_t *string - string to search in
*	wchar_t *control - string containing characters to search for
*
*Exit:
*	returns a pointer to the first character from control found
*	in string.
*	returns NULL if string and control have no characters in common.
*
*Exceptions:
*
*******************************************************************************/

wchar_t * wcspbrk(const wchar_t * string, const wchar_t * control) 
{
    wchar_t *wcset;
    /* 1st char in control string stops search */
    while (*string) {
        for (wcset = (wchar_t *) control; *wcset; wcset++)
            if (*wcset == *string)
                return (wchar_t *) string;
        string++;
    }
    return NULL;
}