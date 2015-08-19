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


/* A single static per file probably cheaper than a static function */
/* not reentrant - so don't multithread unless tmp__ is tls */
/* and don't use a function call as one of the args to ReSizeArr() */

static	void*	tmp___	= 0;

# define	ReSizeArr(a,n)	\
	((tmp___ = realloc ((a),n*sizeof((a)[0])))==0?err:((a)=tmp___,ok))

# endif
