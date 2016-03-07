#include "printer.h"

#define DEBUG(...)                   \
       do  {                         \
           if (debug==1) {           \
            fprintf(stderr, __VA_ARGS__);     \
           }                         \
       } while(0)

extern int debug;


void print_pair(object* obj) {
    printer(obj->data.pair.car);
    
    // the key is that when we want to print the cdr object, if it is a pair, we need to print_pair not printer. Because no matter what object it is (except empty list), it will always connected with some other object as a pair. So, unless it self is enclosed by a paren, it will not be enclosed by a pair of parens when it is printed out. And if it is enclosed by a paren, when we print_pair it, the paren will shows because of printer(car)
    if (obj->data.pair.cdr->type==PAIR) {
        fprintf(stdout," ");
        print_pair(obj->data.pair.cdr);
    }
    else if (obj->data.pair.cdr->type==EMPTY_LIST){
        return ;
    }
    else {
        fprintf(stdout," . ");
        printer(obj->data.pair.cdr);
    }
}


void printer(object *obj) {
    char *str=NULL;
    // DEBUG("object type: %d",obj->type);
    // DEBUG("start to print!\n");
    fprintf(stdout,"RESULT:\n");

    switch(obj->type) {
        case INTEGER:
            fprintf(stdout,"%ld",obj->data.integer.value);
            break;
        case REAL:
            fprintf(stdout,"%ld",obj->data.real.f_part);
            fprintf(stdout,".");
            fprintf(stdout,"%ld",obj->data.real.s_part);
            break;
        case BOOLEAN:
            fprintf(stdout,"#%c",obj->data.boolean.value);
            break;
        case CHARACTER:
            if (  isalpha(obj->data.character.value)  ) {
                fprintf(stdout,"#\\%c",obj->data.character.value);
            }
            else if ( obj->data.character.value=='\n' ) {
                fprintf(stdout,"#\\newline");
            }
            else if (obj->data.character.value==' ' ) {
                fprintf(stdout,"#\\space");
            }
            break;
        case STRING:
            str=obj->data.string.value;
            fprintf(stdout,"\"");
            while( *str!='\0' ) {
                if (*str=='\n') {
                    fprintf(stdout,"\\n");
                }
                else if ( *str=='\\') {
                    fprintf(stdout,"\\");
                }
                else if (*str=='"'){
                    fprintf(stdout, "\\\"");
                }
                else {
                    fprintf(stdout,"%c",*str);
                }
                str++;
            }
            fprintf(stdout,"\"");
            break;
        case PAIR:
            fprintf(stdout,"(");
            print_pair(obj);
            fprintf(stdout,")");
            break;
        case SYMBOL:
            fprintf(stdout,"%s",obj->data.symbol.value);
            break;
        case BUITLIN_PROC:
        case COMPOUND_PROC:
            fprintf(stdout,"#<procedure>");
            break;
        case EMPTY_LIST:
            break;
        case WARN:
            fprintf(stdout,"**%s**",obj->data.warn.value);
            break;
        default:
            fprintf(stderr,"Unknow type!\nExit!\n");
            break;
    }
}