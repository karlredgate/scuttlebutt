
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/socket.h>
// This one does not include the additional packet socket messages
// #include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */

#include <linux/if_packet.h> /* struct tpacket_req */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

struct ring {
    int fd;
    unsigned char *buffer;
};

int
create_receiver( int index ) {
    int result;

    int sock = socket( AF_PACKET, SOCK_RAW, htons(ETH_P_ALL) );

    int version = TPACKET_V2;
    result = setsockopt( sock, SOL_PACKET, PACKET_VERSION, &version, sizeof version );

    if ( result < 0 ) {
        perror( "PKT VERSION" );
        exit( -1 );
    }

    struct sockaddr_ll address;

    memset( &address, 0, sizeof address );
    address.sll_family = AF_PACKET;
    address.sll_protocol = htons( ETH_P_ALL );
    address.sll_ifindex = index;
    address.sll_hatype = 0;
    address.sll_pkttype = 0;
    address.sll_halen = 0;

    result = bind( sock, (struct sockaddr *)&address, sizeof (struct sockaddr_ll) );

    if ( result < 0 ) {
        perror( "bind" );
        exit( -1 );
    }

    struct tpacket_req request;
    request.tp_block_size = 4096;
    request.tp_frame_size = 2048;
    request.tp_block_nr = 4;
    request.tp_frame_nr = 8;

    result = setsockopt( sock, SOL_PACKET, PACKET_RX_RING, &request, sizeof request );

    if ( result < 0 ) {
        perror( "RX_RING" );
        exit( -1 );
    }

    size_t size = request.tp_block_size * request.tp_block_nr;

    void *buffer = mmap( 0, size, PROT_READ | PROT_WRITE, MAP_SHARED, sock, 0 );

    if ( buffer == MAP_FAILED ) {
        perror( "mmap" );
        exit( -1 );
    }

    printf( "ring at %p\n", buffer );

    struct ring *ring = (struct ring *)malloc( sizeof (struct ring) );

    if ( ring == NULL ) {
        ring->fd = sock;
        ring->buffer = buffer;
    }
}

int
create_transmitter( int index ) {
    int packet_socket = socket( AF_PACKET, SOCK_RAW, 0 );
}

/* vim: set autoindent expandtab sw=4 : */
