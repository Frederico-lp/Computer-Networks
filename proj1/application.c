#include "application.h"

//applicationLayer *appL;

int sequence_number = 0;

/*
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
*/
/*
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
*/

int llopen(char *port,int flag) {
    
    // appL->fileDescriptor = iniciate_connection(port, flag);
    // appL->status = flag;
    return iniciate_connection(port, flag);
}


int llwrite(int fd, char *buffer, int length){
    //1º preparar buffer
    unsigned char **ret_buf;
    //fazer malloc
    return i_frame_write(fd, A_E, length, buffer, ret_buf);
}

unsigned char* llread(int fd){
    return read_i_frame(fd);
}

int llclose(int fd, int flag){
    return terminate_connection(&fd, flag);
}
