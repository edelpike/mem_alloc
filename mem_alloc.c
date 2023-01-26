/*!
 * \file    mem_alloc.c
 * \brief   Memory allocator source
 * \details This module is used for memory allocation from static memory pool
 * \author  Evgenii Shchukin
 * \version 01.00
 * \date    25.01.2023
 * \warning To use with preemptive multitasking RTOS for each task create separate static memory pool by mem_alloc_init() function
 */
#include "mem_alloc.h"

/*!
 * \brief Memory allocator initialization
 * \details Takes memory pool, cuts it on equal linked blocks
 * \param[in] pMem - memory pool structure pointer
 * \param[in] pBuf - static memory pool pointer
 * \return [ERR_NONE, ERR_OUT_MEM]
 */
int mem_alloc_init (MemPool_T * pMem, void * pBuf)
{
    unsigned int i;
	unsigned int uiBlkNum;       // memory blocks number
    void** ppLink;
    unsigned char * pucBlk;
	
	if((MEM_POOL_SIZE <= sizeof(NULL_PTR)) || (MEM_POOL_SIZE < MEM_BLK_SIZE))
	{
	    return ERR_OUT_MEM;
	}
	
	// we save pointers in the start of each block to the next block inside memory pool
	uiBlkNum = MEM_POOL_SIZE / MEM_BLK_SIZE;		
	
    ppLink = (void**)pBuf;                          // pointer to the start of the first pool memory block 
    pucBlk = (unsigned char *)pBuf + MEM_BLK_SIZE;  // pointer to the start of the next pool memory block
 
    // First bytes of each block have a pointer to the next block
    for(i = 0; i < uiBlkNum - 1; i++)
    {
        *ppLink = (void*)pucBlk;    // set block pointer to the next block
        ppLink  = (void **)pucBlk;  // pointer to the start of the next pool memory block
        pucBlk += MEM_BLK_SIZE;
    }
     
   *ppLink = NULL_PTR; // last pool memory block pointer
     
   pMem->pFreeMem  = pBuf;
   pMem->ucBlkSize = MEM_BLK_SIZE;
   pMem->uiBlkNum  = uiBlkNum;
   pMem->uiBlkFree = uiBlkNum;
   
   return ERR_NONE;
}

/*!
 * \brief Allocate memory
 * \details Returns free memory block pointer from the pool
 * \param[in] pMem - memory pool structure pointer
 * \return free memory block pointer
 */
void * mem_malloc (MemPool_T *pMem)
{
    void **ppBlk;
    if(pMem->uiBlkFree > 0)
    {
        ppBlk = (void **)pMem->pFreeMem;
        pMem->pFreeMem = *ppBlk;
        pMem->uiBlkFree--; 
        return ppBlk;
    }
    else
    {
        return NULL_PTR;
    }
}

/*!
 * \brief Free memory
 * \details Frees block memory in the memory pool
 * \param[in] pMem - memory pool structure pointer
 * \param[in] pBlk - memory block pointer to get free
 */
void mem_free (MemPool_T *pMem, void *pBlk)
{
    unsigned char **ppLink;
    if(pMem->uiBlkFree < pMem->uiBlkNum)
    {
        ppLink         = (unsigned char **)pBlk;
        *ppLink        = pMem->pFreeMem;
        pMem->pFreeMem = pBlk;
        pMem->uiBlkFree++;
    }
}

/*!
 * \brief Get free memory blocks number
 * \details Returns free memory blocks number 
 * \param[in] pMem - memory pool structure pointer
 * \return free memory blocks number
 */
unsigned int mem_avail (MemPool_T *pMem)
{
	return pMem->uiBlkFree;
}

/******************* (C) COPYRIGHT 2023  *********
*
* END OF FILE mem_alloc.c */