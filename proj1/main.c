#include "application.h"


/*
FALTA:
-control[1] = 0; // N o q e isto
-mandar alguma coisa no llwrite se der timeout?
-corrigir warning
-ver assemble pic e create_control_packet e create_control_packet, eu tinha feito os ultimos
dois e penso q um deles e parecido com o assemble pic que fizeste ------THIS
-escrever o q se recebe no novo ficheiro? (main) ------THIS DONE
-aquilo do struct termios oldtio, newtio; esta repetido varias vezes, eu ja o tinha posto
no llopen, é mm necessario estar no main tmb?
-ver estruturas de dados, diz q as estou a criar a mesma varias vezes(parte do ponto 1)
-receiver penso q so esta a receber um packet, no main é preciso criar varios (ver linkL.maxSize) ------THIS
*/

unsigned char * process_pic(char* path, int* size){
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long int length = ftell(f); //qts bytes tem o ficheiro
    unsigned char *data = (unsigned char*)malloc(length+4);
    unsigned char *buffer = (unsigned char *)malloc(length);

    fseek(f, 0, SEEK_SET);
    fread(buffer, 1, length, f);
    fclose(f);

	data[0] = C_REJ;	// C
	data[1] = 0; // N
	data[2] = length / 255;	// L2
	data[3] = length % 255; // L1

	for (int i = 0; i < length; i++) {
		data[i+4] = buffer[i];
	}

	return data;
}
/*
int assemble_pic(unsigned char * pic_buffer){
    FILE * pic;
    pic = fopen("penguin.gif", "wb+");

    int picSize = pic_buffer[2]*255 + pic_buffer[3];
    unsigned char * aux = malloc(picSize);

    for(int i = 0; i < picSize; i++){
        aux[i] = pic_buffer[i+4];
    }

    fwrite(aux, 1, picSize-1, pic);
    fclose(pic);

    return 0;
}
*/

int main(int argc, char** argv)
{
    // appL = (applicationLayer *)malloc(sizeof(applicationLayer));
    
    // linkL = (linkLayer *)malloc(sizeof(linkLayer));
    // linkL->timeout = 20;
    // linkL->sequenceNumber = 0;

    char *port;
    char *img;
    unsigned char * control;

    int fd, res, length;
    struct termios oldtio, newtio;
    char buf[255];

    /*
    1ºcriar estrutuas de dados -> missing some?
    2ºler os argumentos q lhe vamos passar -> DONE
    3ºabrir (llopen) -> TO_DO FROM THIS POINT DOWN
    4º
    SE FOR TRANSMITTER:
    -abrir ficheiro
    -mandar control packet (app)
    fazer ciclo para criar data packet (app) e fazer llwrite deste
    -mandar control packet (do fim)
    SE FOR RECIEVER:
    fazer ciclo while durante o tempo todo a ler os packets

    */

    // argc = 2 -> programa + porta = receptor
    // argc = 3 -> programa + porta + path da imagem = emissor

    if(argc < 2 || argc > 3){
       printf("Invalid Usage:\tInvalid number of arguments");
       exit(1);
    }

    for(int i = 1; i < argc; i++){
        if(strcmp(MODEMDEVICE_0, argv[i]) == 0 || strcmp(MODEMDEVICE_1, argv[i]) == 0){
            port = argv[i];
        }
        else {
			int accessableImg = access(argv[i], F_OK);
			if (accessableImg == 0) {
				img = argv[i];
			} 
            else {
				printf("Invalid Usage:\tInvalid arguments\n");
				exit(1);
			}
		}
    }
    if(img == NULL){ // Open comunications for receiver
        if((fd = llopen(port, RECEIVER))){
            unsigned char *msg;
            unsigned char *buffer;
            unsigned char *msg_start;
            unsigned char *msg_end;

            msg_start = llread(fd);

            if(msg_start[0] == REJ){
                printf("Received start\n");
                /*
                ESTA MAL
                int reading_data = FALSE;
                int i = 0;
                while(!reading_data){
                    buffer = llread(fd);
                    strcat(msg, buffer);
                    if(msg[strlen(msg) - ] == 3){  //campo de controlo
                        reading_data = TRUE;
                        printf("Received end\n");
                    }
                }
                msg_end = msg[i];
                free(msg[i]);
                */
                msg = llread(fd);
                msg_end = llread(fd);

                }
            
            llclose(*port);

            int number_frames = sizeof(*msg) / (MAX_SIZE - 6);
            FILE *file_return_final = fopen("return_file.gif", "w");
            for(int i = 0; i< number_frames; i++){
                fputc(msg[i], file_return_final);
            }
            fclose(file_return_final);
        
        }
    }
    else if(llopen(port, TRANSMITTER)){ // Open comunications for transmitter
        unsigned char * buffer = process_pic(img, &length);
        if(length <= 5){ // demand at least a byte, the rest is the header
            printf("Error processing image\n");
            exit(1);
        }
        //CONTROL packet
        control[0] = 2; // C_BEGIN
        control[1] = 0; // T_FILESIZE
        control[2] = 1;
        control[3] = length;

        if(llwrite(fd, control, 4)){    //escreve control packet
            if(llwrite(fd, buffer, length)){
                control[0] = 3;
                llwrite(fd, control, 4);
            }
        }

        llclose(fd);
    } 
    
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}

	close(fd);

	return 0;
}

