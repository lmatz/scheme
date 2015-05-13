//
//  scheme.cpp
//  compiler
//
//  Created by martin823
//  
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>


#define DEBUG(...)                   \
       do  {                         \
           if (debug==1) {           \
            fprintf(stderr, __VA_ARGS__);     \
           }                         \
       } while(0)

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
*  PART:  Global variable       *
*                               *
*                               *
*                               *
*********************************/


typedef enum obj_type {
        INTEGER,
        REAL,
        BOOLEAN,
        CHARACTER,
        STRING,
        PAIR,
        SYMBOL,
        BUITLIN_PROC,
        COMPOUND_PROC,
        EMPTY_LIST,
        WARN,
        INPUT_PORT,
        OUTPUT_PORT,
} obj_type;

typedef struct object {
    obj_type type;
    union {
        struct {
            long value;
        }integer;
        struct {
            long f_part;
            long s_part;
        }real;
        struct {
            char value;
        } character;
        struct {
            char value;
        }boolean;
        struct {
            char *value;
        } string;
        struct {
            object * car;
            object * cdr;
        }pair;
        struct {
            char *value;
        }symbol;
        struct {
            object* (*func)(object *args);
        }builtin_proc;
        struct {
            object* parameters;
            object* body;
            object* env;
        }compound_proc;
        struct {
            char *value;
        }warn;
        struct {
            FILE *stream;
        } input_port;
        struct {
            FILE *stream;
        } output_port;
    }data;
} object;


int debug=0;

object* empty_list;
object* False;
object* True;
object* symbol_table;
object* quote_symbol;
object* define_symbol;
object* set_symbol;
object* ok_symbol;
object* if_symbol;
object* lambda_symbol;
object* begin_symbol;
object* cond_symbol;
object* else_symbol;
object* let_symbol;
object* and_symbol;
object* or_symbol;
object* apply_symbol;

object* empty_environment;
object* global_environment;


// forward decalaration

object* read (FILE *in);
void printer(object *obj);
object* eval(object* exp, object* env);
int is_compound_func(object* obj);



/*********************************
*                               *
*                               *
*                               *
*  PART:  Object Manipulation   *
*                               *
*                               *
*                               *
*********************************/

int is_pair(object *obj) {
    return obj->type == PAIR;
}

int is_empty_list(object *obj) {
    return obj == empty_list;
}

int is_false(object *obj) {
    return obj == False;
}

int is_true(object *obj) {
    return !is_false(obj);
}

object *car(object *pair) {
    return pair->data.pair.car;
}
object *cdr(object *pair) {
    return pair->data.pair.cdr;
}

void set_car(object *obj, object* value) {
    obj->data.pair.car = value;
}

void set_cdr(object *obj, object* value) {
    obj->data.pair.cdr = value;
}

#define caar(obj)   car(car(obj))
#define cadr(obj)   car(cdr(obj))
#define cdar(obj)   cdr(car(obj))
#define cddr(obj)   cdr(cdr(obj))
#define caaar(obj)  car(car(car(obj)))
#define caadr(obj)  car(car(cdr(obj)))
#define cadar(obj)  car(cdr(car(obj)))
#define caddr(obj)  car(cdr(cdr(obj)))
#define cdaar(obj)  cdr(car(car(obj)))
#define cdadr(obj)  cdr(car(cdr(obj)))
#define cddar(obj)  cdr(cdr(car(obj)))
#define cdddr(obj)  cdr(cdr(cdr(obj)))
#define caaaar(obj) car(car(car(car(obj))))
#define caaadr(obj) car(car(car(cdr(obj))))
#define caadar(obj) car(car(cdr(car(obj))))
#define caaddr(obj) car(car(cdr(cdr(obj))))
#define cadaar(obj) car(cdr(car(car(obj))))
#define cadadr(obj) car(cdr(car(cdr(obj))))
#define caddar(obj) car(cdr(cdr(car(obj))))
#define cadddr(obj) car(cdr(cdr(cdr(obj))))
#define cdaaar(obj) cdr(car(car(car(obj))))
#define cdaadr(obj) cdr(car(car(cdr(obj))))
#define cdadar(obj) cdr(car(cdr(car(obj))))
#define cdaddr(obj) cdr(car(cdr(cdr(obj))))
#define cddaar(obj) cdr(cdr(car(car(obj))))
#define cddadr(obj) cdr(cdr(car(cdr(obj))))
#define cdddar(obj) cdr(cdr(cdr(car(obj))))
#define cddddr(obj) cdr(cdr(cdr(cdr(obj))))

object* new_object() {
    object* temp;
    
    temp=(object *)malloc(sizeof(object));
    if (temp==NULL) {
        fprintf(stderr,"Run out of memory!\nEXIT!\n");
        exit(1);
    }
    
    return temp;
}

object* cons(object* car, object* cdr) {
    object *temp;
    
