#include "link.h"

int alarmFlag = 0;

void sig_handler(int signum){
 
  alarmFlag = 1;
}

void byte_stuffing(unsigned char *packet, unsigned char *stuffed_packet){
    //fazer do bcc2 tb

    for(int i = 0; i < sizeof(*packet);i++){
        if(packet[i] == FLAG){
            stuffed_packet[i] = 0x7d;
            stuffed_packet[i+1] = 0x5d;
            i++;
        }
        else if(packet[i] == ESCAPE_OCETET){
            stuffed_packet[i] = 0x7d;
            stuffed_packet[i+1] = 0x5d;
            i++;
        }
        else stuffed_packet[i] = packet[i];
    }

}

void byte_destuffing(unsigned char *packet, unsigned char *destuffed_packet){
    for(int i = 0; i < sizeof(*packet); i++){

    }
}



int su_frame_write(int fd, char a, char c) {
    unsigned char buf[5];

    buf[0] = FLAG;  
    buf[1] = a;     
    buf[2] = c;     
    buf[3] = a ^ c; 
    buf[4] = FLAG;  

    return write(fd, buf, 5);
}

int i_frame_write(int fd, char a, char c) {
    unsigned char *buf;
    buf[0] = FLAG; 
    buf[1] = a;  
    buf[2] = c;
}

int iniciate_connection(char *port, int connection)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[5];
    int i, sum = 0, speed = 0;

    (void) signal(SIGALRM, sig_handler);    //Register signal handler


    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */

    if(fd == NULL)
        fd = open(port, O_RDWR | O_NOCTTY );
    if (fd <0) {perror(port); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 1; /* blocking read until 5 chars received */



    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) próximo(s) caracter(es)
    */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
    }

    printf("New termios structure set\n");


    int state = START;
    if(connection == TRANSMITTER){
        int flag = TRUE;

        //re-send message if no confirmation
        for(int transmissions = 0; transmissions < 3; transmissions++){
            //esta a incrementar?
            if(flag){
                alarm(linkL->timeout);
                flag = FALSE;

                if(su_frame_write(fd, A_E, C_SET) < 0){
                    perror("set message failed\n");
                    exit(-1);
                }
            }

            if(read(fd, buf[state], 1) < 0){   //check if any byte of ua was recieved
                if(alarmFlag){
                    perror("alarm timeout\n");
                    exit(-1);
                }
                    
            }
            else{
                if(state_machine(buf, &state)){
                    alarm(0);   
                    printf("recieved UA\n");
                    break;
                }
                
            }
        
        }

        perror("Error establishing connection, too many attempts\n");
    }

    else if(connection == RECEIVER){
        int full_message = FALSE;
        while(!full_message){
            if (read(fd, buf[state], 1) < 0) { // Receive SET message
                perror("Failed to read SET message.");
                exit(1);
            } else {
                if(state_machine(buf, &state))
                    full_message = TRUE;
            }
        }
        printf("UA recieved!\n");
            
    }
    else {
        printf("invalid type of connection!\n");
        exit(-1);
    }
    return fd;
    
}

int terminate_connection(int *fd, int connection)
{
    char buf[5];
    int state = START;
    if(connection == TRANSMITTER){
        int flag = TRUE;

        //re-send message if no confirmation
        for(int transmissions = 0; transmissions < 3; transmissions++){
            //esta a incrementar?
            if(flag){
                alarm(linkL->timeout);
                flag = FALSE;

                if(su_frame_write(fd, A_E, C_DISC) < 0){    //write
                    perror("disc message failed\n");
                    exit(-1);
                }
            }

            if(read(fd, buf[state], 1) < 0){   //read
                if(alarmFlag){
                    perror("alarm timeout\n");
                    exit(-1);
                }
                    
            }
            else{
                if(state_machine(buf, &state)){ //if read all the bytes
                    alarm(0);   
                    printf("recieved disc\n");
                    if(su_frame_write(fd, A_E, C_DISC) < 0){    //write
                        perror("ua message failed\n");
                        exit(-1);
                    }
                    break;
                }
                
            }
        
        }

        perror("Error establishing connection, too many attempts\n");
    }

    else if(connection == RECEIVER){
        int full_message = FALSE;
        while(!full_message){
            if (read(fd, buf[state], 1) < 0) { // Receive SET message
                perror("Failed to read SET message.");
                exit(1);
            } else {
                if(state_machine(buf, &state))
                    full_message = TRUE;
            }
        }
        printf("DISC recieved!\n");

        if(su_frame_write(fd, A_E, C_DISC) < 0){    //write
            perror("ua message failed\n");
            exit(-1);
        }

        full_message = FALSE;
        while(!full_message){
            if (read(fd, buf[state], 1) < 0) { // Receive UA message
                perror("Failed to read SET message.");
                exit(1);
            } else {
                if(state_machine(buf, &state))
                    full_message = TRUE;
            }
        }
        printf("UA recieved!\n");
            
    }
    else {
        printf("invalid type of connection!\n");
        exit(-1);
    }

    return 1;

}



