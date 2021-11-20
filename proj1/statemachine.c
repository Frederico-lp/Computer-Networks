#include "statemachine.h"

//state is the current state in the invoking function
//buf is the last char read in the invoking function
int state_machine(unsigned char buf, int *state){

        switch (*state){

                case START:
                    if(buf == 0x7e){
                        printf("buffer: %x state :%d\n",buf, *state);
                        *state++;
                    }
                    break;

                case FLAG_RCV:
                    printf("buffer: %x state :%d\n",buf, *state);
                    if(buf == 0x03 || buf == 0x01){
                        *state++;

                    }
                    else if(buf == 0x7e){
                        //state = 1;
                    }
                    else {
                        *state = START;
                        buf = 0;	//clean buffer
                    }
                    break;

                case A_RCV:
                    printf("buffer: %x state :%d\n",buf, *state);
                    if(buf == 0x03 || buf == 0x07 || buf == 0x0b 
                        || buf == 0x05  || buf == 0x01 ){ //different options for each signal
                        *state++;

                    }
                    else if(buf == 0x7e){
                        *state = FLAG_RCV;
                        buf = 0;
                    }
                    else{
                        *state = START;
                        buf = 0;
                        buf = 0;
                    }
                    break;


                case C_RCV:
                    printf("buffer: %x state :%d\n",buf, *state);
                    if( buf == (buf ^ buf) ) {
                        *state++;
                    }
                    else if(buf == 0x7e){
                        *state = FLAG_RCV;
                        buf = 0;
                    }
                    else{
                        *state = START;
                        buf = 0;
                        buf = 0;
                        buf = 0;
                    }
                    break;
            
                case BCC_OK:
                    printf("buffer: %x state :%d\n",buf, *state);
                    if(buf == 0x7e){
                        printf("FIM\n");
                        return TRUE;
                    }
                    else{
                        *state = 0;
                        buf = 0;

                    }
            }
            
    
    return FALSE;
}