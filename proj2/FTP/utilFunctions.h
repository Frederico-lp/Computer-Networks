
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdbool.h>
#include <fcntl.h>

#define START 0
#define END 1
#define FIRST_ANSWER 2
#define SEARCH_SECOND 3
#define SECOND_ANSWER 4
#define SEARCH_THIRD 5
#define GET_LAST 6

#define h_addr h_addr_list[0]	//The first address in h_addr_list.
#define FTP_SERVER_PORT 21
/**
 * The struct hostent (host entry) with its terms documented

    struct hostent {
        char *h_name;    // Official name of the host.
        char **h_aliases;    // A NULL-terminated array of alternate names for the host.
        int h_addrtype;    // The type of address being returned; usually AF_INET.
        int h_length;    // The length of the address in bytes.
        char **h_addr_list;    // A zero-terminated array of network addresses for the host.
        // Host addresses are in Network Byte Order.
    };

*/

typedef struct url{
    char user[20];
    char password[20];
    char host[20];
    char urlPath[50];

}url;

url save_arguments(char *arguments);

int establish_connection(char *ip, int port);

char *get_file_name(char *path);

off_t ask_for_file(char *file, int socket);

