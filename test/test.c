#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>


int main(int argc, char* argv[])
{
   int s;
   int optval;
   socklen_t optlen = sizeof(optval);
   struct addrinfo* ai	= 0;
   struct addrinfo h;
	char buf [128];
   int	rv	= 0;
   char*	addr	= "129.78.76.34";
   char*	port	= "22";
   if (argc == 3) {
	addr	= argv[1];
	port	= argv[2];
   }
   else if (argc == 2) {
	addr	= argv[1];
   }
	
	memset (&h, 0, sizeof(h));
	h.ai_family	= AF_INET;
	h.ai_socktype	= SOCK_STREAM;
	
   if ((rv = getaddrinfo (addr, port, &h, &ai)) != 0) {
	fprintf (stderr, gai_strerror(rv));
	exit (1);
   }


   if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket()");
      exit(EXIT_FAILURE);
   }

   rv	= connect (s, ai->ai_addr, ai->ai_addrlen);
	if (rv != 0) {
		perror ("connect()");
	      exit(EXIT_FAILURE);
	}
	read (s, buf, 16);

   if(getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
      perror("getsockopt()");
      close(s);
      exit(EXIT_FAILURE);
   }
   printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));

   if(optval) {
#ifdef TCP_KEEPCNT
      if(getsockopt(s, SOL_TCP, TCP_KEEPCNT, &optval, &optlen) < 0) {
         perror("getsockopt()");
         close(s);
         exit(EXIT_FAILURE);
      }
      printf("TCP_KEEPCNT   = %d\n", optval);
#endif
   
#ifdef TCP_KEEPIDLE
      if(getsockopt(s, SOL_TCP, TCP_KEEPIDLE, &optval, &optlen) < 0) {
         perror("getsockopt()");
         close(s);
         exit(EXIT_FAILURE);
      }
      printf("TCP_KEEPIDLE  = %d\n", optval);
#endif
   
#ifdef TCP_KEEPINTVL
      if(getsockopt(s, SOL_TCP, TCP_KEEPINTVL, &optval, &optlen) < 0) {
         perror("getsockopt()");
         close(s);
         exit(EXIT_FAILURE);
      }
      printf("TCP_KEEPINTVL = %d\n", optval);
#endif
   }
   
   close(s);

   exit(EXIT_SUCCESS);
}
