/*
//	@(#) new.h - (vanity) wrappers for malloc &co
//
//	License: Creative Commons CC0 
//		http://creativecommons.org/publicdomain/zero/1.0/legalcode
//	Author:	James Sainsbury
//		toves@sdf.lonestar.org
//
*/
# if	!defined(NEW_H)
# define	NEW_H	1

# include	<stdlib.h>

# define	New(p)	(((p) = calloc(sizeof(*(p)),1))==0?err:ok)
# define	NewArr(a,n)	(((a) = calloc(sizeof((a)[0]), (n)))==0?err:ok)
# define	Dispose(a)	((a)==0||(free(a),(a)=0,ok))


static	inline	int	resize_array (void** pa, size_t size) {
	void*	na	= realloc (*pa, size);
	int	result	= err;
	if (na) {
		*pa	= na;
		result	= ok;
	}
	return	result;
}
# define	ReSizeArr(a,n)	\
	resize_array ((void*)&(a), n*sizeof((a)[0]))	

# endif