    temp=new_object();
    temp->type=PAIR;
    temp->data.pair.car=car;
    temp->data.pair.cdr=cdr;
    
    return temp;
}

int is_boolean(object* obj) {
    return obj->type==BOOLEAN;
}

// chacacter object
object* make_character(char c) {
    object *temp;
    
    temp=new_object();
    temp->type=CHARACTER;
    temp->data.character.value=c;
    
    return temp;
}

int is_character(object* obj) {
    return obj->type==CHARACTER;
}

// integer object
object* make_integer(long value) {
    object* temp;
    
    temp=new_object();
    temp->type=INTEGER;
    temp->data.integer.value=value;
    
    return temp;
}

int is_integer(object* obj) {
    return obj->type==INTEGER;
}

// real object
object* make_real(long f_part,long s_part) {
    object *temp;
    
    temp=new_object();
    temp->type=REAL;
    temp->data.real.f_part=f_part;
    temp->data.real.s_part=s_part;
    
    return temp;
}

int is_real(object* obj) {
    return obj->type==REAL;
}

// object* make_real(double num) {
//     object* temp;
//     int a=0;

//     temp=new_object();
//     temp->type=REAL;

//     while()

// }

double RealtoNum(object* obj) {
    int f=obj->data.real.f_part;
    int s=obj->data.real.s_part;
    double res=0;

    if (s==0) {
        return (double)f;
    }

    int a=10;
    while( s%a!=0 ) {
        a*=10;
    }

    res=s/a;
    res+=f;
    return res;
}

// string
object* make_string(const char* buffer)  {
    object *temp;
    
    temp=new_object();
    temp->type=STRING;
    temp->data.string.value=(char *)malloc(sizeof(char)*(strlen(buffer)+1));
    strcpy(temp->data.string.value,buffer);
    

    DEBUG("buffer:%s!\n",buffer);
    DEBUG("object:%s!\n",temp->data.string.value);

    return temp;
}

int is_string(object* obj) {
    return obj->type==STRING;
}

object* make_symbol(const char* buffer) {
    object *temp;
    
    object *temp_table=symbol_table;
    
    // printf("start to make symbol , first to look up in the symbol table!\n");
    while(  !is_empty_list(temp_table) ) {
        if (   !strcmp(buffer,car(temp_table)->data.symbol.value)    ) {
            return car(temp_table);
        }
        temp_table=cdr(temp_table);
    }
    // printf("second to make new symbol!\n");
    temp=new_object();
    temp->type=SYMBOL;
    temp->data.symbol.value=(char *)malloc(sizeof(char)*(strlen(buffer)+1));
    if (temp->data.symbol.value == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    strcpy(temp->data.symbol.value,buffer);
    // fprintf(stdout,"buffer:%s!\n",buffer);
    // fprintf(stdout,"object:%s!\n",temp->data.string.value);
    symbol_table = cons(temp, symbol_table);
    return temp;
}

int is_symbol(object* obj) {
    return obj->type==SYMBOL;
}

object* make_warn(const char* buffer) {
    object *temp;
    
    temp=new_object();
    temp->type=WARN;
    temp->data.string.value=(char *)malloc(sizeof(char)*(strlen(buffer)+1));
    strcpy(temp->data.string.value,buffer);

    return temp;
}

int is_warn(object* obj) {
    return obj->type=WARN;
}

object* make_lambda(object *parameters, object *body) {
    return cons(lambda_symbol, cons(parameters, body));
}
/*********************************
 *                               *
 *                               *
 *                               *
 *  PART: Builtin Functions      *
 *                               *
 *                               *
 *                               *
 *********************************/

object* make_builtin_procedure( object *(*func)(object* arguments) ) {
    object* obj;

    obj=new_object();
    obj->type = BUITLIN_PROC;
    obj->data.builtin_proc.func=func;

    return obj;
}

int is_builtin_procedure( object* obj) {
    return obj->type==BUITLIN_PROC;
}

object* add_procedure(object* args) {
    long res=0;

    DEBUG("ADD_PROCEDURE!\n");

    while( !is_empty_list(args)) {
        if ( car(args)->type==INTEGER ) {
            res=res+(car(args))->data.integer.value;
        }
        else if ( car(args)->type==PAIR ) {
            // get_type(car(args)->type);
            res=res+add_procedure(car(args))->data.integer.value;
        }
        else {
            return make_warn("Exception in +: operand is not a number");
        }
        args = cdr(args);
    }
    return make_integer(res);
}

object* sub_procedure(object* args) {
    long res=0;

    if ( !is_empty_list(args) ) {
        if ( car(args)->type==INTEGER ) {
            res+=(car(args))->data.integer.value;
        }
        args=cdr(args);
    }

    while( !is_empty_list(args)) {
        if ( car(args)->type==INTEGER ) {
            res-=(car(args))->data.integer.value;
        }
        else {
            return make_warn("Exception in -: operand is not a number");
        }
        args = cdr(args);
    }

