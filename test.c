/*
example how too use TombPool
 mkdir pack
 cmake ../; make; cd .. ; cp tombpool.h pack; cp test.c pack; cd pack
 sudo cp libtombpool.so /lib
 gcc -o test test.c -lpthread -ltombpool
 ./test

What this it ?
Just another thread pool simple api to work with double linked queue

Author:Antonio Costa aka " Cooler_ "
contact: c00f3r[at]gmail[dot]com

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
#include "tombpool.h"

void tarefa( int a )
{
        
	fprintf(stdout,"%d\n", a);
	
}


int main()
{
	int count = 0;
// the pool struct	
	piscina* threadpool;
// init pool, to fill the pool, alloc heap             
	threadpool = Dig_TombPool ( 10 );       

	while( count < 500 )
	{
// add task in pool and queue...
		Add_Corpse ( threadpool, (void*)tarefa, (void*)count );
		count++;
	}	

// exec all and clear process
	Cover_TombPool ( threadpool , 10 );

	puts("test OK !");	
	return 0;
}
