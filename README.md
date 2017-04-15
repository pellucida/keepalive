## Modified libkeepalive - connect(2) and accept(2)

Routers and firewalls dropping tcp connection after a short period
(eg five minutes) is a problem for applications that don't have
configurable application or tcp keepalive options.

Fabio Busatto's libkeepalive intercept library
[http://libkeepalive.sourceforge.net](http://libkeepalive.sourceforge.net)
works by intercepting function calls to `socket(2)` and
setting the keepalive parameters using `setsockopt(2)`. 

The actual values of (`SO_KEEPALIVE, TCP_KEEPCNT, TCP_KEEPIDLE, TCP_KEEPINTVL`)
are passed to the process in environment variables.

#### Modifications

Since these parameters only apply to tcp, my take on this approach was to intercept
only the `connect(2)` and `accept(2)` system calls and to retrieve the parameter
values from a configuration file instead of from the processes' environment.
eg

    # SSH to 192.94.73.15
    env LD_PRELOAD=/intercept/libkeepalive.so /usr/bin/ssh user@192.94.73.15

    # Example configuration file (location is defined at compile time.)
    
    # Connect entries
    #C   destips/mask   dest-portrange   keepalive   idle   intvl   count   
    # Accept entries
    #A   srcpip/mask    local-portrange  keepalive   idle   intvl   count   
    C     192.168.0.0/16     *           N           -      -       -
    C     192.94.73.15      22           Y           240     75     8
    

In this example the ssh matches the second entry and the parameters
are taken from the file.

#### BUGS
Not thread aware in any way (ie not reentrant or async safe.) So only suitable for single threaded processes. Connect(2) or accept(2) shouldn't be called directly or indirectly from signal handlers. 

#### LICENSE
[Creative Commons CC0](http://creativecommons.org/publicdomain/zero/1.0/legalcode)

#### AUTHOR
[James Sainsbury](mailto:toves@sdf.lonestar.org)
