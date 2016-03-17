
#include <sys/types.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */

int main( int argc, char **argv ) {
    int socket_type = SOCK_RAW;
    int protocol = 0;

    int packet_socket = socket(AF_PACKET, socket_type, protocol);
}

/* vim: set autoindent expandtab sw=4 : */
