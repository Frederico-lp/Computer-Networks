#include "utilFunctions.h"

url save_arguments(char *arguments){
    url parsed_url;

    if(strncmp(arguments, "ftp://", 6)){
        printf("Error reading arguments, second argument should start with ftp://\n");
        exit(-1);
    }

    arguments += 6;
    char *token;
    //check for user
    bool user = false;
    if(!strstr(arguments, ":")){
        strcpy(parsed_url.user, "anonymous");
    }
    //user exists
    else{
        user = true;
        token = strtok(arguments, ":");
        strcpy(parsed_url.user, token);
        arguments += strlen(parsed_url.user) + 1;   //+1 for :
    }

    //check for password
    if(!strstr(arguments, "@")){
        strcpy(parsed_url.password, "anonymous");
    }
    else{
        if(!user){
            printf("Error, you provided a password without a user\n");
            exit(-1);
        }
        token = strtok(arguments, "@");
        strcpy(parsed_url.password, token);
        arguments += strlen(parsed_url.password) + 1;   //+1 for @
    }

    //save host
    token = strtok(arguments, "/");
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

char *get_file_name(char *path) {
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

int parse_answer(char *pasv_answer, char *ip){
    char *temp, *slice[10];
    int i = 0;
    while( (temp = strsep(&pasv_answer,",")) != NULL ){
        slice[i] = temp;
        i++;
    }
    sprintf(ip, "%s.%s.%s.%s", slice[0], slice[1], slice[2], slice[3]);

    return atoi(slice[4]) * 256 + atoi(slice[5]);
}


int get_file_size(char* response) {
    char* s = strrchr(response, '(') + 1;
    int file_size = atoi(strtok(s, " "));

    return file_size;
}


int ask_for_file(char *urlPath, int socket){
    int cmdSize = strlen(urlPath) + 5 + 1 + 1;
    char command[cmdSize];

    sprintf(command, "retr %s\n", urlPath);

    //send the command
    if(write(socket, command, sizeof(command)) < 0){
        printf("Error asking socket for file");
        return -1;
    }

    char answer[MAX_LEN];
    int file_size;
    //read server's answer
    if(read(socket, answer, MAX_LEN) < 0){
        printf("Error reading socket answer");
        return -1;
    }
    char answer_code[4];
    sprintf(answer_code, "%c%c%c", answer[0], answer[1], answer[2]);
    if(strcmp(answer_code, "150") == 0){
        printf("%s\n", answer);
    }
    else {
        printf("Error, wrong answer from server after asking for file\n");
        exit(-1);
    }

    return 1;
}

off_t download_file(char *urlPath, int data_socket){
    char buf[MAX_LEN];
    int bytes;
    int size = 0;

    char *filename = strrchr(urlPath, '/');

    //no ocurrence
    if (filename == NULL)
        filename = urlPath;
    else
        filename += 1;

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file to write\n");
        exit(-1);
    }

    while ((bytes = read(data_socket, buf, sizeof(buf))) > 0) {
        bytes = fwrite(buf, 1, bytes, file);
        size += bytes;
    }

    printf("Download of %s finished \n", filename);
    printf("%d bytes written\n", size);

    fclose(file);

    return 0;

}