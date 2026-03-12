#include "protocol.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

int send_header(Packet *packet, int fd)
{

    const int HEADER_BYTES = 12;

    int sent_bytes = 0;

    int header_data[3] = {packet->command_type, packet->request_id, packet->payload_len};
    char *data_as_char = (char *)header_data;

    while (sent_bytes != HEADER_BYTES)

    { // if 12 bytes are sent we are finished

        int remaining_bytes = HEADER_BYTES - sent_bytes;
        int bytes_this_call = send(fd, data_as_char, remaining_bytes, 0);

        if (bytes_this_call == -1)
        {
            perror("sent");
            return 0;
        }

        sent_bytes += bytes_this_call;
        data_as_char = data_as_char + bytes_this_call;
    }

    return 1;
}

char *recieve_bytes(int n, int fd)
{
}

Packet *assemble_packet(int fd)
{
}