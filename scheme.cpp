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
object *lambda_symbol;
object *empty_environment;
object *global_environment;


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
    temp->type=STRING;
    temp->data.string.value=(char *)malloc(sizeof(char)*(strlen(buffer)+1));
    strcpy(temp->data.string.value,buffer);

    return temp;
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
    fprintf(stderr," unbound variable\n");
    exit(1);
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

void set_var_val(object* var, object* val, object* env) {
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
                return;
            }
            var_list=cdr(var_list);
            val_list=cdr(val_list);
        }
        env=enclosing_env(env);
    }
    fprintf(stderr,"unbound variable\n");
    exit(1);
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
    
    DEBUG("enter skip space!\n");

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

void get_expected_string(FILE *in, const char * expect) {
    char c;
    
    while( *expect!='\0' ) {
        c=getc(in);
        if ( c != *expect ) {
            fprintf(stderr, "unexpected character!\nExit!\n");
            exit(1);
        }
        expect++;
    }
}

void peek_expected_delimiter(FILE *in, const char *expect) {
    char c;
    c=peek(in);
    while ( c != '\0' ) {
        if (c==*expect) {
            return;
        }
        expect++;
    }
    fprintf(stderr,"no expected dilimiter followed!\nExit!\n");
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
            fprintf(stderr,"Symbol name is too long!\nExit!\n");
            exit(1);
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
        fprintf(stderr,"Symbol is not followed by a valid delimiter!\nExit!\n");
        exit(1);
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
        exit(1);
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
                get_expected_string(in,"pace");
                peek_expected_delimiter(in," \n");
                return make_character(' ');
            }
            break;
        case 'n':
            if ( peek(in)=='e' ) {
                get_expected_string(in,"ewline");
                peek_expected_delimiter(in," \n");
                return make_character('\n');
            }
            break;
        case '(':
                peek_expected_delimiter(in," \n");
                return make_character('(');
                break;        
    }
    peek_expected_delimiter(in," \n");
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
    
    DEBUG("enter the read function!\n");

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
                fprintf(stderr, "unknown boolean literal\n");
                exit(1);
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
                fprintf(stderr, "non-terminated string literal\n");
                exit(1);
            }
            if (i < 256 - 1) {
                buffer[i++] = c;
            }
            else {
                fprintf(stderr,
                        "string too long. Maximum length is %d\n",
                        256);
                exit(1);
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
    else {
        DEBUG("read:%c",c);
        DEBUG("invalid grammar!\nExit!\n");
        exit(1);
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
    return caddr(exp);
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

object* operand_list(object* exp, object* env) {
    if ( exp==empty_list ) {
        return empty_list;
    }
    else {
        return cons(eval(first_operand(exp), env),
                    operand_list(rest_operands(exp),env));
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

object* eval_assignment(object* exp, object* env) {
    set_var_val(assignment_var(exp), eval(assignment_val(exp),env),env);
    return ok_symbol;
}

object* eval_def(object* exp, object* env) {
    def_var_val(def_var(exp), eval(def_val(exp),env),env);
    return ok_symbol;
}

object* eval(object* exp, object* env) {
    object *proc=NULL;
    object *args=NULL;

    DEBUG("start to evaluate!\n");

    while(1) {
        if ( is_self_value(exp) ) {
            DEBUG("eval: self_value: %d\n",exp->type);
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
        else if ( is_lambda(exp) ) {
            DEBUG("eval: lambda\n");
            return make_compound_func(lambda_parameters(exp),lambda_body(exp),env);
        }
        else if ( is_function(exp) ) {
            DEBUG("eval: function\n");
            proc= eval(function(exp), env);
            args= operand_list(operands(exp), env);
            if ( is_builtin_procedure(proc) ) {
                DEBUG("eval: builtin function\n");
                return (proc->data.builtin_proc.func)(args);
            }
            else if ( is_compound_func(proc) ) {
                DEBUG("eval: compound function\n");
                env = extend_env(proc->data.compound_proc.parameters, args, proc->data.compound_proc.env);
                exp=proc->data.compound_proc.body;

                DEBUG("eval: compound function: type: %d",car(exp)->type);
                while( !is_last_exp(exp) ) {
                    DEBUG("eval: is NOT last expression , type: %d\n",cdr(exp)->type);
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
        else {
            fprintf(stderr, "cannot eval unknow expression\n");
            exit(1);
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
    lambda_symbol = make_symbol("lambda");

    empty_environment=empty_list;
    global_environment=setup_env();

    add_function("+",add_procedure);
    add_function("-",sub_procedure);
    add_function("*",mul_procedure);
    add_function("/",div_procedure);
    add_function("=",num_equal_procedure);
    add_function(">",greater_than_procedure);
    add_function("<",less_than_procedure);


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

}

void sighandler(int signum)
{
   printf("> ");
}

int main(int argc, char**argv) {

    if ( argc>1 && !strcmp(argv[1],"-d") ) {
        printf("Debug mode!");
        debug=1;
    }
    printf("********* Little_schemer ********\n"
           "ctrl-d to exit.\n");
    
    init();
    signal(SIGINT, sighandler);
    while (1) {
        printf("> ");
        printer(eval(read(stdin), global_environment));
        printf("\n");
    }
        
    return 0;
}







