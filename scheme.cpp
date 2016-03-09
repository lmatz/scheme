//
//  scheme.cpp
//  compiler
//
//  Created by martin823
//  
//

#include "scheme.h"


#define DEBUG(...)                   \
       do  {                         \
           if (debug==1) {           \
            fprintf(stderr, __VA_ARGS__);     \
           }                         \
       } while(0)

int debug=0;


#define add_function(s_name,f_name)   \
        def_var_val(make_symbol(s_name), make_builtin_procedure(f_name), global_environment);


#define get_type(type) \
        if (type==0) {              \
            printf("integer\n");    \
        }                           \
        else if (type==1) {         \
            printf("real\n");       \
        }                           \
        else if (type==2) {         \
            printf("boolean\n");    \
        }                           \
        else if (type==3) {         \
            printf("character\n");  \
        }                           \
        else if (type==4) {         \
            printf("string\n");     \
        }                           \
        else if (type==5) {         \
            printf("pair\n");       \
        }                           \
        else if (type==6) {         \
            printf("symbol\n");     \
        }                           \
        else if (type==7) {         \
            printf("builtin_function\n");   \
        }                           \
        else if (type==8) {         \
            printf("compound_function\n");  \
        }                           \
        else if (type==9) {         \
            printf("empty_list\n"); \
        }                           \
        else if (type==10) {        \
            printf("warn\n");       \
        }                           \
        else if (type==11) {        \
            printf("INPUT PORT\n"); \
        }                           \
        else {                      \
            printf("OUTPUT PORT\n");\
        }

/*********************************
 *                               *
 *                               *
 *                               *
 *  PART: I/O                    *
 *                               *
 *                               *
 *                               *
 *********************************/

object* make_input_port(FILE* in) {
    object* temp=NULL;

    temp=new_object();
    temp->type=INPUT_PORT;
    temp->data.input_port.stream=in;
    return temp;
}

int is_input_port(object* obj) {
    return obj->type==INPUT_PORT;
}

object* make_output_port(FILE* out) {
    object* temp=NULL;

    temp=new_object();
    temp->type=OUTPUT_PORT;
    temp->data.output_port.stream=out;
    return temp;
}

int is_output_port(object* obj) {
    return obj->type==OUTPUT_PORT;
}


object* load_procedure(object* args) {
    char *file=NULL;
    FILE *in=NULL;
    object* exp;
    object* res;

    file=car(args)->data.string.value;
    in=fopen(file,"r");
    if (in==NULL) {
        return make_warn("Exception in Load: Cannot load the file");
    }

    while( (exp=read(in))!=NULL ) {
        res=eval(exp,global_environment);
    }
    fclose(in);
    return res;
}

object* read_procedure(object* args) {
    return args;
}

object* open_input_port_procedure(object* args) {
    char *file=NULL;
    FILE *in=NULL;


    file=car(args)->data.string.value;
    in=fopen(file,"r");
    if (in==NULL) {
        return make_warn("Exception in open input port: Cannot open input port");
    }
    return make_input_port(in);
}

object* close_input_port_procedure(object* args) {
    char res;

    res=fclose(car(args)->data.input_port.stream);
    if ( res==EOF ) {
        return make_warn("Exception in close input port: Cannot close input port");
    }
    return ok_symbol;
}

object* open_output_port_procedure(object* args) {
    char *file=NULL;
    FILE *out=NULL;


    file=car(args)->data.string.value;
    out=fopen(file,"w");
    if (out==NULL) {
        return make_warn("Exception in open input port: Cannot open input port");
    }
    return make_output_port(out);
}

object* close_output_port_procedure(object* args) {
    char res;

    res=fclose(car(args)->data.output_port.stream);
    if ( res==EOF ) {
        return make_warn("Exception in close input port: Cannot close input port");
    }
    return ok_symbol;
}    
    
void init() {

    init_symbol();

    global_environment=setup_env();

    add_function("+",add_procedure);
    add_function("-",sub_procedure);
    add_function("*",mul_procedure);
    add_function("/",div_procedure);
    add_function("mod",mod_procedure);
    add_function("=",num_equal_procedure);
    add_function(">",greater_than_procedure);
    add_function("<",less_than_procedure);

    add_function("and",and_procedure);
    add_function("or",or_procedure);

    add_function("null?",null_procedure);
    add_function("boolean?",boolean_procedure);
    add_function("symbol?",symbol_procedure);
    add_function("integer?",integer_procedure);
    add_function("char?",char_procedure);
    add_function("string?",string_procedure);
    add_function("pair?",pair_procedure);
    add_function("procedure?",procedure_procedure);
    add_function("eq?",eq_procedure);


    add_function("char->integer",char_to_integer_procedure);
    add_function("integer->char",integer_to_char_procedure);
    add_function("number->string",number_to_string_procedure);
    add_function("string->number",string_to_number_procedure);
    add_function("symbol->string",symbol_to_string_procedure);
    add_function("string->symbol",string_to_symbol_procedure);

    add_function("cons",cons_procedure);
    add_function("car",car_procedure);
    add_function("cdr",cdr_procedure);
    add_function("set-car!",set_car_procedure);
    add_function("set-cdr!",set_cdr_procedure);
    add_function("list",list_procedure);

    add_function("load"             , load_procedure);
    add_function("open-input-port"  , open_input_port_procedure);
    add_function("close-input-port" , close_input_port_procedure);
    // add_function("input-port?"      , is_input_port_procedure);
    // add_function("read"             , read_procedure);
    // add_function("read-char"        , read_char_procedure);
    // add_function("peek-char"        , peek_char_procedure);
    // add_function("eof-object?"      , is_eof_object_procedure);
    add_function("open-output-port" , open_output_port_procedure);
    add_function("close-output-port", close_output_port_procedure);
    // add_function("output-port?"     , is_output_port_procedure);
    // add_function("write-char"       , write_char_procedure);
    // add_function("write"            , write_procedure);

}

void loadstd() {
    FILE *std;

    std = fopen("std.scm" , "r");
    if (std == NULL) {
        fprintf(stdout,"Error opening std.scm\n");
    }
    else {
        while ( peek(std)!=EOF ) {
            printer(eval(read(std), global_environment));
        }
    }
    fclose (std);
}

void sighandler(int signum)
{
    printf("\n> ");
}

int main(int argc, char**argv) {

    if ( argc>1 && !strcmp(argv[1],"-d") ) {
        printf("Debug mode!\n");
        debug=1;
    }
    printf("********************************\n\n\n"
        "          Little Schemer          \n\n\n"
        "          ctrl-d to exit.\n\n\n"
           "********************************\n");
    
    init();
    loadstd();
    signal(SIGINT, sighandler);
    while (1) {
        printf("> ");
        printer(eval(read(stdin), global_environment));
        printf("\n");
    }
        
    return 0;
}







