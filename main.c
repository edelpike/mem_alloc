/*! 
 * \file    main.c
 * \brief   Main module
 * \details Creates memory pool and tests it
*/
#include <stdio.h>
#include "mem_alloc.h"

void main (void)
{
    MemPool_T tMemPool;
    unsigned char ucBuf[MEM_POOL_SIZE];
    int i;

	i = mem_alloc_init(&tMemPool, ucBuf);
	if(i)
	{
	    printf("Init memory FAIL");
	    return;
	}
	
	i = mem_alloc_init_test(&tMemPool, ucBuf);
	
	if(i)
	{
	    printf("Init memory test BAD");
	    return;
	}
	else
	{
	    printf("Init memory test OK\n");
	}
	
	i = mem_malloc_free_test(&tMemPool, ucBuf);
	
	if(i)
	{
	    printf("Memory allocate and free tests BAD");
	    return;
	}
	else
	{
	    printf("Memory allocate and free tests OK");
	    return;
	}
}