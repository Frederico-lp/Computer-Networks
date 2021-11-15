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
    

}



int llclose(int fd){
    return terminate_connection(fd, appL->status);
}



void byte_stuffing(unsigned char *packet, unsigned char *stuffed_packet){

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

