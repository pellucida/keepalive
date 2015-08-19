/*
//	@(#) keepalive.c - parse keepalive config file
//
//	License: Creative Commons CC0 
//		http://creativecommons.org/publicdomain/zero/1.0/legalcode
//	Author:	James Sainsbury
//		toves@sdf.lonestar.org
//
*/
# include       <sys/types.h>
# include       <sys/socket.h>
# include       <netdb.h>
# include       <netinet/in.h>
# include       <arpa/inet.h>

# include	<unistd.h>
# include	<stdio.h>
# include	<string.h>
# include	<stdlib.h>
# include	"constants.h"
# include	"new.h"
# include	"cfile.h"
# include	"portlist.h"
# include	"keepalivecfg.h"

/*
// Convert a string usually ip address or fqdn to in_addr_t (NBO)
*/
static	int	str_to_inaddr (char ip[], in_addr_t* adrp) {
	int	result	= ok;
	if (inet_pton (AF_INET, ip, adrp)==0) {
		struct  addrinfo        hints;
		struct  addrinfo*       pres	= 0;
		memset(&hints, 0, sizeof(hints));
		result	= err;

		hints.ai_family         = PF_INET;
		if (getaddrinfo (ip, 0, &hints, &pres) == ok) {
			struct sockaddr_in*	s	= (struct sockaddr_in*)pres->ai_addr;
			if (s) {
				*adrp	= s->sin_addr.s_addr;
				result	= ok;
			}
		}
		freeaddrinfo (pres);
	}
	return	result;
}
/*
// -----------------------------------------------------------------
*/
static	int	parse_portlist (PORTLIST* pl, char ports[]) {
	int	result	= ok;
	char*	t	= ports;
	char*	p	= 0;

	while ((p = strsep (&t, ","))!= 0 && result==ok) {
		h_port_t	low	= 0;
		h_port_t	high	= 0;
		char*	s	= strchr (p, '-');
		if (s) {
			*s++	= '\0';
			low	= atoi (p);
			high	= atoi (s);
			if (low > high) {
				h_port_t tmp	= low;
				low	= high;
				high	= tmp;
			}
			result	= portlist_append_range (pl, low, high);
		}
		else	{
			low	= atoi (p);
			result	= portlist_append_port (pl, low);
		}
	}
	return	result;
}
static	int	make_portlist (PORTLIST** plp, char ports[]) {
	PORTLIST*	pl	= 0;
	int	result	= portlist_Create(&pl);
	if (result == ok) {
		int	ch	= ports[0];
		char*	p	= ports;

		switch (ch) {
		case	'*':
		case	'-':
			portlist_negate (pl);
			result	= ok;
		break;
		case	'!':
			portlist_negate (pl);
			++p;
		default:
			result	= parse_portlist (pl, p);
		}
	}
	if (result == ok) {
		*plp	= pl;
	}
	return	result;
}
	
/*
//	[!]10.117.192.0/255.255.240.0
*/
typedef	struct	srcdst {
	int		negated;
	h_addr_t	tofrom;
	h_addr_t	mask;
}	SRCDST;

struct	keepalive	{
	int	so_keepalive;
	int	tcp_idle;
	int	tcp_intvl;
	int	tcp_cnt;
};
typedef	struct	keepalive	KEEPALIVE;
typedef	struct	klentry {
	int	call;	/* 'C' connect, 'A' accept */
	SRCDST	srcdst;	
	PORTLIST*	port;
	KEEPALIVE	keepalive;
}	KLENTRY;

enum	{
	KLENTRY_SIZEMIN	= 32
};
struct	cfg_kl {
	size_t	size;
	size_t	used;
	KLENTRY*	cfg;	
}; 

