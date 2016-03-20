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
static void
dump_v6addr( unsigned char *buffer ) {
    int i;

    printf( "%02x%02x:", buffer[0], buffer[1] );
    for ( i = 2 ; i < 16 ; i += 2 ) {
        if ( buffer[i] == 0 && buffer[i+1] == 0 ) continue;
        printf( ":%02x%02x", buffer[i], buffer[i+1] );
    }
}

/*
 */
static void
dump_packet( unsigned char *buffer, ssize_t length ) {
    printf( "%02x:%02x:%02x:%02x:%02x:%02x <- ", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5] );
    buffer += 6;
    printf( "%02x:%02x:%02x:%02x:%02x:%02x [", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5] );
    buffer += 6;
    printf( "%02x%02x] ", buffer[0], buffer[1] );
    buffer += 2;
    length -= 14;

    printf( "%02x%02x%02x%02x%02x%02x%02x%02x ", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7] );
    buffer += 8; length -= 8;
    dump_v6addr( buffer ); buffer += 16; length -= 16;
    printf( " " );
    dump_v6addr( buffer ); buffer += 16; length -= 16;
    printf( " " );

    int i;

    for ( i = 0 ; i < length ; ++i ) {
        printf( " %02x", buffer[i] );
    }
    printf( "\n" );
}

/*
 */
static ssize_t
forward( int in, int out ) {
    unsigned char buffer[2048];

    ssize_t octets = read( in, buffer, sizeof(buffer) );

    // ssize_t written = write( out, buffer, octets );
    dump_packet( buffer, octets );

#if 0
    if ( written < 0 ) {
        perror( "write failed" );
    }

    if ( written < octets ) {
        perror( "write truncated" );
    }

    return written;
#endif
    return octets;
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
