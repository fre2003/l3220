//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a Core Service C++ Library.
// Copyright (C) 2002-2008 RISS Ltd.
// All rights reserved.
// 
// This source code or binary libary can be used, distributed or modified
// with or without modification, are permitted including commercial applications.
// -------------- 
//*******************************************************************************
#include "StdAfx.h"
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <windows.h>
#include <tchar.h>


/**
 * Convert a string into C string literal form using \a, \b, \f, \n, \r, \t, \v, and \ooo.
 * The return value is a newly allocated character array containing the result.
 * 4 bytes are allocated for each byte of the input because that is the maximum
 * expansion needed when all of the input needs to be output using the octal form.
 * The return value should be deleted with delete[].
 */
char* Slash(const char* s, bool quoteQuotes)
{
  char* oRet = new char[strlen(s) * 4 + 1];

  if (oRet)
  {
    char* o = oRet;

    while (*s)
    {
      if (*s == '\a')
      {
        *o++ = '\\';
        *o++ = 'a';
      }
      else if (*s == '\b')
      {
        *o++ = '\\';
        *o++ = 'b';
      }
      else if (*s == '\f')
      {
        *o++ = '\\';
        *o++ = 'f';
      }
      else if (*s == '\n')
      {
        *o++ = '\\';
        *o++ = 'n';
      }
      else if (*s == '\r')
      {
        *o++ = '\\';
        *o++ = 'r';
      }
      else if (*s == '\t')
      {
        *o++ = '\\';
        *o++ = 't';
      }
      else if (*s == '\v')
      {
        *o++ = '\\';
        *o++ = 'v';
      }
      else if (*s == '\\')
      {
        *o++ = '\\';
        *o++ = '\\';
      }
      else if (quoteQuotes && (*s == '\''))
      {
        *o++ = '\\';
        *o++ = '\'';
      }
      else if (quoteQuotes && (*s == '\"'))
      {
        *o++ = '\\';
        *o++ = '\"';
      }
      else if (isascii(*s) && (*s < ' '))
      {
        *o++ = '\\';
        *o++ = static_cast < char > ((*s >> 6) + '0');
        *o++ = static_cast < char > ((*s >> 3) + '0');
        *o++ = static_cast < char > ((*s & 0x7) + '0');
      }
      else
      {
        *o++ = *s;
      }
      s++;
    }
    *o = '\0';
  }
  return oRet;
}

/**
 * Is the character an octal digit?
 */
static bool IsOctalDigit(char ch)
{
  return ch >= '0' && ch <= '7';
}

/**
 * If the character is an hexa digit, get its value.
 */
static int GetHexaDigit(char ch)
{
  if (ch >= '0' && ch <= '9')
  {
    return ch - '0';
  }
  if (ch >= 'A' && ch <= 'F')
  {
    return ch - 'A' + 10;
  }
  if (ch >= 'a' && ch <= 'f')
  {
    return ch - 'a' + 10;
  }
  return- 1;
}

/**
 * Convert C style \a, \b, \f, \n, \r, \t, \v, \ooo and \xhh into their indicated characters.
 */
unsigned int UnSlash(char* s)
{
  char* sStart = s;
  char* o      = s;

  while (*s)
  {
    if (*s == '\\')
    {
      s++;
      if (*s == 'a')
      {
        *o = '\a';
      }
      else if (*s == 'b')
      {
        *o = '\b';
      }
      else if (*s == 'f')
      {
        *o = '\f';
      }
      else if (*s == 'n')
      {
        *o = '\n';
      }
      else if (*s == 'r')
      {
        *o = '\r';
      }
      else if (*s == 't')
      {
        *o = '\t';
      }
      else if (*s == 'v')
      {
        *o = '\v';
      }
      else if (IsOctalDigit(*s))
      {
        int val = *s - '0';

        if (IsOctalDigit(*(s + 1)))
        {
          s++;
          val*= 8;
          val += *s - '0';
          if (IsOctalDigit(*(s + 1)))
          {
            s++;
            val*= 8;
            val += *s - '0';
          }
        }
        *o = static_cast < char > (val);
      }
      else if (*s == 'x')
      {
        s++;
        int val = 0;
        int ghd = GetHexaDigit(*s);

        if (ghd >= 0)
        {
          s++;
          val = ghd;
          ghd = GetHexaDigit(*s);
          if (ghd >= 0)
          {
            s++;
            val*= 16;
            val += ghd;
          }
        }
        *o = static_cast < char > (val);
      }
      else
      {
        *o = *s;
      }
    }
    else
    {
      *o = *s;
    }
    o++;
    if (*s)
    {
      s++;
    }
  }
  *o = '\0';
  return o - sStart;
}

/**
 * Convert C style \0oo into their indicated characters.
 * This is used to get control characters into the regular expresion engine.
 */
unsigned int UnSlashLowOctal(char* s)
{
  char* sStart = s;
  char* o      = s;

  while (*s)
  {
    if ((s[0] == '\\') && (s[1] == '0') && IsOctalDigit(s[2]) && IsOctalDigit(s[3]))
    {
      *o = static_cast < char > (8* (s[2] - '0') + (s[3] - '0'));

      s += 3;
    }
    else
    {
      *o = *s;
    }
    o++;
    if (*s)
      s++;
  }
  *o = '\0';
  return o - sStart;
}

#if 0 
static int UnSlashAsNeeded(SString & s, bool escapes, bool regularExpression)
{
  if (escapes)
  {
    char* sUnslashed = StringDup(s.c_str(), s.length());
    size_t len;

    if (regularExpression)
    {
      // For regular expressions, the only escape sequences allowed start with \0
      // Other sequences, like \t, are handled by the RE engine.
      len = UnSlashLowOctal(sUnslashed);
    }
    else
    {
      // C style escapes allowed
      len = UnSlash(sUnslashed);
    }
    s = sUnslashed;
    delete[]sUnslashed;
    return static_cast < int > (len);
  }
  else
  {
    return s.length();
  }
}
#endif 

// Default characters that can appear in a word
bool iswordcharforsel(char ch)
{
  return! strchr("\t\n\r !\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~", ch);
}

// Accept slightly more characters than for a word
// Doesn't accept all valid characters, as they are rarely used in source filenames...
// Accept path separators '/' and '\', extension separator '.', and ':', MS drive unit
// separator, and also used for separating the line number for grep. Same for '(' and ')' for cl.
bool isfilenamecharforsel(char ch)
{
  return! strchr("\t\n\r \"$%'*,;<>?[]^`{|}", ch);
}

static bool IsBrace(char ch)
{
  return ch == '[' || ch == ']' || ch == '(' || ch == ')' || ch == '{' || ch == '}';
}

BOOL TrimString(LPSTR lpString)
{

  LPSTR psz;


  if (!lpString || !*lpString)
    return FALSE;

  // Trim left
  psz = lpString;

  while (*psz == ' ')
    psz = CharNext(psz);

  MoveMemory(lpString,psz,lstrlen(psz) + 1);

  // Trim right
  psz = StrEnd(lpString);

  while (*(psz = CharPrev(lpString,psz)) == ' ')
    *psz = '\0';

  return TRUE;
}

