#include "link.h"

unsigned int sequenceNumber = 0;   /*Número de sequência da trama: 0, 1*/
unsigned int timeout = 3;          /*Valor do temporizador: 1 s*/
unsigned int numTransmissions = 10; /*Número de tentativas em caso de falha*/

int alarmFlag = 0;
int alarmCount = 0;

void sig_handler(int signum){
 
  alarmFlag = 1;
  alarmCount++;
}

void change_sequenceNumber(){
    if(sequenceNumber)
        sequenceNumber = 0;
    else sequenceNumber = 1;
}

unsigned char * byte_stuffing(unsigned char *packet, int length){
    //so do data no i packet
    unsigned char *stuffed_packet;
    stuffed_packet = (unsigned char *)malloc(sizeof(unsigned char)* (length + 20));
    for(int i = 0; i < length; i++){
        if(packet[i] == FLAG){
            stuffed_packet[i] = 0x7d;
            stuffed_packet[i+1] = 0x5e;
            i++;
        }
        else if(packet[i] == ESCAPE_OCTET){
            stuffed_packet[i] = 0x7d;
            stuffed_packet[i+1] = 0x5d;
            i++;
        }
        else stuffed_packet[i] = packet[i];
    //printf("data = %x, stuffed data = %x\n",  packet[i], stuffed_packet[i] );
    
    }
    return stuffed_packet;

}

