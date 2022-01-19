#include "utilFunctions.h"

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Invalid number of parameters.\nUsage: download ftp://[<user>:<password>@]<host>/<url-path>");
        return(-1);
    }

    //read parameters
    url parsed_url = save_arguments(argv[1]);

    printf("user: %s\n", parsed_url.user);
    printf("password: %s\n", parsed_url.password);
    printf("host: %s\n", parsed_url.host);
    printf("url path: %s\n", parsed_url.urlPath);

    //get ip
    struct hostent *h;
    if ((h = gethostbyname(parsed_url.host)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }
    char *ip = inet_ntoa(*((struct in_addr *) h->h_addr));

    printf("IP: %s\n", ip);


    //establish connection and save socket fd
    int sockfd = establish_connection(ip, FTP_SERVER_PORT);

    // ftp://[<user>:<password>@]<host>/<url-path>
    /* We will have 3 possible cases, covered on function save_arguments():
    * 1 - User and Password provided 
    * 2 - User provided
    * 3 - None provided 
    */

    char *msg[3];
    // user <user>
    char *final_user = malloc(strlen("user ") + strlen(parsed_url.user) + 1);
    // pass <password>
    char *final_password = malloc(strlen("pass ") + strlen(parsed_url.password) + 1);

    strcpy(final_user, "user ");
    strcat(final_user, parsed_url.user);
    strcpy(final_password, "pass ");
    strcat(final_password, parsed_url.password);

    msg[0] = final_user;
    msg[1] = final_password;
    msg[2] = "pasv\n"; // passive mode

    printf("msg[0]: %s\n", msg[0]);
    printf("msg[1]: %s\n", msg[1]);
    printf("msg[2]: %s\n", msg[2]);

    char buf = ' ';
    //ip and port of server answer
    char pasv_answer[50];

    // User writing
    write(sockfd, msg[0], strlen(msg[0]));
    write(sockfd, "\n", 1);
    
    printf(">%s\n", msg[0]);

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
                    if(buf == '1') state = WRITE_PASSWORD;
                    //331 Please specify password.
                }
            }
            break;
        
        case WRITE_PASSWORD:
            if(buf == '\n'){
                // Password writing
                write(sockfd, msg[1], strlen(msg[1]));
                write(sockfd, "\n", 1);
                
                printf(">%s\n", msg[1]);
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
                    if(buf == '0') state = WRITE_PASV;
                    //230 Login successful.
                }
            }
            break;

        case WRITE_PASV:
            if(buf == '\n'){
                // Pasv writing
                write(sockfd, msg[2], strlen(msg[2]));
                write(sockfd, "\n", 1);
                
                printf(">%s\n", msg[2]);
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
                    //Entering Passive Mode ()
                }
            }
            break;

        case GET_LAST:

            if (buf == '(') {
                int i = 0;
                read(sockfd, &buf, 1);
                write(STDOUT_FILENO, &buf, 1);

                //read value of server answer after pasv command
                do {
                   pasv_answer[i] = buf;
                   i++;
                   read(sockfd, &buf, 1); 
                   write(STDOUT_FILENO, &buf, 1);
                } while (buf != ')');

                //printf until \n, flush?
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


    char dataIP[50];
    int dataPort = parse_answer(pasv_answer, dataIP);
    printf("Port: %d\n", dataPort);
    printf("IP: %s\n", dataIP);


    int data_sockfd = establish_connection(dataIP, dataPort);



    ask_for_file(parsed_url.urlPath, sockfd);


    download_file(parsed_url.urlPath, data_sockfd);


    //close socket fd
    if (close(sockfd)<0) {
        perror("Error closing socket");
        exit(-1);
    }
    return 0;
}