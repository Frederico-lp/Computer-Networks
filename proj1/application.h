#include "link.h"

#define ESCAPE_OCTET   0X7d

#define C_DATA  0
#define C_START 1
#define C_END   2

#define TRANSMITTER 1
#define RECEIVER 0

#define BAUDRATE B38400
#define MODEMDEVICE_0 "/dev/ttyS0"
#define MODEMDEVICE_1 "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FLAG 0x7e

/*
unsigned char T_SET[5];
unsigned char T_UA[5];
unsigned char T_DISC[5];
struct termios oldtio, newtio;
int fd;

int alarmFlag;
int alarmCount = 0;
int sequenceNumber = 0;
*/

/*
void sig_handler(int signum){
  alarmFlag = signum;
  alarmCount++;
}
*/




/*-------------------------------------------------------------------------------*/

typedef struct {
int fileDescriptor; /*Descritor correspondente à porta série*/
int status;         /*TRANSMITTER | RECEIVER*/
} applicationLayer;



applicationLayer appL;

int create_control_packet(size_t c, size_t t1, size_t t2, int file_size, char *file_name, char**packet);

int create_data_packet(size_t c, unsigned char *data, int data_size, char**packet);

int llopen(char *port,int flag);

int llwrite(int fd, char * buffer, int length);

unsigned char* llread(int fd);

int llclose(int fd);

void create_packet();

//int llread(int fd, char * buffer, char * argv);
