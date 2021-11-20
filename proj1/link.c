#include "link.h"


//NOTA: adicionar variavel para while n continuar dps de chegar ao fim do state machine
//Verificar o terminate connection, tem coisas mal

int alarmFlag = 0;
int alarmCount = 0;
int sequenceNumber = 0;

void sig_handler(int signum){
 
  alarmFlag = 1;
  alarmCount++;
}

void byte_stuffing(unsigned char *packet, unsigned char **stuffed_packet, int length){

    for(int i = 0; i < length; i++){
        if(packet[i] == FLAG){
            *stuffed_packet[i] = 0x7d;
            *stuffed_packet[i+1] = 0x5e;
            i++;
        }
        else if(packet[i] == ESCAPE_OCTET){
            *stuffed_packet[i] = 0x7d;
            *stuffed_packet[i+1] = 0x5d;
            i++;
        }
        else *stuffed_packet[i] = packet[i];
    }

}

unsigned char* byte_destuffing(unsigned char *packet){

    unsigned char * msg = malloc(sizeof(*packet));
    unsigned char * aux_msg = malloc(sizeof(*packet));

    for(int i = 4; i < sizeof(*packet); i++){ //i=4 ignores the first 5 bytes that correspond to ESCAPE_OCTET, 0x5e, A_R, C, BCC1
        if(packet[i] != ESCAPE_OCTET){
            aux_msg[i] = packet[i];
        }
        if(packet[i] == ESCAPE_OCTET){
            if(packet[i+1] == 0x5e){
                aux_msg[i] = FLAG;
            }
        }
        if(packet[i] == FLAG){
            break;
        }
    }

    for(int i = 0; i < sizeof(*aux_msg) - 1; i++){ //sizeof(*aux_msg) - 1 because bcc2
        msg[i] = aux_msg[i];
    }

    return msg;
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

int i_frame_write(int fd, char a, int length, unsigned char *data, unsigned char **ret_buf) {
    //bff2 before stuffing
    unsigned char bcc2 = data[0];
    for(int i = 1; i < length; i++){
        bcc2 ^= data[i];
    }

    //byte stuffing
    unsigned char *stuffed_data;
    byte_stuffing(data, &stuffed_data, length);

    //put stuffed data into frame
    *ret_buf[0] = FLAG; 
    *ret_buf[1] = a;  
    *ret_buf[2] = linkL.sequenceNumber; //sequenxe nymber!!!
    *ret_buf[3] = a^linkL.sequenceNumber;
    int j = 4;

    for(int i = 0; i < length; i++, j++){
        *ret_buf[j] = data[i];          //começa no buf[2]
    }
    *ret_buf[j+1] = bcc2;
    *ret_buf[j+2] = FLAG;
    
    //write frame
    int written_length = 0;
    int state = START;
    alarmCount = 0;
    unsigned char buf[5];
    int flag = TRUE;

    while(alarmCount < linkL.numTransmissions){
        if(flag){
            alarm(linkL.timeout);
            flag = FALSE;

            if( (written_length = write(fd, ret_buf, j+2)) < 0){
                perror("i message failed\n");
            }
        }

        if(read(fd, &buf[state], 1) < 0){   //check if any byte of ua was recieved
            if(alarmFlag){
                perror("alarm timeout\n");
                break;
            }
                
        }
        else{
            if(state_machine(buf[state], &state)){
                alarm(0);   
                printf("recieved UA\n");
                break;
            }
            
        }
        
        
    }





    linkL.sequenceNumber = linkL.sequenceNumber ^ 1;
    return write(fd, ret_buf, j + 3);
}

int iniciate_connection(char *port, int connection)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[5];
    alarmCount = 0;
    alarmFlag = 0;
    int i, sum = 0, speed = 0;

    (void) signal(SIGALRM, sig_handler);    //Register signal handler


    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */

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
        do{
            if(su_frame_write(fd, A_E, C_SET) < 0){
                perror("set message failed\n");
            }
            alarm(linkL.timeout);
            flag = FALSE;
            while(alarmFlag && state != BCC_OK ){
                read(fd, &buf[state], 1);
                state_machine(buf[state], &state);
            }
            if(state == BCC_OK)
                break;
            

        }
        while(!alarmFlag && alarmCount < linkL.numTransmissions);

        if(alarmCount == linkL.numTransmissions){
            perror("Error establishing connection, too many attempts\n");
            return -1;
        }
        else(printf("UA from SET message recieved\n"));

    }

    else if(connection == RECEIVER){
        while(state != BCC_OK){
            if (read(fd, &buf[state], 1) < 0) { // Receive SET message
                perror("Failed to read SET message.");
            } else {
                state_machine(buf[state], &state);
            }
        }
        printf("UA recieved!\n");
            
    }
    else {
        printf("invalid type of connection!\n");
        return -1;
    }
    return fd;
    
}

int terminate_connection(int *fd, int connection)
{
    char buf[5];
    alarmCount = 0;
    alarmFlag = 0;
    int state = START;
    if(connection == TRANSMITTER){
        int flag = TRUE;

        //re-send message if no confirmation
        //send and check if recieved DISC msg
        do{
            if(su_frame_write(*fd, A_E, C_DISC) < 0){
                perror("disc message failed\n");
            }
            alarm(linkL.timeout);
            flag = FALSE;
            while(alarmFlag && state != BCC_OK ){
                read(*fd, &buf[state], 1);
                state_machine(buf[state], &state);
            }
            if(state == BCC_OK)
                break;
            

        }
        while(!alarmFlag && alarmCount < linkL.numTransmissions);

        if(alarmCount == linkL.numTransmissions){
            perror("Error establishing connection, too many attempts\n");
            return -1;
        }
        else{
            printf("UA from SET message recieved\n");
            su_frame_write(*fd, A_E, C_UA);
        }
    }

    else if(connection == RECEIVER){
        while(state != BCC_OK){
            if (read(*fd, &buf[state], 1) < 0) { // Receive SET message
                perror("Failed to read DISC message.");
            } else {
                state_machine(buf[state], &state);
            }
        }
        printf("DISC recieved!\n");

        if(su_frame_write(*fd, A_E, C_DISC) < 0){    //write
            perror("ua message failed\n");
            return -1;
        }

        state = START;
        while(state != BCC_OK){
            if (read(*fd, &buf[state], 1) < 0) { // Receive UA message
                perror("Failed to read SET message.");
            } else {
                state_machine(buf[state], &state);
            }
        }
        printf("UA recieved!\n");
            
    }
    else {
        printf("invalid type of connection!\n");
        return -1;
    }

    return 1;

}



