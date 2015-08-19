/*
//	@(#) keepalivecfg.h
//
//	License: Creative Commons CC0 
//		http://creativecommons.org/publicdomain/zero/1.0/legalcode
//	Author:	James Sainsbury
//		toves@sdf.lonestar.org
//
*/
# if	!defined(KEEPALIVECFG_H)
# define	KEEPALIVECFG_H

# if  !defined(LIBKEEPALIVE_CFG)
# define	LIBKEEPALIVE_CFG	"/proxy/keep.cfg"
# endif

typedef	struct	cfg_kl	CFG_KL;
typedef      struct  sockaddr_in     SOCKADDR;
typedef	in_addr_t	h_addr_t;	

int	cfg_init (CFG_KL** cfp, char* cfgfile);
int	cfg_parameters (CFG_KL* cf, int sd, int type, __CONST_SOCKADDR_ARG sock, socklen_t len, int*, int*, int*, int*);
# endif
