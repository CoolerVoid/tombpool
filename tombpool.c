/*

What this it ?
Just another thread pool simple api to work with double linked queue

Author:Antonio Costa aka " Cooler_ "
contact: c00f3r[at]gmail[dot]com
contribuitor: MarceloMF - Marcelo M. Fleury
email: marcelo.fleury[at]4linux[dot]com[dot]br

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/resource.h>

#include "tombpool.h"


void no_write_coredump (void) 
{
  struct rlimit rlim;
   
  rlim.rlim_cur = 0; 
  rlim.rlim_max = 0; 
  setrlimit(RLIMIT_CORE, &rlim);
}


#define BUGVIEW 1
// to help debug 
#define DEBUG(x, s...) do { \
 if (!BUGVIEW) { break; } \
 time_t t = time(NULL); \
 char *d = ctime(&t); \
 fprintf(stderr, "%.*s %s[%d] %s(): ", \
 (int)strlen(d) - 1, d, __FILE__, \
 __LINE__, __FUNCTION__); \
 fprintf(stderr, x, ## s); \
} while (0);

static int segura = 1;

// morfopara ajudar no lock da fila
pthread_mutex_t morfo = PTHREAD_MUTEX_INITIALIZER; 

/* Individual job */
typedef struct piscina_tarefa{
	void* arg;                    // argumentos da função
	void* (*function)(void* arg);  // ponteiro para função
	struct piscina_tarefa* proximo;   // proxima tarefa              
	struct piscina_tarefa* anterior;  //tarefa anterior          
}piscina_tarefa;

typedef struct piscina_fila{
	int NumTarefa;         // tarefas na fila
	sem_t *filaSemaforo;  // semaforo para controlar as tarefas
	piscina_tarefa *start;  // inicio da fila
	piscina_tarefa *end; // final da fila
}piscina_fila;

// struct da thread pool
typedef struct piscina{
	int NumThread;   // num threads
	pthread_t* threads;              
	piscina_fila* TarefaFila; // ponteiro apra tarefa
}piscina;

// ponte de dados
typedef struct thread_data{                            
	pthread_mutex_t *mutex_p;
	piscina        *dados;
}thread_data;


// HEAP alignment :-X
void *xmalloc(unsigned int len)
{
	void *ptr;
	
	ptr = malloc ( len );
	if ( ptr == NULL )
		DEBUG("fail malloc");
		
	return ptr;
}

// enche a piscina kkkkk
piscina *Dig_TombPool( int NumThread )
{
	no_write_coredump();
	piscina* dados;
	int count = 0;
	
	if( NumThread < 1 ) 
		NumThread = 1;
// usamos a heap para criar a pool
	dados = xmalloc ( sizeof( piscina ) );                             

// alocamos apra ID agora
	dados->threads = xmalloc ( NumThread * sizeof(pthread_t) );      
	dados->NumThread = NumThread;
	
// iniciamos tarefa de fila
	piscina_fila_init( dados );
	
// alocamos para o semaforo
	dados->TarefaFila->filaSemaforo = xmalloc ( sizeof( sem_t ) );                 
	sem_init( dados->TarefaFila->filaSemaforo , 0, 0 ); 
	
// damos create com pthread

	do{
		
		pthread_create ( & ( dados->threads[count] ), NULL , (void *) poolWheel , (void *) dados ); 
		count++;
		
	}while( count < NumThread );
	
	
	return dados;
}

// roda a fila
void poolWheel( piscina* dados )
{
	do{
//espera ateh que tenha trabalho na fila		
		sem_wait(dados->TarefaFila->filaSemaforo);
   
		if ( segura )
		{			
// passa tarefa para fila e executa
// Hic Sunt Dracones
			void *(*func_buff) (void* arg);
			void* arg_buff;

			piscina_tarefa* job_p;
	
			pthread_mutex_lock( &morfo );               			
			job_p = piscina_fila_peek(dados);
			if ( job_p == NULL )
			{
				piscina_fila_removelast( dados );
				pthread_mutex_unlock( &morfo );
				return;
			}
			
			func_buff = job_p->function;
			arg_buff = job_p->arg;
			piscina_fila_removelast( dados );
			pthread_mutex_unlock( &morfo );                
// roda a função			
			func_buff( arg_buff );               			
			free( job_p );  
			job_p=NULL; 
			segura--;                                                    
		}
		else
		{
			return; 
		}
		
	}while ( segura );
	
	return;
}


