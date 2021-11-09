#include "proj.h"


int llopen(char port,int flag) {
    if(TRANSMITTER)
        transmitter(port);
    else if(RECEIVER)
        reciever(port);
    else perror("invalid flag!\n");
}


int llwrite(int fd, char * buffer, int length){
    printf("teste");
    //enviar
    buffer[0] = 0x7e;
    buffer[1] = 0x03;
    buffer[2] = 0x03;
    buffer[3] = buffer[1] ^ buffer[2];
    //enviar dados
    //buf[4] = 0x7e;

    reciever
}


int llread(int fd, char * buffer){
    printf("teste");
}



int llclose(int fd){
    printf("teste");
}