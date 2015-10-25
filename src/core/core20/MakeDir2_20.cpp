#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <windows.h>
#include <Win32Shl.h>
#include <shlwapi.h>


static char *simplify(const char *str);

static void conver_str(const char *d, char* out)
{
	char  *endptr, *str;

	endptr = (char*)((char*)d + strlen(d));
	str    = (char*)d;

    while (str < endptr)
	{
		if(*str == '\\')
			*out = '/';
		else 
			*out = *str;
		out++; str++;
	}
}

static long g_errno = 0;
#define EEXIST 17
#define ENOENT 2
static int mk_dir( const char *dirname)
{
	if (CreateDirectoryA(dirname, NULL))
	{
		g_errno = 0;
		return 0;
	}
	else
	{
	   long err = GetLastError();
	   if(err == ERROR_FILE_EXISTS)
	   {
		   g_errno = EEXIST;//ENOENT
	   }
	   else
	   {
		   g_errno = ENOENT;//ENOENT
	   }
	}
	
	return -1;
}

int UTL_MakeDir(PCSTR d)
{
	char  *endptr, *ptr, *slash, *str;
	char src[260] = {0, };
  
	conver_str(d, src);

	str = simplify(src);
	
	/* If space couldn't be allocated for the simplified names, return. */
	
	if (str == NULL)
		return (-1);
	
	/* Try to make the directory */
	
	if (mk_dir(str) == 0) {
		free(str);
		return (0);
	}
	if (g_errno != ENOENT) {
		free(str);
		return (-1);
	}

	endptr = strrchr(str, '\0');
	slash = strrchr(str, '/');

	/* Search upward for the non-existing parent */
	
	while (slash != NULL) {
		
		ptr = slash;
		*ptr = '\0';
		
		/* If reached an existing parent, break */
		
		if (access(str, 0) == 0)
			break;
		
		/* If non-existing parent */
		
		else {
			slash = strrchr(str, '/');
			
			/* If under / or current directory, make it. */
			
			if (slash == NULL || slash == str) {
				if (mk_dir(str) != 0 && g_errno != EEXIST) {
					free(str);
					return (-1);
				}
				break;
			}
		}
	}
	
	/* Create directories starting from upmost non-existing parent */
	
	while ((ptr = strchr(str, '\0')) != endptr) {
		*ptr = '/';
		if (mk_dir(str) != 0 && g_errno != EEXIST) {
		/*
		*  If the mkdir fails because str already
		*  exists (EEXIST), then str has the form
		*  "existing-dir/..", and this is really
		*  ok. (Remember, this loop is creating the
		*  portion of the path that didn't exist)
			*/
			free(str);
			return (-1);
		}
	}
	free(str);
	return (0);
}

/*
*	simplify - given a pathname, simplify that path by removing
*		   duplicate contiguous slashes.
*
*		   A simplified copy of the argument is returned to the
*		   caller, or NULL is returned on error.
*
*		   The caller should handle error reporting based upon the
*		   returned vlaue, and should free the returned value,
*		   when appropriate.
*/

static char *
simplify(const char *str)
{
	int i;
	size_t mbPathlen;	/* length of multi-byte path */
	size_t wcPathlen;	/* length of wide-character path */
	wchar_t *wptr;		/* scratch pointer */
	wchar_t *wcPath;	/* wide-character version of the path */
	char *mbPath;		/* The copy fo the path to be returned */
	
						/*
						*  bail out if there is nothing there.
	*/
	
	if (!str)
		return (NULL);
	
		/*
		*  Get a copy of the argument.
	*/
	
	if ((mbPath = strdup(str)) == NULL) {
		return (NULL);
	}
	
	/*
	*  convert the multi-byte version of the path to a
	*  wide-character rendering, for doing our figuring.
	*/
	
	mbPathlen = strlen(mbPath);
	
	if ((wcPath = (wchar_t* )calloc(sizeof (wchar_t), mbPathlen+1)) == NULL) {
		free(mbPath);
		return (NULL);
	}
	
	if ((wcPathlen = mbstowcs(wcPath, mbPath, mbPathlen)) == (size_t)-1) {
		free(mbPath);
		free(wcPath);
		return (NULL);
	}
	
	/*
	*  remove duplicate slashes first ("//../" -> "/")
	*/
	
	for (wptr = wcPath, i = 0; i < (int)wcPathlen; i++) {
		*wptr++ = wcPath[i];
		
		if (wcPath[i] == '/') {
			i++;
			
			while (wcPath[i] == '/') {
				i++;
			}
			
			i--;
		}
	}
	
	*wptr = '\0';
	
	/*
	*  now convert back to the multi-byte format.
	*/
	
	if (wcstombs(mbPath, wcPath, mbPathlen) == (size_t)-1) {
		free(mbPath);
		free(wcPath);
		return (NULL);
	}
	
	free(wcPath);
	return (mbPath);
}