int	cfg_Create (CFG_KL** cfp, size_t desired) {
	int	result	= err;
	size_t	n	= desired < KLENTRY_SIZEMIN ? KLENTRY_SIZEMIN : desired;
	CFG_KL*	cf	= 0;
	if (New(cf)==ok) {
		KLENTRY*	cfg	= 0;
		
		if (NewArr(cfg, n)==ok) {
			cf->cfg		= cfg;
			cf->size	= n;
			cf->used	= 0;
			*cfp	= cf;
			result	= ok;
		}
		else	{
			Dispose (cf);
		}
	}
	return	result;
}
static	int	cfg_append (CFG_KL* cf, char* type, char* ip, char* mask, char* ports, char* so_keepalive, char* tcp_idle, char* tcp_intvl, char* tcp_cnt) {
	int	result	= err;
	in_addr_t	nmsk;
	in_addr_t	ndst;
	h_addr_t	msk;
	h_addr_t	dst;
	int	negated	= false;
	char*		dports	= "*";
	PORTLIST*	pl	= 0;
	int	x_keepalive	= 0;
	int	x_idle		= 0;
	int	x_intvl		= 0;
	int	x_cnt		= 0;
	size_t	used	= cf->used;
	size_t	size	= cf->size;
	KLENTRY*	cfg	= &(cf->cfg[used]);
	
	if (used+1 > size) {
		size_t	newsize		= 2*size;
		if (ReSizeArr(cf->cfg, newsize)==ok) {
			cf->size	= newsize;
			return cfg_append (cf, type, ip, mask, ports, so_keepalive, tcp_idle, tcp_intvl,  tcp_cnt);
		}
		else	return err;
	}

	if (ip[0] == '!') {
		negated	= true;
		++ip;
	}
	if (*ip == '*') {
		ip	= "0.0.0.0";
		inet_pton (AF_INET, "0.0.0.0", &nmsk);
	}
	else if (strchr (mask, '.')==0) {
		int     cidr    = atoi(mask);
		if (0 <= cidr && cidr <= 32) {
			int     shift   = 32 - cidr;
			nmsk = htonl(((~0u)<<shift));
		}
	}
	else	{
		inet_pton (AF_INET, mask, &nmsk);
	}
	msk	= ntohl (nmsk);
	str_to_inaddr (ip, &ndst);
	dst	= ntohl (ndst);

	if (ports && ports[0] != '*') {
		dports	= ports;
	}
	make_portlist (&pl, dports);

	if (so_keepalive && so_keepalive[0] == 'Y') {
		x_keepalive	= 1;
	}
	if (tcp_idle && tcp_idle[0] != '-') {
		x_idle	= atoi (tcp_idle);
	}
	if (tcp_intvl && tcp_intvl[0] != '-') {
		x_intvl	= atoi (tcp_intvl);
	}
	if (tcp_cnt && tcp_cnt[0] != '-') {
		x_cnt	= atoi (tcp_cnt);
	}
	
	cfg->call		= type[0];
	cfg->srcdst.negated	= negated;
	cfg->srcdst.tofrom	= dst;
	cfg->srcdst.mask	= msk;
	cfg->port		= pl;
	cfg->keepalive.so_keepalive	= x_keepalive;
	cfg->keepalive.tcp_idle		= x_idle;
	cfg->keepalive.tcp_intvl	= x_intvl;
	cfg->keepalive.tcp_cnt		= x_cnt;

	cf->used		= used+1;
	
	return result;
}

const	char	SEPS[]	= " \t";
static	char	MASK[]	= "255.255.255.255";

