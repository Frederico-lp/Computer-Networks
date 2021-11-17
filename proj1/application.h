#include "link.h"

#define ESCAPE_OCTET   0X7d

#define C_DATA  0
#define C_START 1
#define C_END   2


typedef struct {
int fileDescriptor; /*Descritor correspondente à porta série*/
int status;         /*TRANSMITTER | RECEIVER*/
} applicationLayer;



applicationLayer *appL;

int create_control_packet(size_t c, size_t t1, size_t t2, int file_size, char *file_name, char**packet);

int create_data_packet(size_t c, unsigned char *data, int data_size, char**packet);

int llopen(char port,int flag);

int llwrite(int fd, char * buffer, int length);

int llread(int fd, char * buffer);

int llclose(int fd);

void create_packet();
