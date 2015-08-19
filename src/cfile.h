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

typedef	struct	cfile	CFILE;
int	cfile_open (CFILE** cfp, char* file);
int	cfile_close (CFILE* cf);
int	cfile_getline (CFILE* cf, char* line, size_t linesz);

#endif
