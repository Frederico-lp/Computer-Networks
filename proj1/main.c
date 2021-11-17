#include "application.h"

int main()
{
    appL = (applicationLayer *)malloc(sizeof(applicationLayer));
    
    // linkL = (linkLayer *)malloc(sizeof(linkLayer));
    linkL.timeout = 20;
    linkL.sequenceNumber = 0;
    /*
    1ºcriar estrutuas de dados
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
}