/* Copyright (C) 1991, 1992, 1993 Free Software Foundation, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-1999.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#include "../common.h"

#ifdef FNMATCH

#ifndef	_FNMATCH_H

#define	_FNMATCH_H	1

#ifdef	__cplusplus
extern "C" {
#endif

#if defined (__cplusplus) || (defined (__STDC__) && __STDC__)
#undef	__P
#define	__P(protos)	protos
#else /* Not C++ or ANSI C.  */
#undef	__P
#define	__P(protos)	()
/* We can get away without defining `const' here only because in this file
   it is used only inside the prototype for `fnmatch', which is elided in
   non-ANSI C where `const' is problematical.  */
#endif /* C++ or ANSI C.  */


/* We #undef these before defining them because some losing systems
   (HP-UX A.08.07 for example) define these in <unistd.h>.  */
#undef	FNM_PATHNAME
#undef	FNM_NOESCAPE
#undef	FNM_PERIOD

/* Bits set in the FLAGS argument to `fnmatch'.  */
#define	FNM_PATHNAME	(1 << 0) /* No wildcard can ever match `/'.  */
#define	FNM_NOESCAPE	(1 << 1) /* Backslashes don't quote special chars.  */
#define	FNM_PERIOD	(1 << 2) /* Leading `.' is matched only explicitly.  */

#if !defined (_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 2 || defined (_GNU_SOURCE)
#define	FNM_FILE_NAME	FNM_PATHNAME /* Preferred GNU name.  */
#define	FNM_LEADING_DIR	(1 << 3) /* Ignore `/...' after a match.  */
#define	FNM_CASEFOLD	(1 << 4) /* Compare without regard to case.  */
#endif

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#define	FNM_NOMATCH	1

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
extern int fnmatch __P ((const char *__pattern, const char *__string,
			 int __flags));

#ifdef	__cplusplus
}
#endif

#endif /* fnmatch.h */

#endif /* FNMATCH */

