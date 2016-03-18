
#include <sys/types.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

struct ring {
    int fd;
    unsigned char *buffer;
};

int
create_receiver( int index ) {
    int sock = socket( AF_PACKET, SOCK_RAW, htons(ETH_P_ALL) );

    struct sockaddr_ll address;

    address.sll_family = AF_PACKET;
    address.sll_protocol = htons( ETH_P_ALL );
    address.sll_ifindex = index;

    int result = bind( sock, (struct sockaddr *)&address, sizeof (struct sockaddr_ll) );

    if ( result < 0 ) {
        perror( "bind" );
        exit( -1 );
    }

    struct ring *ring = (struct ring *)malloc( sizeof (struct ring) );

    if ( ring == NULL ) {
    }
}

int
create_transmitter( int index ) {
    int packet_socket = socket( AF_PACKET, SOCK_RAW, 0 );
}

/* vim: set autoindent expandtab sw=4 : */
