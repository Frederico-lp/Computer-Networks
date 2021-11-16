#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "statemachine.c"


#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
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

#define TRANSMITTER 1
#define RECEIVER 0

#define ESCAPE_OCETET   0X7d

#define MAX_SIZE 50

typedef struct {
char port[20];                 /*Dispositivo /dev/ttySx, x = 0, 1*/
int baudRate;                  /*Velocidade de transmissão*/
unsigned int sequenceNumber;   /*Número de sequência da trama: 0, 1*/
unsigned int timeout;          /*Valor do temporizador: 1 s*/
unsigned int numTransmissions; /*Número de tentativas em caso de falha*/
char frame[MAX_SIZE];          /*Trama*/
} linkLayer;

linkLayer *linkL;   //tem de estar no .h para poder inicializar no main

void byte_stuffing(unsigned char *packet, unsigned char *stuffed_packet);

void byte_destuffing(unsigned char *packet, unsigned char *destuffed_packet);

void sig_handler(int signum);

int su_frame_write(int fd, char a, char c);

int iniciate_connection(char *port, int connection);

int terminate_connection(int *fd, int connection);