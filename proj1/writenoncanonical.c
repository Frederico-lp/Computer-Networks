/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "statemachine.c"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;


int main(int argc, char** argv)
{
  int fd,c, res;
  struct termios oldtio,newtio;
  char buf[255];
  int i, sum = 0, speed = 0;

  if ( (argc < 2) ||
  ((strcmp("/dev/ttyS10", argv[1])!=0) &&
  (strcmp("/dev/ttyS11", argv[1])!=0) )) {
  printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
  exit(1);
  }


  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */


  fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd <0) {perror(argv[1]); exit(-1); }

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
  perror("tcgetattr");
  exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 1; /* blocking read until 5 chars received */



  /*
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
  leitura do(s) próximo(s) caracter(es)
  */



  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
  perror("tcsetattr");
  exit(-1);
  }

  printf("New termios structure set\n");


  
  //send msg
  buf[0] = 0x7e;
  buf[1] = 0x03;
  buf[2] = 0x03;
  buf[3] = buf[1] ^ buf[2];
  buf[4] = 0x7e;

  int flag = FALSE;

  //re-send message if no confirmation
  for(int k = 0; k < 3; k++){
    res = write(fd,buf,5);
    if(state_machine(fd, buf))
      break;
  }

  //(void) signal(SIGALRM, atende);  



  sleep(1);

  if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
  perror("tcsetattr");
  exit(-1);
  }




  close(fd);
  return 0;
}