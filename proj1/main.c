#include "application.h"

int main(int argc, char** argv)
{
    appL = (applicationLayer *)malloc(sizeof(applicationLayer));
    
    // linkL = (linkLayer *)malloc(sizeof(linkLayer));
    linkL.timeout = 20;
    linkL.sequenceNumber = 0;

    char *port;
    char *img;

    /*
    1ºcriar estrutuas de dados -> missing some?
    2ºler os argumentos q lhe vamos passar
    3ºabrir (llopen)
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
    }


}