    return make_integer(res);
}

object* mul_procedure(object* args) {
    long res=1;
    int c=0;

    while( !is_empty_list(args)) {
        if ( car(args)->type==INTEGER ) {
            res=res*(car(args))->data.integer.value;
        }
        else if ( car(args)->type==PAIR ) {
            // get_type(car(args)->type);
            res=res+mul_procedure(car(args))->data.integer.value;
        }
        args = cdr(args);
        c++;
    }
    if(c<=1) {
        return False;
    }
    return make_integer(res);
}

object* div_procedure(object* args) {
    double res=0;

    if ( !is_empty_list(args) ) {
        if ( car(args)->type==INTEGER ) {
            res+=(car(args))->data.integer.value;
        }
        args=cdr(args);
    }

    while( !is_empty_list(args)) {
        if ( car(args)->type==INTEGER ) {
            res/=(car(args))->data.integer.value;
        }
        args = cdr(args);
    }
    return make_integer((int)res);
}

object* num_equal_procedure(object* args) {
    long value=0;

    value=(car(args))->data.integer.value;
    while ( !is_empty_list(args=cdr(args))) {
        if (value!=car(args)->data.integer.value) {
            return False;
        }
    }
    return True;
}

object* less_than_procedure(object* args) {
    long first;
    long second;

    first=car(args)->data.integer.value;
    while( !is_empty_list(args=cdr(args))) {
        second=car(args)->data.integer.value;
        if (first<second) {
            first=second;
        }
        else {
            return False;
        }
    }
    return True;
}

object* greater_than_procedure(object* args) {
    long first;
    long second;

    first=car(args)->data.integer.value;
    while( !is_empty_list(args=cdr(args))) {
        second=car(args)->data.integer.value;
        if (first>second) {
            first=second;
        }
        else {
            return False;
        }
    }
    return True;
}

// object* and_procedure(object* args) {
//     if( is_empty_list(args) ) {
//         return True;
//     }

//     if ( car(args)==False ) {
//         return False;
//     }
//     else {
//         return and_procedure(cdr(args));
//     }
// }

// object* or_procedure(object* args) {
//     if( is_empty_list(args) ) {
//         return False;
//     }

//     if ( car(args)==True ) {
//         return True;
//     }
//     else {
//         return and_procedure(cdr(args));
//     }
// }

object* null_procedure(object* args) {
    return is_empty_list(car(args))?True:False;
}

object* boolean_procedure(object* args) {
    return is_boolean(car(args))?True:False;
}

object* symbol_procedure(object* args) {
    return is_symbol(car(args))?True:False;
}

object* integer_procedure(object* args) {
    return is_integer(car(args))?True:False;
}

object* char_procedure(object* args) {
    return is_character(car(args))?True:False;
}

object* string_procedure(object* args) {
    return is_string(car(args)) ?True:False;
}

object* pair_procedure(object *args) {
    return is_pair(car(args))?True:False;
}

object* procedure_procedure(object* args) {
    return (is_builtin_procedure(car(args)) || is_compound_func(car(args)))?True:False;
}

object* char_to_integer_procedure(object* args) {
    return make_integer(car(args)->data.character.value);
}

object* integer_to_char_procedure(object* args) {
    return make_character(car(args)->data.integer.value);
}

object* number_to_string_procedure(object* args) {
    char buffer[11];

    sprintf(buffer, "%ld", (car(args))->data.integer.value);
    return make_string(buffer);
}

object* string_to_number_procedure(object* args) {
    return make_integer(atoi((car(args)->data.string.value)));
}

object* symbol_to_string_procedure(object* args) {
    return make_string((car(args))->data.symbol.value);
}

object* string_to_symbol_procedure(object* args) {
    return make_symbol((car(args))->data.string.value);
}

object* cons_procedure(object* args) {
    return cons(car(args),cadr(args));
}

object* car_procedure(object* args) {
    return caar(args);
}

object* cdr_procedure(object* args) {
    return cdar(args);
}

object* set_car_procedure(object* args) {
    set_car(car(args),cadr(args));
    return ok_symbol;
}

object* set_cdr_procedure(object* args) {
    set_cdr(car(args),cadr(args));
    return ok_symbol;
}

object* list_procedure(object* args) {
    return args;
}

object* eq_procedure(object* args) {
    object *obj1;
    object *obj2;
    
    obj1 = car(args);
    obj2 = cadr(args);
    
    if (obj1->type != obj2->type) {
        return False;
    }
    switch (obj1->type) {
        case INTEGER:
            return (obj1->data.integer.value == 
                    obj2->data.integer.value) ?
                        True : False;
            break;
        case CHARACTER:
            return (obj1->data.character.value == 
                    obj2->data.character.value) ?
                        True : False;
            break;
        case STRING:
            return (strcmp(obj1->data.string.value, 
                           obj2->data.string.value) == 0) ?
                        True : False;
            break;
        default:
            return (obj1 == obj2) ? True : False;
    }
}

object* make_compound_func(object* parameters, object* body, object* env) {
    object* temp;

