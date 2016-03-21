
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
#include <poll.h>
#include <string.h>

/*
 * calculate block address from base time block number
 * tpacket2_hdr *packet = (tpacket2_hdr *) (base * block);
 */
struct ring {
    unsigned char *base;
    int fd;
    int blocks;
    int frames;
    int block_size;
    int frame_size;
};

struct ring *
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

    printf( "bind to index %d\n", index );

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
        perror( "malloc" );
        exit( -1 );
    }
    ring->fd = sock;
    ring->base = buffer;
    ring->block_size = 4096;
    ring->frame_size = 2048;
    ring->blocks = 4;
    ring->frames = 8;

    return ring;
}

int
create_transmitter( int index ) {
    int packet_socket = socket( AF_PACKET, SOCK_RAW, 0 );
}

void
receive_loop( struct ring *ring ) {
    printf("receive loop with block count = %d\n", ring->blocks );

    struct pollfd io;
    io.fd = ring->fd;
    io.events = POLLIN | POLLERR;
    io.revents = 0;

    int frame = 0;
    int block = 0;

    while (1) {
        poll( &io, 1, 5 );
        printf("receive\n" );

        block = (block + 1) % ring->blocks;
        printf( "check block %d\n", block );
        void *block_address = ring->base + (ring->block_size * block);
        printf( "block address is %p\n", block_address );
        struct tpacket2_hdr *packet = (struct tpacket2_hdr *) block_address;
    }
}

/* vim: set autoindent expandtab sw=4 : */
