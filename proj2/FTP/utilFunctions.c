#include "utilFunctions.h"

url save_arguments(char *arguments){
    //fazer debbuging dos strtok depois
    url parsed_url;
    if(strncmp(arguments, "ftp://", 6) != 0){
        perror("Error reading arguments, second argument should start with ftp://\n");
        exit(-1);
    }
    arguments += 6;
    char *token;
    //check for user
    bool user = false;
    if(!strstr(arguments, ':')){
        strcpy(parsed_url.user, "anonymous");
    }
    //user exists
    else{
        user = true;
        token = strtok(arguments, ':');
        strcpy(parsed_url.user, token);
        arguments += strlen(parsed_url.user) + 1;   //+1 for :
    }

    //check for password
    if(!strstr(arguments, '@')){
        strcpy(parsed_url.password, "anonymous");
    }
    else{
        if(!user){
            perror("Error, you provided a password without a user\n");
            exit(-1);
        }
        token = strtok(arguments, '@');
        strcpy(parsed_url.user, token);
        arguments += strlen(parsed_url.password) + 1;   //+1 for @
    }

    //save host
    token = strtok(arguments, '/');
    strcpy(parsed_url.host, token);
    arguments += strlen(parsed_url.host) + 1;   //+1 for /

    //save url-path
    strcpy(parsed_url.urlPath, arguments);

    return parsed_url;
}



int establish_connection(char *ip, int port){
    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }
    /*connect to the server*/
    if (connect(sockfd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }

    return sockfd;
}

char *getFileName(char *path) {
    char * file_name = malloc(50);

    int last_index = 0;
    for (int i = 0; i < strlen(path); i++) {
        if (path[i] == '/') {
            last_index = i+1;
        }
    }

    int j = 0;
    for (int i = last_index; i < strlen(path); i++, j++) {
        file_name[j] = path[i];
    }

    return file_name;
}