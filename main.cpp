#include <iostream>
#include "scheme.h"

int main() {
    

    
    
    
    printf("********* Little_schemer ********\n"
           "ctrl-c to exit.\n");
    
    init();

    
    while (1) {
        printf("> ");
        printer(eval(read(stdin)));
        printf("\n");
        indent=0;
    }
    
    
    
    
    
    return 0;
}
