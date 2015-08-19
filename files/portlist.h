/*
//	@(#) portlist.h
//
//	License: Creative Commons CC0 
//		http://creativecommons.org/publicdomain/zero/1.0/legalcode
//	Author:	James Sainsbury
//		toves@sdf.lonestar.org
//
*/
# if	!defined(PORTLIST_H)
# define	PORTLIST_H	1

# include	<netinet/in.h>

/* If we are building a shared library */
# if !defined( SOLIB)
# define	SOLIB
# endif

typedef	in_port_t	h_port_t;	/* emphasize host byte order */

typedef	struct	portlist	PORTLIST;


SOLIB	int	portlist_Create (PORTLIST** plp);
static	int	portlist_resize (PORTLIST* pl, size_t size);
SOLIB	int	portlist_append_range (PORTLIST* pl, h_port_t low, h_port_t high);
SOLIB	int	portlist_append_port (PORTLIST* pl, h_port_t port);
SOLIB	int	portlist_negate (PORTLIST* pl);
SOLIB	int	portlist_contains (PORTLIST* pl, h_port_t port);
# endif
