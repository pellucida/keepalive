/*
//	@(#) cfile.h
//
//	License: Creative Commons CC0 
//		http://creativecommons.org/publicdomain/zero/1.0/legalcode
//	Author:	James Sainsbury
//		toves@sdf.lonestar.org
//
*/
# if	!defined( CFILE_H)
# define	CFILE_H

# if	!defined( SOLIB)
# define	SOLIB
# endif

typedef	struct	cfile	CFILE;
SOLIB	int	cfile_open (CFILE** cfp, char* file);
SOLIB	int	cfile_close (CFILE* cf);
SOLIB	int	cfile_getline (CFILE* cf, char* line, size_t linesz);

#endif
