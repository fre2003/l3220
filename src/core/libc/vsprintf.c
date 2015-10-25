/******************************************************************\
 * This code is derived from NetBSD code, for which the following *
 * copyright notice applies.                                      *
 ******************************************************************/

/*-
 * Copyright (c) 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)printf.c	8.1 (Berkeley) 6/11/93
 */

/*
 * Scaled down version of printf(3).
 */

#include <cs_type.h>
#include "ctype.h"

static void _printn(unsigned long, unsigned int, unsigned int);
static void _doprnt(const char *, va_list);

static char *sbuf, *ebuf;

int
vsnprintf(char *buf, int size, const char *fmt, va_list ap)
{

	sbuf = buf;
	ebuf = buf + size - 1;
	_doprnt(fmt, ap);
	*sbuf = '\0';
	return (sbuf - buf);
}

int
vsprintf(char *buf, const char *fmt, va_list ap)
{

	return (vsnprintf(buf, -(int)buf, fmt, ap));
}

int
sprintf(char *buf, const char *fmt, ...)
{
	va_list ap;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(buf, -(int)buf, fmt, ap);
	va_end(ap);
	return (len);
}

int
snprintf(char *buf, int size, const char *fmt, ...)
{
	va_list ap;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(buf, size, fmt, ap);
	va_end(ap);
	return (len);
}

/* "sputchar" */
#define _put(c) \
	if (sbuf < ebuf) *sbuf++ = (c);

/*
 * Macros for converting digits to letters and vice versa
 */
#define	to_digit(c)	((c) - '0')
#define is_digit(c)	((unsigned)to_digit(c) <= 9)
#define	to_char(n)	((char)((n) + '0'))

/* flags definition */
/* 4 bits reserved for base :) */
#define FLAG_BASE_8		8
#define FLAG_BASE_10	10
#define FLAG_BASE_16	16
#define MASK_FLAG_BASE  0x1a

#define FLAG_HALF_WORD	0x01
#define FLAG_LONG_WORD	0x04
#define FLAG_ZERO_PAD	0x20
#define FLAG_RIGHT_PAD	0x40  
#define FLAG_WITH_PREC	0x80
#define FLAG_SPACE		0x100
#define FLAG_PLUS		0x200
#define FLAG_SIGNED		0x400

static void
_doprnt(const char *fmt, va_list ap)
{
	char *p;
    const char *fmt0;
	int ch;
	unsigned long ul;
	unsigned int flags = 0;
	unsigned int width = 0;
	unsigned int prec = 0;

	for (;;) {
		while ((ch = *fmt++) != '%') {
			if (ch == '\0')
				return;
			_put(ch);
		}
		flags = width = prec = 0;
        fmt0 = fmt; /* useful to test if we are at the first char */
reswitch:	switch (ch = *fmt++) {
		case '0':
            /* 0 can be either zero padding if first after %
               or part of a precision or width if after... */
            if (fmt == fmt0 + 1) {
                flags |= FLAG_ZERO_PAD;
                goto reswitch;
            }
            /* else fall through */
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			if (flags & FLAG_WITH_PREC) {
				prec = 10 * prec + to_digit(ch);
			}
			else { 
				width = 10 * width + to_digit(ch);
			}
			goto reswitch;
		case '-':
			flags |= FLAG_RIGHT_PAD;
			goto reswitch;
		case ' ':
			flags |= FLAG_SPACE;
			goto reswitch;
		case '+':
			flags |= FLAG_PLUS;
			goto reswitch;
		case '.':
			flags |= FLAG_WITH_PREC;
			goto reswitch;
		case 'h':
			flags |= FLAG_HALF_WORD;
			goto reswitch;
		case 'l':
			flags |= FLAG_LONG_WORD;
			goto reswitch;
		case 'c':
			ch = va_arg(ap, int);
			_put(ch & 0x7f);
			break;
		case 's':
			p = va_arg(ap, char *);
			if ((flags & FLAG_WITH_PREC) && prec) {
				while ((prec--) && (ch = *p++))  
					_put(ch);
			}
			else {
				while ((ch = *p++))
					_put(ch);
			}
			break;
		case 'd':
			ul = va_arg(ap, long);
			flags |= (FLAG_BASE_10|FLAG_SIGNED);
			_printn(ul, width, flags);
			break;
		case 'o':
			ul = va_arg(ap, unsigned long);
			flags |= FLAG_BASE_8;
			_printn(ul, width, flags);
			break;
		case 'u':
			ul = va_arg(ap, unsigned long);
			flags |= FLAG_BASE_10;
			_printn(ul, width, flags);
			break;
		case 'p':
			_put('0');
			_put('x');
			/* fall through */
		case 'x': case 'X':
			ul = va_arg(ap, unsigned long);
			flags |= FLAG_BASE_16;
			_printn(ul, width, flags);
			break;
		default:
			_put('%');
			if (flags & FLAG_HALF_WORD) 
				_put('h');
			if (flags & FLAG_LONG_WORD) 
				_put('l');
			if (ch == '\0')
				return;
			_put(ch);
		} /* end switch */
	} /* end for */
}

static void
_printn(unsigned long ul, unsigned int width, unsigned int flags)
{
					/* hold a long in base 8 */
	char *p, buf[(sizeof(long) * 8 / 3) + 1];
	unsigned int len, pad;
    char sign = 0;
	unsigned int base = flags & MASK_FLAG_BASE;

    if (flags & FLAG_SIGNED)
    {
			if ( (flags & FLAG_HALF_WORD) && ((short) ul < 0)) {
				sign = '-';
				ul = -(short)ul;
			}
			else if ((long)ul < 0) {
				sign = '-';
				ul = -(long)ul;
			}
			/* Plus overrides space ... */
			else if (flags & FLAG_PLUS) {
				sign = '+';
			}
			else if (flags & FLAG_SPACE) {
				sign = ' ';
			}
    }

    else if (flags & FLAG_HALF_WORD)
		ul = (unsigned short) ul;

	p = buf;
	do {
		*p++ = "0123456789abcdef"[ul % base];
	} while (ul /= base);

	len = (unsigned int) (p - buf);
	pad =  width - len;

    if (sign) {
        if (flags&FLAG_ZERO_PAD) _put(sign);
        pad--;
    }

	if ( !(flags&FLAG_RIGHT_PAD) && (width > len)) {
		while (pad--) {
			_put((flags & FLAG_ZERO_PAD) ? '0' : ' ');
		}
	}

    if (sign && !(flags & FLAG_ZERO_PAD))
        _put(sign);

	do {
		_put(*--p);
	} while (p > buf);

	if ( (flags&FLAG_RIGHT_PAD) && (width > len)) {
		while (pad--) {
			_put(' ');
		}
	}
}

