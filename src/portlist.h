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

typedef	in_port_t	h_port_t;	/* emphasize host byte order */

typedef	struct	portlist	PORTLIST;


int	portlist_Create (PORTLIST** plp);
static	int	portlist_resize (PORTLIST* pl, size_t size);
int	portlist_append_range (PORTLIST* pl, h_port_t low, h_port_t high);
int	portlist_append_port (PORTLIST* pl, h_port_t port);
int	portlist_negate (PORTLIST* pl);
int	portlist_contains (PORTLIST* pl, h_port_t port);
# endif
