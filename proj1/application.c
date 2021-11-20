#include "application.h"

int sequence_number = 0;

int create_control_packet(size_t c, size_t t1, size_t t2, 
                int file_size, char *file_name, char**packet){
    //l1 = size, l2 = name
    size_t l1;
    //then l1 is size and l2 is name
    size_t l2 = strlen(file_name);

    int packet_size = 3 + l1 + 2 + l2;  //c+t1+l1+v1+t2+l2+v2
    *packet = (unsigned char*)malloc(packet_size);
    *packet[0] = c;
    *packet[1] = t1;
    *packet[2] = l1;
    int i;
    for(i = 0; i < l1; i++){
        *packet[3+i] =  (file_size >> 8*(i+1) & 0xff);   //so o ultimo bit do numero
    }
    i+=4;
    *packet[i] = t2;
    i++;
    *packet[i] = l2;
    i++;
    for(int j = 0; i < j; i++, j++)
        *packet[i] = file_name[j];

    return packet_size;
}

int create_data_packet(size_t c, unsigned char *data, int data_size, char**packet){
    size_t l1 = data_size % 256;
    size_t l2 = (int)(data_size / 256);
    int packet_size = 4 +  data_size;
    *packet = (unsigned char*)malloc(packet_size);
    *packet[0] = c;
    *packet[1] = sequence_number;
    sequence_number++;
    *packet[2] = l2;
    *packet[3] = l1;
    for(int i = 0; i < l1; i++){
        *packet[4+i] = data[i];    
    }

    return packet_size;
}

int llopen(char *port,int flag) {
    appL.fileDescriptor = iniciate_connection(port, flag);
    appL.status = flag;
    return appL.fileDescriptor;
}


int llwrite(int fd, char *buffer, int length){
    //1º preparar buffer
    unsigned char **ret_buf;
    //fazer malloc
    return i_frame_write(fd, A_E, length, buffer, ret_buf);
}


int llread(int fd, char * buffer, char * argv){

    int res;
    struct termios oldtio, newtio;
    char buf[255];
    int stop;

    /*
        Open serial port device for reading and writing and not as controlling tty
        because we don't want to get killed if linenoise sends CTRL-C.
    */



    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 5;  /* blocking read until 5 chars received */

    /* 
        VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
        leitura do(s) pr�ximo(s) caracter(es)
    */

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");


    /* 
        O ciclo WHILE deve ser alterado de modo a respeitar o indicado no gui�o 
    */
    while (stop == FALSE)
    {                           /* loop for input */
        res = read(fd, buf, 255); /* returns after 5 chars have been input */
        buf[res] = 0;             /* so we can printf... */
        printf(":%s:%d\n", buf, res);
        if (buf[0] == 'z')
        stop = TRUE;
    }

    tcsetattr(fd, TCSANOW, &oldtio);
    close(fd);
    return 0;

    unsigned char *msg = malloc(sizeof(*buffer)), byte_read;
    int flag = 0, CURRENT_STATE = START;
    
    //while(state_machine(&byte_read, CURRENT_STATE)){
    while(!state_machine(byte_read, &CURRENT_STATE)){




        read(fd, &byte_read, 1);
    }

    //needs checking and confirmation
    
    return sizeof(*msg);
}



int llclose(int fd){
    return terminate_connection(&fd, appL.status);
}

/*---------------------------------------------------------*/

    /*
int llopen(char* port, int flag){

    fd = open(port, O_RDWR | O_NOCTTY);
    unsigned char buffer;
    int CURRENT_STATE = START;
    if (fd < 0)
    {
        perror(port);
        exit(-1);
    }

    if(flag == TRANSMITTER){
        T_SET[0] = FLAG;
        T_SET[1] = A_R;
        T_SET[2] = C_SET;
        T_SET[3] = A_R ^ C_SET;
        T_SET[4] = FLAG;

        if (tcgetattr(fd, &oldtio) == -1){ 
            perror("tcgetattr");
            exit(-1);
        }

        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;


        newtio.c_lflag = 0;

        newtio.c_cc[VTIME] = 0; 
        newtio.c_cc[VMIN] = 5;  
        tcflush(fd, TCIOFLUSH);

        if (tcsetattr(fd, TCSANOW, &newtio) == -1)
        {
            perror("tcsetattr");
            exit(-1);
        }

        printf("New termios structure set\n");

        UA = FALSE; //NEEDS CHECKING

        do {
            write(fd, T_SET, 5);
            alarm(5);
            sig_handler(0);
            write(STDOUT_FILENO, "T_SET SENT\n", 24);

            while(!UA && !alarmFlag){
                read(fd, &buffer, 1);
                CURRENT_STATE = statemachine(buffer, CURRENT_STATE);
            }

        }while(alarmFlag && alarmCount < 3); // FROM THIS DOWN IS MISSING

    }
    else if (flag == RECEIVER){

    }


}
    */