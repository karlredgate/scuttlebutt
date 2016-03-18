/*
 * Copyright (c) 2010-2012 Karl N. Redgate
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_tun.h>

static int debug = 0;

/*
 */
static ssize_t
forward( int in, int out ) {
    unsigned char buffer[2048];

    ssize_t octets = read( in, buffer, sizeof(buffer) );

    ssize_t written = write( out, buffer, octets );

    if ( written < 0 ) {
        perror( "write failed" );
    }

    if ( written < octets ) {
        perror( "write truncated" );
    }

    return written;
}

/*
 */
static void
loop( int tap, int sock ) {
    struct timeval timeout;
    fd_set fds;

    for (;;) {
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        FD_ZERO( &fds );
        FD_SET( tap, &fds );

        int result = select( 32, &fds, NULL, NULL, &timeout );
        if ( result == -1 ) {
            perror( "select" );
            continue;
        }

        if ( result == 0 )  continue;

        if ( FD_ISSET(tap, &fds) ) {
            ssize_t octets = forward( tap, sock );
            if ( debug ) printf( "tap->sock %zd octets\n", octets );
        }
    }
}

/*
 */
static int
tap_open( char *name ) {
    int tap = open( "/dev/net/tun", O_RDWR );
    if ( tap < 0 ) {
        perror( "open tunnel driver" );
        exit( -1 );
    }

    struct ifreq request;
    memset( &request, 0, sizeof(request) );
    request.ifr_flags = IFF_TAP | IFF_NO_PI;
    snprintf( request.ifr_name, IFNAMSIZ, "%s", name );

    if ( ioctl(tap, TUNSETIFF, &request) < 0 ) {
        perror( "create tunnel device" );
        exit( -1 );
    }

    return tap;
}

/*
 */
int
main( int argc, char **argv ) {
    if ( argc < 2 ) {
        fprintf( stderr, "usage: %s interface-name\n", argv[0] );
        exit( -1 );
    }

    int tap = tap_open( argv[1] );

    loop( tap, 1 );

    return 0;
}

/* vim: set autoindent expandtab sw=4 : */