static	int	cfg_parseline (CFG_KL* pcf, char* line) {
	int	result	= ok;
	char*	l	= line;
	char*	t	= strchr (l, '#');
	char*	type	= 0;
	char*	destip	= 0;
	char*	mask	= MASK;
	char*	portlist	= 0;
	char*	so_keepalive	= 0;
	char*	tcp_idle	= 0;
	char*	tcp_intvl	= 0;
	char*	tcp_cnt		= 0;

	if (t) {
		*t	= '\0';
	}
	while (*l != '\0' && isspace(*l))
		++l;
	if (*l == '\0')
		return	result;

	while ((t = strsep(&l, SEPS))!=0 && *t == '\0')
		; 
	if (t) {
		type	= t;
	}
	while ((t = strsep(&l, SEPS))!=0 && *t == '\0')
		; 
	if (t) {
		destip	= t;
		char*	m	= strchr (t, '/');
		if (m) {
			*m++	= '\0';
			mask	= m;
		}
	}
	while ((t = strsep(&l, SEPS))!=0 && *t == '\0')
		; 
	if (t) {
		portlist	= t;
	}
	while ((t = strsep(&l, SEPS))!=0 && *t == '\0')
		; 
	if (t) {
		so_keepalive	= t;
	}
	while ((t = strsep(&l, SEPS))!=0 && *t == '\0')
		; 
	if (t) {
		tcp_idle	= t;
	}
	while ((t = strsep(&l, SEPS))!=0 && *t == '\0')
		; 
	if (t) {
		tcp_intvl	= t;
	}
	while ((t = strsep(&l, SEPS))!=0 && *t == '\0')
		; 
	if (t) {
		tcp_cnt	= t;
	}
	return	cfg_append (pcf, type, destip, mask, portlist, so_keepalive, tcp_idle, tcp_intvl, tcp_cnt);
}

int	cfg_init (CFG_KL** cfp, char* cfgfile) {
	CFG_KL*	cf	= 0;
	int	result	= cfg_Create (&cf, 0);
	if (result == ok) {
		CFILE*	cfd	= 0;	
		result	= cfile_open (&cfd, cfgfile);
		if (result == ok) {
			char	line[BUFSIZ];
			
			while (cfile_getline (cfd, line, sizeof(line)-1)!=EOF) {
				cfg_parseline (cf, line);
			}
			cfile_close (cfd);
			*cfp	= cf;
		}
	}
	return	result;
}

/*
// Lookup for connect (type == 'C')
//	destip dport  -> so_keepalive = 1 (Y) tcp_idle etc
//                 
// Lookup for accept (type == 'A')
//	srcip  dport --> so_keepalive = 1 (Y) tcp_idle etc
//	note that is client srcip connecting to local bound port dport
//	
//	Return true if match found; false otherwise
*/

int	cfg_parameters (CFG_KL* cf, int sd, int type,
		__CONST_SOCKADDR_ARG sock, socklen_t len,
		int* pso_keepalive,
		int* ptcp_idle, int* ptcp_intvl, int* ptcp_cnt)
{
	int	result	= false;
	struct  sockaddr_in*       s       = *(struct sockaddr_in**)&sock;
	size_t	i	= 0;
	size_t	j	= cf->used;
	KLENTRY*	cfg	= cf->cfg;
	h_port_t	port	= ntohs (s->sin_port);
	h_addr_t	srcdst	= ntohl (s->sin_addr.s_addr);
	if (type == 'A') { /* accept(2) call need the bound port */
		struct	sockaddr_in	ac_skt;
		socklen_t	len	= sizeof(ac_skt);
		getsockname (sd, (struct sockaddr*)&ac_skt, &len);
		port	= ntohs (ac_skt.sin_port);
	}

	while (i != j) {
		KLENTRY	pe	= cfg[i];
		int	call	= pe.call;
		if (type == call) {	
			SRCDST	pd	= pe.srcdst;
			PORTLIST*	pl	= pe.port;
		
			int	match	= (pd.tofrom & pd.mask)==(srcdst & pd.mask);
			if (match || pd.negated) {
				match	= !match;
				if (portlist_contains (pl, port)) {
					KEEPALIVE	kl	= pe.keepalive;
					*pso_keepalive	= kl.so_keepalive;
					*ptcp_idle	= kl.tcp_idle;
					*ptcp_intvl	= kl.tcp_intvl;
					*ptcp_cnt	= kl.tcp_cnt;
					j	= i;
					result	= true;
				}
				else	++i;
			}
			else	++i;
		}  
		else	++i;
	}
	return	result;
}
