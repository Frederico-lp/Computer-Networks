#include "application.h"

int main()
{
    appL = (applicationLayer *)malloc(sizeof(applicationLayer));
    
    linkL = (linkLayer *)malloc(sizeof(linkLayer));
    linkL->timeout = 20;
}