/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include "statemachine.c"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define START 0
#define FLAG_RCV 1
#define A_RCV 2
#define C_RCV 3
#define BCC_OK 4



//volatile int STOP=FALSE;

int main(int argc, char** argv)
{
	int fd,c, res;
	struct termios oldtio,newtio;
	char buf[255];

	if ( (argc < 2) ||
	((strcmp("/dev/ttyS10", argv[1])!=0) &&	//0 e dps 1q estava
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
	leitura do(s) pr髕imo(s) caracter(es)
	*/



	tcflush(fd, TCIOFLUSH);

	if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}

	printf("New termios structure set\n");

	//recieve msg
	if(state_machine(fd, buf))	//confirm
		res = write(fd,buf,5);	


	/*
	int state = 0;
	//state machine
	while (STOP==FALSE) { 
		res = read(fd,&buf[state],1); 
		printf("%d bytes read", res);
		switch (state){

			case START:
				if(buf[0] == 0x7e){
					printf("buffer: %x state :%d\n",buf[state], state);
					state++;
					continue;
				}
				break;

			case FLAG_RCV:
				printf("buffer: %x state :%d\n",buf[state], state);
				if(buf[1] == 0x03 || buf[1] == 0x01){
					state++;

					continue;
				}
				else if(buf[1] == 0x7e){
					//state = 1;
					continue;
				}
				else {
					state = START;
					buf[1] = 0;	//clean buffer
				}
				break;

			case A_RCV:
				printf("buffer: %x state :%d\n",buf[state], state);
				if(buf[2] == 0x03 || buf[2] == 0x07){
					state++;

					continue;
				}
				else if(buf[2] == 0x7e){
					state = FLAG_RCV;
					buf[2] = 0;
				}
				else{
					state = START;
					buf[2] = 0;
					buf[1] = 0;
				}
				break;


			case C_RCV:
				printf("buffer: %x state :%d\n",buf[state], state);
				if( buf[3] == (buf[1] ^ buf[2]) ) {
					printf("aqui1");
					state++;
					continue;
				}
				else if(buf[3] == 0x7e){
					printf("aqui2");
					state = FLAG_RCV;
					buf[3] = 0;
				}
				else{
					printf("aqui3");
					state = START;
					buf[3] = 0;
					buf[2] = 0;
					buf[1] = 0;
				}
				break;
		
			case BCC_OK:
				printf("buffer: %x state :%d\n",buf[state], state);
				if(buf[4] == 0x7e){
					printf("FIM\n", state);
					res = write(fd, buf, strlen(buf) + 1);
					printf("%d bytes written\n", res);
					STOP=TRUE;
				}
				else{
					state = 0;
					buf[4] = 0;
					buf[3] = 0;
					buf[2] = 0;
					buf[1] = 0;
				}
			}
		
	}
	*/

	sleep(1);
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	return 0;
}