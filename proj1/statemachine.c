#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>


#define FALSE 0
#define TRUE 1
#define START 0
#define FLAG_RCV 1
#define A_RCV 2
#define C_RCV 3
#define BCC_OK 4

volatile int STOP=FALSE;



int state_machine(char *buf, int *state){

        switch (*state){

                case START:
                    if(buf[0] == 0x7e){
                        printf("buffer: %x state :%d\n",buf[*state], state);
                        state++;
                    }
                    break;

                case FLAG_RCV:
                    printf("buffer: %x state :%d\n",buf[*state], state);
                    if(buf[1] == 0x03 || buf[1] == 0x01){
                        state++;

                    }
                    else if(buf[1] == 0x7e){
                        //state = 1;
                    }
                    else {
                        state = START;
                        buf[1] = 0;	//clean buffer
                    }
                    break;

                case A_RCV:
                    printf("buffer: %x state :%d\n",buf[*state], state);
                    if(buf[2] == 0x03 || buf[2] == 0x07 || buf[2] == 0x0b 
                        || buf[2] == 0x05  || buf[2] == 0x01 ){ //different options for each signal
                        state++;

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
                    printf("buffer: %x state :%d\n",buf[*state], state);
                    if( buf[3] == (buf[1] ^ buf[2]) ) {
                        state++;
                    }
                    else if(buf[3] == 0x7e){
                        state = FLAG_RCV;
                        buf[3] = 0;
                    }
                    else{
                        state = START;
                        buf[3] = 0;
                        buf[2] = 0;
                        buf[1] = 0;
                    }
                    break;
            
                case BCC_OK:
                    printf("buffer: %x state :%d\n",buf[*state], state);
                    if(buf[4] == 0x7e){
                        printf("FIM\n", state);
                        return TRUE;
                    }
                    else{
                        state = 0;
                        buf[4] = 0;
                        buf[3] = 0;
                        buf[2] = 0;
                        buf[1] = 0;
                    }
            }
            
    
    return FALSE;
}