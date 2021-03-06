/***
*wcsset.c - sets all characters of wchar_t string to given character
*
*       Copyright (c) 1985-1997, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _wcsset() - sets all of the characters in a string (except
*       the L'\0') equal to a given character (wide-characters).
*
*******************************************************************************/


#include <cs_type.h>
#include <C:\CoolStudio\crosscompiler\jade\include\string.h>

/***
*wchar_t * wcsset(string, val) - sets all of string to val (wide-characters)
*
*Purpose:
*       Sets all of wchar_t characters in string (except the terminating '/0'
*       character) equal to val (wide-characters).
*
*
*Entry:
*       wchar_t *string - string to modify
*       wchar_t val - value to fill string with
*
*Exit:
*       returns string -- now filled with val's
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

wchar_t * wcsset (
        wchar_t * string,
        wchar_t val
        )
{
        wchar_t *start = string;

        while (*string)
                *string++ = (wchar_t)val;

        return(start);
}