// adiciona um trabalho na pool
int Add_Corpse(piscina* dados, void *(*function_p)(void*), void* arg_p)
{
	piscina_tarefa* newJob;
	newJob = xmalloc( sizeof(piscina_tarefa) );                     
	
// adiciona a função e o argumento
	newJob->function = function_p;
	newJob->arg = arg_p;
	
// adiciona tarefa na fila e logo o lock para evitar reentrancia
	pthread_mutex_lock( &morfo );          
	piscina_fila_add( dados, newJob );
	pthread_mutex_unlock( &morfo );                
	
	return 0;
}


// executa as thread e limpa tudo
void Cover_TombPool ( piscina* dados, int NumThread)
{
	int count = 0;
// acabar com thread que nao sai de loop
	segura = NumThread; 

// threads atuais esperam o semaforo
	do{
		
		if ( sem_post(dados->TarefaFila->filaSemaforo) != 0)
			DEBUG ( "not bypass semafore wait,car crash xD!\n" );
		count++;
		
	}while (count < (dados->NumThread) );


 count=0;
// acabamos e executamos
	do{
		pthread_join( dados->threads[count] , NULL );
		count++;
		
	}while( count < (dados->NumThread) );


// destroi o semaforo
	if(sem_destroy( dados->TarefaFila->filaSemaforo ) != 0 )
		DEBUG( "error in destroy semaphore\n" );

// liberamos a heap
	free( dados->threads );
	dados->threads=NULL;                                           
	free( dados->TarefaFila->filaSemaforo );
	dados->TarefaFila->filaSemaforo=NULL;                                        
	free( dados->TarefaFila );           
	dados->TarefaFila=NULL;                                       
	free( dados );         
	dados=NULL;                                                   
}


// inicia fila , aloca heap...
int piscina_fila_init( piscina* dados)
{
	
	dados->TarefaFila= xmalloc( sizeof(piscina_fila) );      
	dados->TarefaFila->end = NULL;
	dados->TarefaFila->start = NULL;
	dados->TarefaFila->NumTarefa = 0;

	return 0;
}


// adiciona tarefa na fila
void piscina_fila_add( piscina* dados , piscina_tarefa* newjob_p)
{ 
	int x = 0;
// setamos para null parte duplamente encadeada
	newjob_p->proximo = NULL;
	newjob_p->anterior = NULL;
	piscina_tarefa *oldFirstJob;
	oldFirstJob = dados->TarefaFila->start;
	
// ajusta os ponteiros das tarefas
	switch( dados->TarefaFila->NumTarefa )
	{
// caso nao tenha tarefas na fila
		case 0:
			dados->TarefaFila->end = newjob_p;
			dados->TarefaFila->start = newjob_p;
			break;

// caso já tenha tarefas na fila		
		default: 	
			oldFirstJob->anterior = newjob_p;
			newjob_p->proximo = oldFirstJob;
			dados->TarefaFila->start = newjob_p;
	}

	(dados->TarefaFila->NumTarefa)++;     
	sem_post( dados->TarefaFila->filaSemaforo );
	
	sem_getvalue(dados->TarefaFila->filaSemaforo, &x);
}


// remove tarefa da fila
int piscina_fila_removelast( piscina* dados )
{
	
	piscina_tarefa *oldLastJob;
	oldLastJob = dados->TarefaFila->end;
	int x = 0;
	
// ajustando tarefas
	switch( dados->TarefaFila->NumTarefa )
	{	
// caso tenha alguma tarefa na fila	
		case 0:     
			return -1;
			break;
// se tem apenas uma tarefa na fila		
		case 1:     
			dados->TarefaFila->end = NULL;
			dados->TarefaFila->start = NULL;
			break;
// caso tenha mais de uma tarefa na fila					
		default:
			oldLastJob->anterior->proximo = NULL;               
			dados->TarefaFila->end = oldLastJob->anterior;
	}
	(dados->TarefaFila->NumTarefa)--;
	sem_getvalue( dados->TarefaFila->filaSemaforo , &x );

	return 0;
}


// pega primeiro elemento da fila
piscina_tarefa* piscina_fila_peek(piscina* dados)
{
	
	return dados->TarefaFila->end;
}
