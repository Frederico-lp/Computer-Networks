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
    int sockfd = establish_connection(ip, FTP_SERVER_PORT);

    // ftp://[<user>:<password>@]<host>/<url-path>
    /* We will have 4 possible cases, covered on function save_arguments():
    * 1 - User and Password provided 
    * 2 - User provided
    * 3 - Password provided
    * 4 - None provided 
    */

    char *msg[3];
    char *final_user = malloc(strlen("user ") + strlen(parsed_url.user) + 1);
    char *final_password = malloc(strlen("password ") + strlen(parsed_url.password) + 1);
    strcpy(final_user, "user ");
    strcat(final_user, parsed_url.user);
    strcpy(final_password, "password ");
    strcat(final_password, parsed_url.password);
    msg[0] = final_user;
    msg[1] = final_password;
    msg[2] = "pasv\n"; // passive mode

    char buf = ' ';

    // User writing
    write(sockfd, msg[0], strlen(msg[0]));
    write(sockfd, "\n", 1);
    
    write(STDOUT_FILENO, ">", 1);
    write(STDOUT_FILENO, msg[0], strlen(msg[0]));
    write(STDOUT_FILENO, "\n", 1);

    int state = START;

    while(state != END){
        read(sockfd,&buf, 1);
        write(STDOUT_FILENO, &buf, 1);

        switch (state)
        {
        case START:
            if(buf == '3') {
                read(sockfd,&buf, 1);
                write(STDOUT_FILENO, &buf, 1);
                if(buf == '3'){
                    read(sockfd,&buf, 1);
                    write(STDOUT_FILENO, &buf, 1);
                    if(buf == '1') state = FIRST_ANSWER;
                }
            }
            break;
        
        case FIRST_ANSWER:
            if(buf == '\n'){
                // Password writing
                write(sockfd, msg[1], strlen(msg[1]));
                write(sockfd, "\n", 1);
                
                write(STDOUT_FILENO, ">", 1);
                write(STDOUT_FILENO, msg[1], strlen(msg[1]));
                write(STDOUT_FILENO, "\n", 1);
                state = SEARCH_SECOND;
            }
            break;

        case SEARCH_SECOND:
            if(buf == '2') {
                read(sockfd,&buf, 1);
                write(STDOUT_FILENO, &buf, 1);
                if(buf == '3'){
                    read(sockfd,&buf, 1);
                    write(STDOUT_FILENO, &buf, 1);
                    if(buf == '0') state = SECOND_ANSWER;
                }
            }
            break;

        case SECOND_ANSWER:
            if(buf == '\n'){
                // Pasv writing
                write(sockfd, msg[2], strlen(msg[2]));
                write(sockfd, "\n", 1);
                
                write(STDOUT_FILENO, ">", 1);
                write(STDOUT_FILENO, msg[2], strlen(msg[2]));
                write(STDOUT_FILENO, "\n", 1);
                state = SEARCH_THIRD;
            }
            break;

        case SEARCH_THIRD:
            if(buf == '2') {
                read(sockfd,&buf, 1);
                write(STDOUT_FILENO, &buf, 1);
                if(buf == '2'){
                    read(sockfd,&buf, 1);
                    write(STDOUT_FILENO, &buf, 1);
                    if(buf == '7') state = GET_LAST;
                }
            }
            break;

        case GET_LAST:
            if (buf == '(') {
                int i = 0;
                read(sockfd, &buf, 1);
                write(STDOUT_FILENO, &buf, 1);
                do {
                   /*
                   reads server port ??
                   */
                   read(sockfd, &buf, 1); 
                   write(STDOUT_FILENO, &buf, 1);
                } while (buf != ')');
                while(buf != '\n') {
                    read(sockfd, &buf, 1);    
                    write(STDOUT_FILENO, &buf, 1);
                }
                state = END;
            }
            break;
        default:
            break;
        }
    }

    // TO-DO : Send filepath to server

    char * file_name = getFileName(*parsed_url.urlPath);
    int file = open(file_name, O_CREAT | O_WRONLY, 0777);
    
    char file_buf[256];
    int bytes;

    // write the file 

    //The FTP protocol typically uses port 21 as its main means of communication
    //An FTP server will listen for client connections on port 21

    /*send a string to the server*/




    close(file);

    //close socket fd
    if (close(sockfd)<0) {
        perror("Error closing socket");
        exit(-1);
    }
    return 0;
}