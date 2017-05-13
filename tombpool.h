/*

What this it ?
Just another thread pool simple api to work with double linked queue

Author:Antonio Costa aka " Cooler_ "
contact: coolerlair[at]gmail[dot]com
contribuitor: MarceloMF - Marcelo M. Fleury
email: marcelo.fleury@4linux.com.br

    Copyright (C) 2012 tombpool authors,
    
    This file is part of tombpool
    
    tombpool is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    tombpool is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef __TOMBPOOL_H__
#define __TOMBPOOL_H__

#include <pthread.h>
#include <semaphore.h>
#include <sys/resource.h>
#include <limits.h>
#include <stdint.h>

#define MUL_NO_OVERFLOW	((size_t)1 << (sizeof(size_t)*4))

struct piscina_tarefa;
struct piscina_fila;
struct piscina;
struct thread_data;
typedef struct piscina_tarefa piscina_tarefa;
typedef struct piscina_fila piscina_fila;
typedef struct piscina piscina;
typedef struct thread_datafala thread_datafala;

// to prevent to write core dump file
void no_write_coredump (void);


void *tombpool_xmalloc ( unsigned int len );

// to init api
piscina* Dig_TombPool ( int NumThread );
void poolWheel ( piscina* dados );

// to add task
int Add_Corpse (piscina* dados, void *(*function_p)(void*), void* arg_p );

// to execute tasks and free heap...
void Cover_TombPool ( piscina* dados,int NumThead );
int piscina_fila_init ( piscina* dados );
void piscina_fila_add ( piscina* dados, piscina_tarefa* newjob_p );
int piscina_fila_removelast ( piscina* dados );
piscina_tarefa* piscina_fila_peek ( piscina* dados );
void *tombpool_xmallocarray (size_t nmemb, size_t size); 

#endif
