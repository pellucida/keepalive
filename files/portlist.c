/*
//	@(#) portlist.c - maintain a list of ports possibly negated.
//
//	License: Creative Commons CC0 
//		http://creativecommons.org/publicdomain/zero/1.0/legalcode
//	Author:	James Sainsbury
//		toves@sdf.lonestar.org
//
*/

# include       <sys/types.h>
# include	<stdio.h>

# include	"constants.h"
# include	"new.h"
# include	"portlist.h"

/* If we are building a shared library */
# if	!defined( SOLIB)
# define	SOLIB
# endif
enum	{
	PORTLIST_MINSIZE	= 4
};

typedef	struct	portrange {
	h_port_t	low;
	h_port_t	high;
} PORTRANGE;

struct	portlist {
	int	negated;
	size_t	size;
	size_t	used;
	PORTRANGE*	ports;
};

/*
//	Create the list as empty - full is negated empty
*/
SOLIB int	portlist_Create (PORTLIST** plp) {
	PORTLIST*	pl	= 0;
	int	result	= New(pl);
	if (result==ok) {
		pl->negated	= false;
		pl->size	= 0;
		pl->used	= 0;
		pl->ports	= 0;
		*plp		= pl;
	}
	return	result;
}	
static	int	portlist_resize (PORTLIST* pl, size_t size) {
	int	result	= err;
	if (size < PORTLIST_MINSIZE)
		size	= PORTLIST_MINSIZE;
	if (pl->size == 0) 
		result	= NewArr (pl->ports, size);
	else
		result	= ReSizeArr (pl->ports, size);
	if (result==ok)
		pl->size	= size;
	return	result;
}
SOLIB int	portlist_append_range (PORTLIST* pl, h_port_t low, h_port_t high) {
	int	result	= ok;
	size_t	used	= pl->used;
	size_t	size	= pl->size;
	if (used >= size) {
		result	= portlist_resize (pl, 2*size);
		if (result==ok) {
			result	= portlist_append_range (pl, low, high);
		}
	}
	else	{
		pl->ports[used].low	= low;
		pl->ports[used].high	= high;
		pl->used	= used+1;
	}
	return	result;
}
SOLIB int	portlist_append_port (PORTLIST* pl, h_port_t port) {
	return	portlist_append_range (pl, port, port);
}
SOLIB int	portlist_negate (PORTLIST* pl) {
	int	result	= pl->negated;
	pl->negated	= true;
	return	result;

}
# if	defined(DEBUG)
int	portlist_print (PORTLIST* pl, FILE* output) {
	PORTRANGE*	pr	= pl->ports;
	if (pl->negated && !pr) {
		fprintf (output, "ports(*)\n");
		return	0;
	}
	else if (pr) {
		size_t	i	= 0;
		fprintf (output, "ports %d (", pl->used);
		if (pl->negated)
			fprintf (output, "!");
		for (;i < pl->used; ++i) {
			fprintf (output, "%d-%d,",pr[i].low, pr[i].high);
		}
		fprintf (output, ")\n");
	}
	return	0;
}
# endif
SOLIB int	portlist_contains (PORTLIST* pl, h_port_t port) {
	int	result	= false;
	PORTRANGE*	pr	= pl->ports;
	if (pr != 0) {
		size_t	i	= 0;
		size_t	j	= pl->used;
		while (i!=j) {
			h_port_t	low	= pr[i].low;
			h_port_t	high	= pr[i].high;
			if (low <= port && port <= high) {
				j	= i;
				result	= true;
			}
			else	++i;
		}
	}
	if (pl->negated)
		result	= !result;
	return	result;
}