    temp=new_object();
    temp->type= COMPOUND_PROC;
    temp->data.compound_proc.parameters=parameters;
    temp->data.compound_proc.body=body;
    temp->data.compound_proc.env=env;

    return temp;
}

int is_compound_func(object* obj) {
    return obj->type==COMPOUND_PROC;
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




/*********************************
 *                               *
 *                               *
 *                               *
 *  PART: Frame and Environment  *
 *                               *
 *                               *
 *                               *
 *********************************/

int is_empty_env(object* env) {
    return env==empty_environment;
}

object* enclosing_env(object* env) {
    return cdr(env);
}

object* first_frame(object* env) {
    return car(env);
}

object* frame_var(object* frame) {
    return car(frame);
}

object* frame_val(object* frame) {
    return cdr(frame);
}

object* loop_up_env(object* var, object* env) {
    object* frame;
    object* var_list;
    object* val_list;
    while (!is_empty_list(env)) {
        frame=first_frame(env);
        var_list=frame_var(frame);
        val_list=frame_val(frame);
        while (!is_empty_list(var_list)) {
            if (var==car(var_list)) {
                return car(val_list);
            }
            var_list=cdr(var_list);
            val_list=cdr(val_list);
        }
        env=enclosing_env(env);
    }
    // fprintf(stderr," unbound variable\n");
    // exit(1);
    return make_warn("unbound variable");
}

void add_to_frame(object* var,object* val, object* frame) {
    set_car(frame , cons(var , car(frame)));
    set_cdr(frame , cons(val , cdr(frame)));
}

object* make_frame(object* var,object* val) {
    return cons(var,val);
}

object* extend_env(object* var,object* val, object* old_env) {
    return cons(make_frame(var,val),old_env);
}
int set_var_val(object* var, object* val, object* env) {
    object* frame;
    object* var_list;
    object* val_list;

    while ( !is_empty_list(env)) {
        frame=first_frame(env);
        var_list=frame_var(frame);
        val_list=frame_val(frame);
        while ( !is_empty_list(var_list) ) {
            if (var==car(var_list) ) {
                set_car(val_list, val);
                return 1;
            }
            var_list=cdr(var_list);
            val_list=cdr(val_list);
        }
        env=enclosing_env(env);
    }
    // fprintf(stderr,"unbound variable\n");
    // exit(1);
    return 0;
}

void def_var_val(object* var, object* val, object* env) {
    object* frame;
    object* var_list;
    object* val_list;

    frame=first_frame(env);
    var_list=frame_var(frame);
    val_list=frame_val(frame);

    while ( !is_empty_list(var_list)) {
        if (var==car(var_list)) {
            set_car(val_list, val);
            return;
        }
        var_list=cdr(var_list);
        val_list=cdr(val_list);
    }
    add_to_frame(var,val,frame);
}

object* setup_env() {
    object * env=NULL;
    
    env=extend_env( empty_list, empty_list, empty_environment);

    return env;
}

/*********************************
*                               *
*                               *
*                               *
*  PART:  Tokenize and Parse    *
*                               *
*                               *
*                               *
*********************************/

char peek(FILE *in) {
    char c;
    c=getc(in);
    ungetc(c,in);
    return c;
}

void skip_space(FILE *in) {
    char c;
    
    // DEBUG("enter skip space!\n");

    while ( (c = getc(in))!= EOF) {
        
        // printf("loop: c: %c!\n",c);

        if (isspace(c)) {
            continue;
        }
        else if (c == ';') { /* comments are whitespace also */
            while (((c = getc(in)) != EOF) && (c != '\n'));
            continue;
        }
        ungetc(c, in);
        break;
    }
    // printf("leave skip space!\n");
}

int is_ex_al(char c) {
    if ( c=='!' || c=='$' || c=='%' || c=='&' || c=='*'
        || c=='+' || c=='-' || c=='.' || c=='/' ||
        c==':' || c=='<' || c=='=' || c=='>' ||
        c=='?' || c=='@' || c=='^' || c=='_' ||
        c=='~' ) {
        return 1;
    }
    return 0;
}

//int is_delimiter(int c) {
//    return isspace(c) || c == EOF ||
//    c == '('   || c == ')' ||
//    c == '"'   || c == ';';
//}

int get_expected_string(FILE *in, const char * expect) {
    char c;
    
    while( *expect!='\0' ) {
        c=getc(in);
        if ( c != *expect ) {
            // fprintf(stderr, "unexpected character!\nExit!\n");
            // exit(1);
            return 0;
        }
        expect++;
    }
    return 1;
}

int peek_expected_delimiter(FILE *in, const char *expect) {
    char c;
    c=peek(in);
    while ( c != '\0' ) {
        if (c==*expect) {
            return 1;
        }
        expect++;
    }
    // fprintf(stderr,"no expected dilimiter followed!\nExit!\n");
    return 0;
}

object* get_symbol(FILE *in) {
    char c;
    c=getc(in);
    char buffer[256];
    int offset=0;
    
    while( is_ex_al(c) || isalpha(c) || isdigit(c)  ) {
        if (offset<256-1) {
            buffer[offset++]=c;
        }
        else {
            // fprintf(stderr,"Symbol name is too long!\nExit!\n");
            // exit(1);
            return make_warn("Symbol name is too long!");
        }
        c=getc(in);
    }
    if ( isspace(c) || c==')' || c=='(' ) {
        buffer[offset]='\0';
        DEBUG("Get symbol name: %s\n",buffer);
        ungetc(c,in);
        return make_symbol(buffer);
    }
    else {
        // fprintf(stderr,"Symbol is not followed by a valid delimiter!\nExit!\n");
        // exit(1);
        return make_warn("Symbol is not followed by a valid delimiter!");
    }
}

object* get_pair(FILE *in) {
    char c;
    object *car;
    object *cdr;
    
    DEBUG("start to get pair!\n");
    
    skip_space(in);

    if (peek(in)==')') {
        getc(in);
        DEBUG("Get empty list\n");
        return empty_list;
    }
    
    car=read(in);
    
    skip_space(in);
    
    c=getc(in);
    if ( c=='.' ) {
        // printf("start to get improper list!\n");
        peek_expected_delimiter(in," \n");
        cdr=read(in);
        skip_space(in);
        c=getc(in);
        if ( c!=')' ) {
            DEBUG("Parentheses don't match!\nExit!\n");
            return make_warn("Parentheses don't match!");
        }
        return cons(car,cdr);
    }
    else {
        ungetc(c,in);
        cdr=get_pair(in);
        return cons(car,cdr);
    }
}

object* get_number(FILE *in) {
    short int sign=1;
    short int d_sign=0;
    char buffer[21];
    int offset=0;
    char c;
    
    c=getc(in);
    
    if (c=='-' ) {
        sign=-1;
    }
    else {
        ungetc(c,in);
    }
    while ( isdigit((c=getc(in))) || c=='.' ) {
        if (c=='.') {
            d_sign++;
        }
        buffer[offset++]=c;
    }
    ungetc(c,in);
    if (d_sign>1) {
        DEBUG("Invalid float number!\nExit!\n");
        // exit(1);
        return make_warn("Invalid float number!");
    }
    else if (d_sign==0) {
        int loop=0;
        long num=0;
        while( loop<offset ) {
            num=num*10+(buffer[loop]-'0');
            loop++;
        }
        num*=sign;
        DEBUG("Get integer: %ld\n",num);
        return make_integer(num);
    }
    else if (d_sign==1) {
        int loop=0;
        long f_part=0;
        long s_part=0;
        while( loop<offset && buffer[loop]!='.' ) {
            f_part=f_part*10+(buffer[loop]-'0');
            loop++;
        }
        f_part*=sign;
        loop++;
        while ( loop<offset ) {
            s_part=s_part*10+(buffer[loop]-'0');
            loop++;
        }
        DEBUG("Get real: %ld.%ld\n",f_part,s_part);
        return make_real(f_part,s_part);
    }
    return new_object();
}

object* get_character(FILE *in) {
    char c;
    c=getc(in);   
    switch(c) {
        case 's':
            if ( peek(in)=='p' ) {
                if ( !get_expected_string(in,"pace") ) {
                    return make_warn("unexpected character!");
                }
                peek_expected_delimiter(in," \n");
                return make_character(' ');
            }
            break;
        case 'n':
            if ( peek(in)=='e' ) {
                if ( !get_expected_string(in,"ewline") ) {
                    return make_warn("unexpected character!");
                }
                peek_expected_delimiter(in," \n");
                return make_character('\n');
            }
            break;
        case '(':
                if ( !peek_expected_delimiter(in," \n") ) {
                    return make_warn("no expected dilimiter followed!");
                }
                return make_character('(');
                break;        
    }
    if ( !peek_expected_delimiter(in," \n") ) {
        return make_warn("no expected dilimiter followed!");
    }
    return make_character(c);
}


/*********************************
 *                               *
 *                               *
 *                               *
 *  PART:  Dealing with char     *
 *                               *
 *                               *
 *                               *
 *********************************/



object* read (FILE *in) {
    char c;
    char temp;
    
    skip_space(in);
    c=getc(in);
    
    // DEBUG("enter the read function!\n");

    if ( c=='#'  ) { // try to get character
        
        // printf("character!\n");
    
        c = getc(in);
        switch (c) {
            case 'T':
                
            case 't':
                return True;
            case 'F':
                
            case 'f':
                return False;
            case '\\':
                return get_character(in);
            default:
                // fprintf(stderr, "unknown boolean literal\n");
                // exit(1);
            return make_warn("unknown boolean literal");
        }
    }
    else if ( isdigit(c) ||  ( c=='-' && isdigit(peek(in)) )    ) { //try to get number: integer & double
        
        // printf("digit!\n");
        
        ungetc(c,in);
        return get_number(in);
    }
    else if ( c=='"' ) { // try to get string

        // fprintf(stdout,"start to get string!\n");
        
        int i=0;
        char buffer[256];
        while( (c=getc(in))!='"' ) {
            if (c == '\\') {
                temp = peek(in);
                if (temp=='n') {
                    c='\n';
                    getc(in);
                }
                if (temp == '"') {
                    c = '"';
                    getc(in);
                }
            }
            if (c == EOF) {
                // fprintf(stderr, "non-terminated string literal\n");
                // exit(1);
                return make_warn("non-terminated string literal");
            }
            if (i < 256 - 1) {
                buffer[i++] = c;
            }
            else {
                // fprintf(stderr,
                //         "string too long. Maximum length is %d\n",
                //         256);
                return make_warn( "string too long");
            }
        }
        buffer[i]='\0';
    
        // fprintf(stderr,"before make string!");
        
        return make_string(buffer);
    }
    else if (c=='(') {  // try to get pair
        return get_pair(in);
    }
    else if ( is_ex_al(c) || isalpha(c) ) { //try to get symbol
        ungetc(c,in);
        return get_symbol(in);
    }
    else if (c=='\'') {
        return cons(quote_symbol,cons(read(in),empty_list));
    }
    else if (c!=EOF) {
        return make_warn("Read: invalid grammar!");
    }
    else {
        DEBUG("read:%c",c);
        DEBUG("invalid grammar!\nExit!\n");
        exit(1);
        // return make_warn("Read: invalid grammar!");
    }
    return new_object();
}




/*********************************
*                               *
*                               *
*                               *
*  PART:  Evaluator             *
*                               *
*                               *
*                               *
*********************************/


int is_number(object *exp) {
    return exp->type==INTEGER || exp->type==REAL;
}

// int is_character(object *exp) {
//     return exp->type==CHARACTER;
// }

// int is_boolean(object *exp) {
//     return exp->type==BOOLEAN;
// }

// int is_string(object *exp) {
//     return exp->type==STRING;
// }

int is_self_value(object * exp) {
    return  is_number(exp) || is_character(exp) || is_boolean(exp) || is_string(exp) ;
}

int is_quote(object * exp) {
    return is_symbol(car(exp)) && car(exp)==quote_symbol;
}

int is_variable(object* exp) {
    return is_symbol(exp);
}

object *content_of_quote(object *exp) {
    return cadr(exp);
}

// assignment expression: set!
int is_assignment(object* exp) {
    return is_pair(exp) && is_symbol(car(exp)) && car(exp)==set_symbol;
}

object* assignment_var(object* exp) {
    return cadr(exp);
}

object* assignment_val(object* exp) {
    return caddr(exp);
}

// define expression: define
int is_def(object* exp) {
    return is_pair(exp) && is_symbol(car(exp)) && car(exp)==define_symbol;
}

object* def_var(object* exp) {
    if ( is_symbol(cadr(exp))) {
        return cadr(exp);
    }
    else {
        return caadr(exp);
    }
}

object* def_val(object* exp) {
    if ( is_symbol(cadr(exp)) ) {
        return caddr(exp);
    }
    else {
        DEBUG("def_val_2: type: %d",(cdddr(exp))->type);
        return make_lambda(cdadr(exp),cddr(exp));
    }
}

// if expression: if
int is_if(object* exp) {
    return is_pair(exp) && is_symbol(car(exp)) && car(exp)==if_symbol;
}

object* if_predicate(object* exp) {
    return cadr(exp);
}

object* if_true(object* exp) {
    if (is_empty_list(caddr(exp))) {
        return True;
    }
    else {
        return caddr(exp);
    }
}

object* if_false(object* exp) {
    DEBUG("if_false: exp type: %d",exp->type);
    if (is_empty_list(cdddr(exp))) {
        DEBUG("if_false: alternative is empty\n");
        return False;
    }
    else {
        return cadddr(exp);
    }
}

int is_function(object* exp) {
    return is_pair(exp);
}

object* function(object* exp) {
    return car(exp);
}

object* operands(object* exp) {
    return cdr(exp);
}

object* first_operand(object* exp) {
    return car(exp);
}

object* rest_operands(object* exp) {
    return cdr(exp);
}

object* eval_operand_list(object* exp, object* env) {
    if ( exp==empty_list ) {
        return empty_list;
    }
    else {
        return cons(eval(first_operand(exp), env),
                    eval_operand_list(rest_operands(exp),env));
    }
}

int is_lambda(object* exp) {
    return is_pair(exp) && is_symbol(car(exp)) && car(exp)==lambda_symbol;
}

object* lambda_parameters(object* exp) {
    return cadr(exp);
}

object* lambda_body(object* exp) {
    return cddr(exp);
}

int is_last_exp(object* exp) {
    return is_empty_list(cdr(exp));
}

object* first_exp(object* exp) {
    return car(exp);
}

object* rest_exp(object* exp) {
    return cdr(exp);
}

int is_begin(object* exp) {
    return is_pair(exp) && is_symbol(car(exp)) && car(exp)==begin_symbol;
}

object* begin_body(object* exp) {
    return cdr(exp);
}

int is_cond(object* exp) {
    return is_pair(exp) && is_symbol(car(exp)) && car(exp)==cond_symbol;
}

object* cond_body(object* exp) {
    return cdr(exp);
}

object* cond_predicate(object* exp) {
    return car(exp);
}

object* make_if(object* predicate, object* consequence, object* alternative) {
    return cons(if_symbol, cons(predicate, cons(consequence, cons(alternative, empty_list))));
}

object* make_begin(object* exp) {
    return cons(begin_symbol,exp);
}

object* cond_consequence(object* exp) {
    object* first;
    object* rest;

    first=cadr(exp);
    rest=cddr(exp);
    if ( is_empty_list(rest) ) {
        return first;
    }
    else {
        return make_begin(cdr(exp));
    }
}

int is_cond_else(object* exp) {
    return is_symbol(car(exp)) && car(exp)==else_symbol;
}

object* convert_to_if(object* exp) {
    object* first;
    object* rest;

    if ( is_empty_list(exp) ) {
        return False;
    }
    else { 
        first=first_exp(exp);
        rest=rest_exp(exp);
    
        if ( is_cond_else(first) ) {
            DEBUG("cond else\n");
            if ( is_empty_list(rest) ) {
                return  cond_consequence(first);
            }
            else {
                return make_warn("cond: else is not the last expression");
            }
        }
        else {
            return make_if(cond_predicate(first), cond_consequence(first), convert_to_if(rest));
        }
    }
}

int is_let(object* exp) {
    return is_pair(exp) && is_symbol(car(exp)) && car(exp)==let_symbol; 
}

object* let_bindings_parameters(object* exp) {
    return is_empty_list(exp)? empty_list : cons( caar(exp) , let_bindings_parameters(cdr(exp)) );
}

object* let_bindings_arguments(object* exp) {
    return is_empty_list(exp)? empty_list : cons( cadar(exp) , let_bindings_arguments(cdr(exp)) );
}

object* let_parameters(object* exp) {
    return let_bindings_parameters(cadr(exp));
}

object* let_arguments(object* exp) {
    return let_bindings_arguments(cadr(exp));
}

object* let_body(object* exp) {
    return cddr(exp);
}
object* make_function(object* function, object* operand) {
    return cons( function, operand );
}

int is_apply(object* exp) {
    return is_pair(exp) && is_symbol(car(exp)) && car(exp)==apply_symbol;
}

object* apply_operator(object* exp) {
    if ( is_empty_list(cdr(exp)) ) {
        return make_warn("Exception: invalid syntax (apply)");
    }
    else {
        return cadr(exp);
    }
}

object* apply_operand(object* exp) {
    return cddr(exp);
}

int is_and_or(object* exp) {
    return is_pair(exp) && is_symbol(car(exp)) && (car(exp)==and_symbol || car(exp)==or_symbol);
}

object* eval_and(object* exp, object* env) {
    if ( is_empty_list(exp) ) {
        return True;
    }

    if ( eval(car(exp),env)==False  ) {
        return False;
    }
    else {
        return eval_and(cdr(exp),env);
    }
}

object* eval_or(object* exp, object* env) {
    if ( is_empty_list(exp) ) {
        return False;
    }

    if ( eval(car(exp),env)==True ) {
        return True;
    }
    else {
        return eval_or(cdr(exp),env);
    }
}

object* eval_and_or(object* exp, object* env) {
    if ( car(exp)==and_symbol ) {
        return eval_and(cdr(exp),env);
    }
    else {
        return eval_or(cdr(exp),env);
    }
}

object* eval_assignment(object* exp, object* env) {
    if ( set_var_val(assignment_var(exp), eval(assignment_val(exp),env),env) ) {
        return ok_symbol;
    }
    else {
        return make_warn("Unbounded variable");
    }
}

object* eval_def(object* exp , object* env) {
    if ( is_empty_list(cdr(exp)) || is_empty_list(cddr(exp)) ) {
        return make_warn("Exception: invalid syntax (define)");
    }

    def_var_val(def_var(exp), eval(def_val(exp),env),env);
    return ok_symbol;
}

object* eval(object* exp, object* env) {
    object *proc=NULL;
    object *args=NULL;

    DEBUG("start to evaluate!\n");

    while(1) {

        if ( is_self_value(exp) ) {
            DEBUG("eval: self_value: \n");
            get_type(exp->type);
            return exp;
        }
        else if ( is_variable(exp) ) {
            DEBUG("eval: variable\n");
            return loop_up_env(exp,env);
        }
        else if ( is_quote(exp) ) {
            DEBUG("eval: quote\n");
            return content_of_quote(exp);
        }
        else if ( is_assignment(exp) ) {
            DEBUG("eval: assignment\n");
            return eval_assignment(exp,env);
        }
        else if ( is_def(exp) ) {
            DEBUG("eval: def\n");
            return eval_def(exp,env);
        }
        else if ( is_if(exp) ) {
            DEBUG("eval: if\n");
            exp= is_true(eval(if_predicate(exp),env)) ? if_true(exp):if_false(exp);
        }
        else if ( is_begin(exp) ) {
            DEBUG("eval: begin\n");
            exp = begin_body(exp);
            while( !is_last_exp(exp) ) {
                DEBUG("eval begin: is NOT last expression\n");
                eval(first_exp(exp),env);
                exp=rest_exp(exp);
            }
            exp=first_exp(exp);
        }
        else if( is_cond(exp) ) {
            DEBUG("eval: cond\n");
            if ( is_empty_list(cond_body(exp)) ) {
                return make_warn("Exception: invalid syntax (cond)");
            }
            exp= convert_to_if(cond_body(exp));
        }
        else if( is_let(exp) ) {
            DEBUG("eval: let\n");
            exp = make_function( make_lambda(let_parameters(exp),let_body(exp)),let_arguments(exp));
        }
        else if ( is_lambda(exp) ) {
            DEBUG("eval: lambda\n");
            return make_compound_func(lambda_parameters(exp),lambda_body(exp),env);
        }
        else if ( is_and_or(exp) ) {
            DEBUG("eval: and / or\n");
            return eval_and_or((exp),env);
        }
        else if ( is_apply(exp) ) {
            DEBUG("eval: apply\n");
            proc=apply_operator(exp);
            args=apply_operand(exp);
            exp= cons(proc,args);
        }
        else if ( is_function(exp) ) {
            DEBUG("eval: function\n");
            proc= eval(function(exp), env);
            args= eval_operand_list(operands(exp), env);
            if ( is_builtin_procedure(proc) ) {
                DEBUG("eval: builtin function\n");
                return (proc->data.builtin_proc.func)(args);
            }
            else if ( is_compound_func(proc) ) {
                DEBUG("eval: compound function\n");
                env = extend_env(proc->data.compound_proc.parameters, args, proc->data.compound_proc.env);
                exp=proc->data.compound_proc.body;

                DEBUG("eval: compound function: type: %d\n",car(exp)->type);
                while( !is_last_exp(exp) ) {
                    DEBUG("eval compound function: is NOT last expression , type: %d\n",cdr(exp)->type);
                    eval(first_exp(exp),env);
                    exp=rest_exp(exp);
                }
                exp=first_exp(exp);
            }
            else {
                fprintf(stderr, "cannot eval unknow expression\n");
                exit(1);
            }
        }
        else if ( is_warn(exp) ) {
            return exp;
        }
        else {
            fprintf(stderr, "cannot eval unknow expression\n");
            exit(1);
        }

        if (proc->type==WARN) {
            return proc;
        }

        if (args->type==WARN) {
            return args;
        }
    }
    
    return exp;
}


/*********************************
*                               *
*                               *
*                               *
*  PART:  Printer               *
*                               *
*                               *
*                               *
*********************************/


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
    DEBUG("object type: %d",obj->type);
    DEBUG("start to print!\n");


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
    
    
void init() {
    empty_list=new_object();
    empty_list->type=EMPTY_LIST;

    False=new_object();
    False->type=BOOLEAN;
    False->data.boolean.value='f';
    
    True=new_object();
    True->type=BOOLEAN;
    True->data.boolean.value='t';

    symbol_table=empty_list;
    quote_symbol=make_symbol("quote");
    define_symbol=make_symbol("define");
    set_symbol=make_symbol("set!");
    ok_symbol=make_symbol("ok");
    if_symbol=make_symbol("if");
    lambda_symbol=make_symbol("lambda");
    begin_symbol=make_symbol("begin");
    cond_symbol=make_symbol("cond");
    else_symbol=make_symbol("else");
    let_symbol=make_symbol("let");
    and_symbol=make_symbol("and");
    or_symbol=make_symbol("or");
    apply_symbol=make_symbol("apply");


    empty_environment=empty_list;
    global_environment=setup_env();

    add_function("+",add_procedure);
    add_function("-",sub_procedure);
    add_function("*",mul_procedure);
    add_function("/",div_procedure);
    add_function("=",num_equal_procedure);
    add_function(">",greater_than_procedure);
    add_function("<",less_than_procedure);

    // add_function("and",and_procedure);
    // add_function("or",or_procedure);

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

void sighandler(int signum)
{

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
    signal(SIGINT, sighandler);
    while (1) {
        printf("> ");
        printer(eval(read(stdin), global_environment));
        printf("\n");
    }
        
    return 0;
}







