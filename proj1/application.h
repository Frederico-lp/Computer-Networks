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

#define SET     0
#define DISC    1
#define UA      0
#define RR      1
#define REJ     2
#define A_R     0x03    //Comandos enviados pelo Emissor e Respostas enviadas pelo Receptor
#define A_E     0x01    //Comandos enviados pelo Receptor e Respostas enviadas pelo Emissor
#define C_SET   0x03
#define C_DISC  0x0b
#define C_UA    0x07
#define C_RR    0x05
#define C_REJ   0x01
#define C_0     0x00
#define C_1     0x40

unsigned char T_SET[5];
unsigned char T_UA[5];
unsigned char T_DISC[5];
struct termios oldtio, newtio;
int fd;

int alarmFlag;
int alarmCount = 0;
int sequenceNumber = 0;

void sig_handler(int signum){
  alarmFlag = signum;
  alarmCount++;
}





/*-------------------------------------------------------------------------------*/

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

