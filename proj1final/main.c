#include "application.h"

unsigned char * process_pic(char* path, int* size){
    FILE *f = fopen(path, "r");
    struct stat st;
    if (stat(path, &st) == 0)
        *size = st.st_size;
    //*size = ftell(f); //qts bytes tem o ficheiro
    unsigned char *data = (unsigned char*)malloc(*size+4);
    unsigned char *buffer = (unsigned char *)malloc(*size);


    //fseek(f, 0, SEEK_SET);
    fread(buffer, 1, *size, f);
    fclose(f);

	data[0] = C_REJ;	// C
	data[1] = 0; // N
	data[2] = *size / 255;	// L2
	data[3] = *size % 255; // L1

	for (int i = 0; i < *size; i++) {
		data[i+4] = buffer[i];
	}
    printf("main- total file bytes = %d\n", *size);

	return data;
}


int main(int argc, char** argv)
{
    fflush(stdout);

    char *port;
    char *img;
    unsigned char control[100];

    int fd, res, length = 5;
    char buf[255];


    if(argc < 2 || argc > 3){
       printf("Invalid Usage:\tInvalid number of arguments0");
       exit(1);
    }

        if(strcmp(MODEMDEVICE_0, argv[1]) == 0 || strcmp(MODEMDEVICE_1, argv[1]) == 0 || 
    strcmp(SOCAT_MODEMDEVICE_10, argv[1]) == 0 || strcmp(SOCAT_MODEMDEVICE_11, argv[1]) == 0 ){
            port = argv[1];
        }
    if(argc == 2)
        img = NULL;
    else if(argc == 3){
	    int accessableImg = access(argv[2], F_OK);
	    if (accessableImg == 0) {
		    img = argv[2];
	    } 
        else {
		    printf("Invalid Usage:\tInvalid arguments1\n");
		    exit(1);
	    }
    }
    
    //img = NULL;
    if(img == NULL){ // Open comunications for receiver
    printf("receiver\n");
        if((fd = llopen(port, RECEIVER))){
            printf("after ua received\n");
            unsigned char *msg = NULL;
            msg = (unsigned char *)malloc(1000 * 2);
            unsigned char *buffer;
            unsigned char *msg_start;
            unsigned char *msg_end;
            printf("receiver reading first control packet\n");
            int control_size = 0;
            msg_start = llread(fd, &control_size);

            printf("receiver reading first data packet\n");
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

                }
                int size = 0;
                msg = llread(fd, &size);
                printf("receiver reading last control packet\n");
                msg_end = llread(fd, &control_size);
            
            llclose(fd, RECEIVER);
            //int number_frames = sizeof(*msg) / (MAX_SIZE - 6);
            printf("SIZE OF READ IS %d\n\n\n", size);
            FILE *f = fopen("return_file.gif", "w");
            for(int i = 4; i< size; i++){
                fputc(msg[i], f);
            }
            int written = fwrite(msg, sizeof(unsigned char), sizeof(msg), f);
            if (written == 0) {
                printf("Error during writing to file !");
            }
            fclose(f);
        
        }
    }
    else if(fd = llopen(port, TRANSMITTER)){ 
        printf("transmitter");
        unsigned char * buffer = process_pic(img, &length);
        if(length <= 5){ // demand at least a byte, the rest is the header
            printf("Error processing image\n");
            exit(1);
        }
        printf(" length in main is %d\n", length);
        //CONTROL packet
        control[0] = 2; // C_BEGIN
        control[1] = 0; // T_FILESIZE
        control[2] = 1;
        control[3] = length;
        int tempLength = length;
        int l1 = (length / 255) + 1;
        int i;
        for(i = 0; i < l1; i++){
        control[4+i] =  (tempLength >> 8*(i+1) & 0xff);   //so o ultimo byte do numero
    }
        printf("starting transmission of CONTROL packet\n");
        printf("fd = %d\n", fd);    
        if(llwrite(fd, control, i + 4)){    //escreve control packet  
            printf("starting transmission of I packet\n");
            if(llwrite(fd, buffer, length)){
                control[0] = 3;
                printf("starting transmission of last CONTROL packet\n");
                llwrite(fd, control, 4);
            }
        }

        llclose(fd, TRANSMITTER);
    } 
    
    

	return 0;
}

