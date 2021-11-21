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



unsigned char* llread(int fd){
    return read_i_frame(fd);
}



int llclose(int fd){
    return terminate_connection(&fd, appL.status);
}



    /*
int llread(int fd, char * buffer, char * argv){

    int res;
    char buf[255];
    int stop;

     
    //    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no gui�o 
    
    while (stop == FALSE)
    {                           
        res = read(fd, buf, 5);
        buf[res] = 0;            
        printf(":%s:%d\n", buf, res);
        if (buf[0] == 'z') stop = TRUE;
    }


    unsigned char *msg = malloc(sizeof(*buffer)), bytes_read;
    int flag = 0, CURRENT_STATE = START;
    
    //while(state_machine(&bytes_read, CURRENT_STATE)){
    while(!state_machine(bytes_read, &CURRENT_STATE)){
        read(fd, &bytes_read, 32); // reads 32 bytes
    }

    //needs checking and confirmation
    
    return sizeof(*msg);
}
    */