#include "link.h"

#define ESCAPE_OCETET   0X7d

typedef struct {
int fileDescriptor; /*Descritor correspondente à porta série*/
int status;         /*TRANSMITTER | RECEIVER*/
} applicationLayer;

applicationLayer *appL;

