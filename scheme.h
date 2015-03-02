//
//  scheme.h
//  compiler
//
//  Created by 刘智 on 15-1-27.
//  Copyright (c) 2015年 刘智. All rights reserved.
//

#ifndef __compiler__scheme__
#define __compiler__scheme__

#include <iostream>
#include <stdio.h>
#include <vector>

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
            object* (*func)(object *arguments);
        }builtin_proc;
        struct {
            object* parameters;
            object* body;
            object* environment;
        }compound_proc;
    }data;
} object;
    
    
    


object *cons(object* car, object* cdr);
object *car(object* pair);
object *cdr(object* pair);
void set_car(object* obj, object* value);
void set_cdr(object* obj, object* value);





void init();
void emit();


object* new_object();
object* make_character(char c);
object* make_integer(long value);
object* make_real(long f_part,long s_part);
object* make_symbol(const char *buffer);

object* setup_env();



int is_true(object *obj);
int is_false(object *obj);
int is_boolean(object *obj);
int is_empty_list(object *obj);
void set_cdr(object *obj, object* value);
object* cdr(object *pair);
void set_car(object *obj, object* value);
object* car(object *pair);
int is_pair(object *obj);


void get_expected_string(FILE *in, const char * expect);
void peek_expected_delimiter(FILE *in, const char * expect);
object* get_character(FILE *in);
object* get_pair(FILE *in);
object* get_number(FILE *in);



int is_ex_al(char c);
int is_delimiter(int c);



void skip_space(FILE *in);
char peek(FILE *in);
object* read (FILE *in);



object* eval(object* exp);
void printer(object *obj);



    



#endif /* defined(__compiler__scheme__) */