unsigned char* byte_destuffing(unsigned char *packet, int length){
    //a partir do data do i packet ate ao bcc2(nao inclusive)
    unsigned char * msg = malloc(sizeof(packet) * length);
    unsigned char * aux_msg = malloc(sizeof(*packet) * length);
    int j = 0;
    for(int i = 0; i < length; i++, j++){
        if(packet[i] != ESCAPE_OCTET){
            aux_msg[j] = packet[i];
        }
        if(packet[i] == ESCAPE_OCTET){
            if(packet[i+1] == 0x5e){
                aux_msg[j] = FLAG;
                i++;
            }
        }
        else if(packet[i] == ESCAPE_OCTET){
            if(packet[i+1] == 0x5d){
                aux_msg[j] = ESCAPE_OCTET;
                i++;
            }
        }
        if(packet[i] == FLAG){
            break;
        }
    }

    // for(int i = 0; i < sizeof(*aux_msg) - 1; i++){ //sizeof(*aux_msg) - 1 because bcc2
    //     msg[i] = aux_msg[i];
    // }

    return aux_msg;
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

int i_frame_write(int fd, char a, int length, unsigned char *data) {
    //bff2 before stuffing
    printf("length = %d", length);
    alarmFlag = FALSE;
    alarmCount = 0;
    unsigned char bcc2 = data[0];
    for(int i = 1; i < length; i++){
        bcc2 ^= data[i];
    }
    unsigned char *framed_data = (unsigned char*)malloc(sizeof(unsigned char) * (length + 7));
    //byte stuffing
    unsigned char *stuffed_data = byte_stuffing(data, length);
    // for(int i = 0; i<length;i++)
    //     printf("stuffed data in main = %x\n", stuffed_data[i]);

    //put stuffed data into frame
    framed_data[0] = FLAG; 
    framed_data[1] = a;  
    framed_data[2] = sequenceNumber; //sequenxe nymber!!!
    framed_data[3] = a^ sequenceNumber;
    int j = 4;

    for(int i = 0; i < length; i++){
        framed_data[j] = stuffed_data[j+i];        //começa no buf[2]
        j++;
    }
    framed_data[j+1] = bcc2;
    framed_data[j+2] = FLAG;

    printf("original bcc2 = %x\n", bcc2);
    //write frame
    int frame_length = j+2+1; //+1 bcd 0 index
    int written_length = 0;
    int state = START;
    alarmCount = 0;
    unsigned char buf[5];
    int flag = FALSE;
    printf("frame length = %d", frame_length);
    do{
            if( (written_length = write(fd, framed_data, frame_length)) < 0){
                printf("written_length = %d ", written_length);
                perror("i frame failed\n");
            }
            alarm( timeout);
            flag = FALSE;
            while(!alarmFlag && state != BCC_OK ){
                read(fd, &buf[state], 1);
                state_machine(buf, &state);
            }
            if(state == BCC_OK){
                alarm(0);
                break;
            }
            

        }
        while(alarmFlag && alarmCount <   numTransmissions);

        if(alarmCount ==   numTransmissions){
            perror("Error sending i packet, too many attempts\n");
            return -1;
        }
        else{
            printf("RR from i message recieved\n");
        }

      //sequenceNumber =  sequenceNumber ^ 1;
    return written_length;

}

unsigned char* read_i_frame(int fd){
    int state = START;
    int data_size = 0;
    unsigned char buffer;
    unsigned char *data_received = (unsigned char*)malloc(data_size);
    int all_data_received = FALSE;
    int data_couter = 0;
    int test_flag = FALSE;
    while(!all_data_received){
        if(read(fd, &buffer, 1) < 0)
            perror("failed to read i frame\n");
        else{
            //ver estado
            switch(state){

                case START:
                    if(test_flag)
                        exit(-1);
                    test_flag = TRUE;
                    printf("buffer: %x state :start\n", buffer);
                    //printf("data counter = %d\n", data_couter);
                    data_couter++;
                    if(buffer == FLAG)
                        state = FLAG_RCV;
                    break;
                case FLAG_RCV:
                    printf("buffer: %x state :flag_rcv\n", buffer);
                    // printf("data counter = %d\n", data_couter);
                    data_couter++;
                    if(buffer == 0x01 || buffer == 0x03)
                        state = A_RCV;
                    else if(buffer == 0x7e)
                        state = FLAG_RCV;
                    else state = START;  
                    break;
                case A_RCV:
                    printf("buffer: %x state :a_rcv\n", buffer);
                    // printf("data counter = %d\n", data_couter);
                    data_couter++;
                    printf("sequenceNumber = %d\n", sequenceNumber);
                    if(buffer ==  sequenceNumber)
                        state = C_RCV;
                    else if(buffer == 0x7e)
                        state = FLAG_RCV;
                    else
                        state = START;
                    break;
                case C_RCV:
                    printf("buffer: %x state :c_rcv\n", buffer);
                    // printf("data counter = %d\n", data_couter);
                    data_couter++;
                    if(buffer == 0x01 ^ sequenceNumber)
                        state = DATA;
                    else if(buffer == 0x7e)
                        state = FLAG_RCV;
                    else
                        state = START;
                    break;
                case DATA:
                    if(data_couter % 1000 == 0)
                        printf("buffer: %x state :data\n",buffer);
                    // printf("data counter = %d\n", data_couter);
                    data_couter++;
                    //sleep(1);
                    //exit(1);
                    if(buffer == FLAG){     //finished transmitting data
                        printf("received final flag!\n");
                        data_received = byte_destuffing(data_received, data_size);  //data size starts in 0

                        unsigned char bcc2 = data_received[0];
                        for(int i = 1; i < data_size; i++){
                            bcc2 ^= data_received[i];
                        }

                        unsigned char post_transmission_bcc2 = data_received[data_size];
                        printf("'original' bcc2 = %x, other bcc2 = %x\n", bcc2, post_transmission_bcc2);

                        //if(strcmp(&bcc2, &post_transmission_bcc2) == 0){
                            printf("data packet received!\n");
                            all_data_received = TRUE;
                        //}
                        // else{
                        //     perror("BCC2 dont match in llread\n");
                        //     free(data_received);
                        //     data_size = 0;
                        //     data_received = malloc(data_size);

                        // }
                    }
                    else{
                        data_size++;
                        data_received = (unsigned char*)realloc(data_received, data_size);
                        data_received[data_size - 1] = buffer;
                    }
                    break;
            }
        }
    }
    printf("receiver received packet!\n");
    if(data_received[0])   //posso fazer isto?

        su_frame_write(fd, A_R, C_RR);
    
    return data_received;
}

int iniciate_connection(char *port, int connection)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[5];
    alarmCount = 0;
    alarmFlag = FALSE;
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
            alarm(  timeout);
            flag = FALSE;
            while(!alarmFlag && state != BCC_OK ){
                read(fd, &buf[state], 1);
                state_machine(buf, &state);
            }
            if(state == BCC_OK){
                alarm(0);
                break;
            }
            

        }
        while(alarmFlag && alarmCount <   numTransmissions);

        if(alarmCount ==   numTransmissions){
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
                state_machine(buf, &state);
            }
        }
        printf("establish connection - SET recieved!\n");
        su_frame_write(fd, A_E, C_UA);
            
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
    alarmFlag = FALSE;
    int state = START;
    if(connection == TRANSMITTER){
        int flag = TRUE;
        printf("terminate connection(transmitter) starting\n");
        //re-send message if no confirmation
        //send and check if recieved DISC msg
        do{
            if(su_frame_write(*fd, A_E, C_DISC) < 0){
                sleep(3);
                perror("disc message failed\n");
            }
            alarm(  timeout);
            flag = FALSE;
            while(!alarmFlag && state != BCC_OK ){
                read(*fd, &buf[state], 1);
                state_machine(buf, &state);
            }
            if(state == BCC_OK){
                alarm(0);
                break;
            }
            

        }
        while(alarmFlag && alarmCount <   numTransmissions);

        if(alarmCount ==   numTransmissions){
            perror("Error establishing connection, too many attempts\n");
            return -1;
        }
        else{
            printf("DISC from DISC message recieved\n");
            su_frame_write(*fd, A_E, C_UA);
        }
    }

    else if(connection == RECEIVER){
        printf("terminate connection(receiver) starting\n");
        while(state != BCC_OK){
            if (read(*fd, &buf[state], 1) < 0) { // Receive SET message
            sleep(10);
                perror("Failed to read DISC message.");
            } else {
                state_machine(buf, &state);
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
                state_machine(buf, &state);
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



