
typedef enum
{

    COMMAND_HELLO = 0,
    COMMAND_HEARTBEAT = 1,
    COMMAND_SET_SLEEP = 2,
    COMMAND_SHUTDOWN = 3,
    COMMAND_READ_DATA = 4,
    COMMAND_WRITE_DATA = 5,
    COMMAND_RUN_CMD = 6,
    COMMAND_ERROR = 7

} Command;

typedef struct
{
    Command command_type;

    int request_id;

    int payload_len;

    char *payload; // Only commands WRITE_DATA, RUN_CMD, ERROR, will have the payload field with relevent data

} Packet;


int send_header(Packet * packet, int fd); //sends the first 12 bytes of the headers value

char * recieve_bytes(int n,int fd); // function that loops through the recieve function and waits until it has recieved n bytes

Packet * assemble_packet(int fd); //assembles the packet, into a Packet * type


