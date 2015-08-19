/*
//	@(#) cfile.c - read a config file using mmap
//
//	License: Creative Commons CC0 
//		http://creativecommons.org/publicdomain/zero/1.0/legalcode
//	Author:	James Sainsbury
//		toves@sdf.lonestar.org
//
*/
# include       <sys/types.h>
# include	<sys/stat.h>
# include	<sys/mman.h>
# include	<fcntl.h>
# include	<stdio.h>
# include	<stdlib.h>
# include	"constants.h"
# include	"new.h"
# include	"cfile.h"

struct	cfile {
	size_t	size;
	size_t	pos;
	char*	data;
};


int	cfile_open (CFILE** cfp, char* file) {
	int	result	= err;
	CFILE*	cf	= 0;
	if (New(cf)==ok) {
		int	fd	= open (file, O_RDONLY);
		if (fd >= 0) {
			struct	stat	sb;
			if (fstat (fd, &sb) == 0) {
				size_t	size	= sb.st_size;
				char*	data	= mmap (0, size, PROT_READ, MAP_SHARED, fd, 0);
				if (data != MAP_FAILED) {
					cf->data	= data;
					cf->size	= size;
					cf->pos		= 0;
					*cfp		= cf;
					result	= ok;
				}
			}
			close (fd);
		}
	}
	return	result;
}

int	cfile_close (CFILE* cf) {
	int	result	= munmap (cf->data, cf->size);
	cf->data	= 0;
	cf->size	= 0;
	cf->pos		= 0;
	Dispose (cf);
	return	result;
}


int	cfile_getline (CFILE* cf, char* line, size_t linesz) {
	int	result	= EOF;
	size_t	size	= cf->size;
	size_t	pos	= cf->pos;
	char*	data	= cf->data;
	if (pos < size) {
		size_t	i	= 0;
		int	ch	= 0;
		while ((ch = data[pos++]) != '\n' && i < linesz && pos<size) {
			line [i++]	= ch;
		}
		line [i]	= '\0';
		cf->pos	= pos;
		result	= i;
	}
	return result;
}

