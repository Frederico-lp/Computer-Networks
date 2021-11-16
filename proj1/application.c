#include "application.h"

int llopen(char port,int flag) {
    appL->fileDescriptor = iniciate_connection(port, flag);
    appL->status = flag;
}


int llwrite(int fd, char * buffer, int length){
    printf("teste");
    //enviar
    buffer[0] = 0x7e;
    buffer[1] = 0x03;
    buffer[2] = 0x03;
    buffer[3] = buffer[1] ^ buffer[2];

    
    //enviar dados
    //APLICAÇAO
    //buf[4] = 0x7e;

    reciever(0, fd);
}


int llread(int fd, char * buffer){

    int length = sizeof(*buffer);
    
    for (int i = 0; i < length; i++){
        if(buffer[i] == FLAG){
            //byte_destuffing()
        }
        
    }

    return 0;

}



int llclose(int fd){
    return terminate_connection(fd, appL->status);
}

