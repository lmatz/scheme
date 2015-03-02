//
//  scheme.cpp
//  compiler
//
//  Created by 刘智 on 15-1-27.
//  Copyright (c) 2015年 刘智. All rights reserved.
//

#include "scheme.h"


extern int indent;







void emit() {
    int loop=indent;
    while(loop--) {
        printf("  ");
    }
}




int debug=0;

object *empty_list;
object *False;
object *True;
object *symbol_table;
object *quote_symbol;
object *define_symbol;
object *set_symbol;
object *ok_symbol;
object *empty_environment;
object *global_environment;

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
    empty_environment=empty_list;
    global_environment=setup_env();
    
    
    quote_symbol=make_symbol("quote");
    
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

object* get_frame(object* env) {
    return car(env);
}

object* var_frame(object* frame) {
    return car(frame);
}

object* val_frame(object* frame) {
    return cdr(frame);
}

object* loop_up_env(object* var, object* env) {
    object* frame;
    object* var_list;
    object* val_list;
    if (!is_empty_env(env) ) {
        frame=get_frame(env);
        var_list=car(frame);
        val_list=cdr(frame);
    }
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


object* setup_env() {
    object * env=NULL;
    
    env=extend_env( empty_list,
                    empty_list,
                   empty_environment);
    return env;
}










/*********************************
*                               *
*                               *
*                               *
*  PART:  Object Manipulation   *
*                               *
*                               *
*                               *
*********************************/

object* new_object() {
    object* temp;
    
    temp=(object *)malloc(sizeof(object));
    if (temp==NULL) {
        fprintf(stderr,"Run out of memory!\nEXIT!\n");
        exit(1);
    }
    
    return temp;
}

// chacacter object
object* make_character(char c) {
    object *temp;
    
    temp=new_object();
    temp->type=CHARACTER;
    temp->data.character.value=c;
    
    return temp;
}


// integer object
object* make_integer(long value) {
    object* temp;
    
    temp=new_object();
    temp->type=INTEGER;
    temp->data.integer.value=value;
    
    return temp;
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

// string
object* make_string(const char* buffer)  {
    object *temp;
    
    temp=new_object();
    temp->type=STRING;
    temp->data.string.value=(char *)malloc(sizeof(char)*(strlen(buffer)+1));
    strcpy(temp->data.string.value,buffer);
    
    if (debug==1) {
        fprintf(stdout,"buffer:%s!\n",buffer);
        fprintf(stdout,"object:%s!\n",temp->data.string.value);
    }
    
    return temp;
}

object* make_symbol(const char *buffer) {
    object *temp;
    
    object *temp_table=symbol_table;
    
    while(  !is_empty_list(temp_table) ) {
        if (   !strcmp(buffer,car(temp_table)->data.symbol.value)    ) {
            return car(temp_table);
        }
        cdr(temp_table);
    }
    
    temp=new_object();
    temp->type=SYMBOL;
    temp->data.symbol.value=(char *)malloc(sizeof(char)*(strlen(buffer)+1));
    if (temp->data.symbol.value == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    strcpy(temp->data.symbol.value,buffer);
    
    
    if (debug==1) {
        fprintf(stdout,"buffer:%s!\n",buffer);
        fprintf(stdout,"object:%s!\n",temp->data.string.value);
    }
    symbol_table = cons(temp, symbol_table);
    
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




/*********************************
*                               *
*                               *
*                               *
*  PART:  Tokenize and Parse    *
*                               *
*                               *
*                               *
*********************************/

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
    }
    if ( isspace(c) ) {
        buffer[offset]='\0';
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
    
    if (debug==1) {
        printf("start to get pair!\n");
    }
    
    skip_space(in);
    c=getc(in);
    if (c==')') {
        return empty_list;
    }
    
    ungetc(c,in);
    
    car=read(in);
    
    skip_space(in);
    
    c=getc(in);
    if ( c=='.' ) {
        if (debug==1) {
            printf("start to get improper list!\n");
        }
        peek_expected_delimiter(in," \n");
        cdr=read(in);
        skip_space(in);
        c=getc(in);
        if ( c!=')' ) {
            fprintf(stderr,"Parentheses don't match!\nExit!\n");
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
        fprintf(stderr,"Invalid float number!\nExit!\n");
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


void skip_space(FILE *in) {
    char c;
    if (debug==1) {
        printf("enter skip space!\n");
    }
    while ( (c = getc(in))!= EOF) {
        if (debug==1) {
            printf("loop: c: %c!\n",c);
        }
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
    if (debug==1) {
        printf("leave skip space!\n");
    }
}


char peek(FILE *in) {
    char c;
    c=getc(in);
    ungetc(c,in);
    return c;
}


object* read (FILE *in) {
    char c;
    char temp;
    
    skip_space(in);
    c=getc(in);
    
    if ( c=='#'  ) { // try to get character
        if (debug==1) {
            printf("character!\n");
        }
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
        if (debug==1) {
            printf("digit!\n");
        }
        ungetc(c,in);
        return get_number(in);
    }
    else if ( c=='"' ) { // try to get string
        if (debug==1) {
            fprintf(stdout,"start to get string!\n");
        }
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
        if (debug==1) {
            fprintf(stderr,"before make string!");
        }
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
        fprintf(stderr,"invalid grammar!\nExit!\n");
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

int is_character(object *exp) {
    return exp->type==CHARACTER;
}

int is_boolean(object *exp) {
    return exp->type==BOOLEAN;
}

int is_string(object *exp) {
    return exp->type==STRING;
}


int is_self_value(object * exp) {
    if ( is_number(exp) || is_character(exp) || is_boolean(exp) || is_string(exp)   ) {
        return 1;
    }
    return 0;
}

int is_quote(object * exp) {
    return exp->data.pair.car==quote_symbol;
}

object *content_of_quote(object *exp) {
    return cadr(exp);
}

object* eval(object* exp) {
    if ( is_self_value(exp) ) {
        return exp;
    }
    else if ( is_quote(exp) ) {
        return content_of_quote(exp);
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
    if (debug==1) {
        printf("start to eval!\n");
    }
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
                fprintf(stdout,"#\\");
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
        case BUITLIN_PROC:
            break;
        case COMPOUND_PROC:
            break;
        case EMPTY_LIST:
            break;
        default:
            fprintf(stderr,"Unknow type!\nExit!\n");
            break;
    }
}
    
