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
}


int llread(int fd, char * buffer){
    printf("teste");
}



int llclose(int fd){
    printf("teste");
}