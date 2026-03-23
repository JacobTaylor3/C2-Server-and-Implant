#include "protocol.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

int send_header(Packet *packet, int fd)
{

    const int HEADER_BYTES = 12;

    int sent_bytes = 0;

    int header_data[3] = {packet->command_type, packet->request_id, packet->payload_len};
    char *data_as_char = (char *)header_data;

    while (sent_bytes != HEADER_BYTES) // if 12 bytes are sent we are finished

    {

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

    int recieved_bytes = 0;

    char *buffer = (char *)malloc(n);

    char *start_buffer = buffer;

    while (recieved_bytes != n)
    {

        int remaining_bytes = n - recieved_bytes;
        int bytes_this_call = recv(fd, buffer, remaining_bytes, 0);

        if (bytes_this_call == -1)
        {

            perror("recieved");
            free(start_buffer);
            return NULL;
        }

        if (bytes_this_call == 0)
        {

            printf("Connetion closed!");
            free(start_buffer);
            return NULL;
        }

        recieved_bytes += bytes_this_call;
        buffer = buffer + bytes_this_call;
    }

    return start_buffer;
}

Packet *assemble_packet(int fd)
{

    // Packet *packet = (Packet *)malloc(sizeof(Packet *));

    char *header = recieve_bytes(12, fd);

    if (header == NULL)
    {
        printf("Error creating header_packet!");
        return NULL;
    }

    Packet *header_packet = (Packet *)header;

    Command command_type = header_packet->command_type;
    int request_id = header_packet->request_id;
    int payload_len = header_packet->payload_len;
    char *payload;
    free(header);

    if (payload_len == 0)
    {

        payload = NULL;
    }
    else
    {

        payload = recieve_bytes(payload_len, fd);

        if (payload == NULL)
        {

            printf("Error creating payload_packet!");
            return NULL;
        }
    }

    Packet *packet = malloc(sizeof(Packet));

    packet->command_type = command_type;
    packet->request_id = request_id;
    packet->payload_len = payload_len;
    packet->payload = payload;

    return packet;
}