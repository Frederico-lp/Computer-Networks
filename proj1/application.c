#include "application.h"

int llopen(char port,int flag) {
    appL->fileDescriptor = iniciate_connection(port, flag);
    appL->status = flag;
}


int llwrite(int fd, char * buffer, int length){
    //1º preparar buffer
    i_frame_write(fd, *buffer);
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

