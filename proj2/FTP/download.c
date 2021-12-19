#include "utilFunctions.h"

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Invalid number of parameters.\nUsage: download ftp://[<user>:<password>@]<host>/<url-path>");
        return(-1);
    }


    //read parameters
    url parsed_url = save_arguments(argv[1]);


    //get ip
    struct hostent *h;
    if ((h = gethostbyname(argv[1])) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }
    char *ip = inet_ntoa(*((struct in_addr *) h->h_addr));


    //establish connection and save socket fd
    //The FTP protocol typically uses port 21 as its main means of communication
    //An FTP server will listen for client connections on port 21
    int sockfd = establish_connection(ip, FTP_SERVER_PORT);

    /*send a string to the server*/






    //close socket fd
    if (close(sockfd)<0) {
        perror("Error closing socket");
        exit(-1);
    }
    return 0;
}