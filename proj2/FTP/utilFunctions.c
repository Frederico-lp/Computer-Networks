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


off_t ask_for_file(char *file, int socket){
    int cmdSize = strlen(file) + 5 + 1 + 1;
    char command[cmdSize];
    int strSize = strlen(file) + BUFFER_SIZE;
    char answer[strSize];
    char *expectedAnswerPrefix = "150 Opening BINARY mode data connection for ";
    int cmpSize = strlen(expectedAnswerPrefix) + strlen(file) + 1;
    char expectedAnswer[cmpSize];

    snprintf(command, cmdSize, "RETR %s\n", file);

    //socketWrite(socket, command);
    write(socket, command, strlen(command));

    //socketRead(socket, answer, strSize);
    read(socket, answer, strSize);

    snprintf(expectedAnswer, cmpSize, "%s%s", expectedAnswerPrefix, file);

    if (strncmp(answer, expectedAnswer, cmpSize - 1))
    {
      fprintf(stderr, "Wrong RETR answer: %s", answer);
      exit(-1);
    }

    //falta isto do get file sizw tmb
    off_t fileSize = getFileSize(answer, strlen(expectedAnswer) + 1);
    return fileSize;
}

int download_file(char *file, int socket, off_t file_size){
    char buf[MAX_LEN];
    int bytes;
    float total_bytes = 0;

    // char* filename = strrchr(file, '/');

    // if (filename == NULL)
    //     filename = file;
    // else
    //     filename += 1;

    // FILE* file = fopen(filename, "w");
    // if (file == NULL) {
    //     printf("ERROR: Failed to open file.\n");
    //     exit(1);
    // }

    printf("Download starting %s\n", file);

    while ((bytes = read(socket, buf, sizeof(buf))) > 0) {
        bytes = fwrite(buf, 1, bytes, file);
        total_bytes += bytes;
    }

    printf("Download finished %s\n", file);

    fclose(file);

    return 0;

}