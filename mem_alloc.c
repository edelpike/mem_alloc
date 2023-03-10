/*!
 * \file    mem_alloc.c
 * \brief   Memory allocator source
 * \details This module is used for memory allocation from static memory pool
 * \author  Evgenii Shchukin
 * \version 01.00
 * \date    26.01.2023
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
	
	if((MEM_BLK_SIZE < sizeof(NULL_PTR)) || (MEM_POOL_SIZE < MEM_BLK_SIZE))
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

/*!
 * \brief Memory allocator initialization test
 * \details Tests memory pool for blocks number and memory 
 * \param[in] pMem - memory pool structure pointer
 * \param[in] pBuf - static memory pool pointer
 * \return [ERR_NONE, ERR_TEST_INIT]
 */
int mem_alloc_init_test (MemPool_T * pMem, void * pBuf)
{
    unsigned int i;
	unsigned int mem_sum;        // memory summ in bytes
	unsigned int uiBlkNum;       // memory blocks number
    void** ppLink;
    void * pBlk;
	
	uiBlkNum = MEM_POOL_SIZE / MEM_BLK_SIZE;
	
	if(uiBlkNum == 0)
	{
		return ERR_TEST_INIT;
	}
	
	if(uiBlkNum == 1)
	{
		if(pMem->pFreeMem == pBuf)
		{
			return ERR_NONE;
		}
		else
		{
			return ERR_TEST_INIT;
		}
	}
	
	ppLink  = (void **)pMem->pFreeMem;
	i       = 0;
	pBlk    = pBuf;
	mem_sum = 0;
	
	do
    {
        if(*ppLink)       // in the last block we have pointer to the next block == NULL
        {
		    mem_sum += *ppLink - pBlk;
        }
		pBlk     = *ppLink;
        ppLink   = (void **)(*ppLink);
		
		i++;
		if(i > uiBlkNum)
		{
			return 1;
		}
    }
    while(pBlk);
    
    if((i == uiBlkNum) && (mem_sum == MEM_POOL_SIZE - MEM_BLK_SIZE))
    {
        return ERR_NONE;
    }
    else
    {
        return ERR_TEST_INIT;
    }
}

/*!
 * \brief Memory allocate and free tests
 * \details Tests memory pool for allocation and free
 * \param[in] pMem - memory pool structure pointer
 * \param[in] pBuf - static memory pool pointer
 * \return [ERR_NONE, ERR_TEST_MAL_FREE]
 */
int mem_malloc_free_test (MemPool_T *pMem, void * pBuf)
{
    int i;
	unsigned char * pBlk, * pBlkCpy;
	void * pLink, * pLinkBad, * pLinkOk;

    pLink = *(void **)pBuf;

    // Allocate first memory block
	pBlk    = mem_malloc(pMem);
	if(pBlk == NULL_PTR)
	{
	    return ERR_TEST_MAL_FREE;
	}
	pBlkCpy = pBlk;
	
	// Fill first memory block
	for(i = 0; i < MEM_BLK_SIZE; i++)
    {
        *pBlkCpy += i;
        pBlkCpy++;
    }
    
    pLinkBad = *(void **)pBuf;
    
    if(pLink == pLinkBad)
    {
        return ERR_TEST_MAL_FREE;
    }
    
    // Free first memory block
    mem_free(pMem, pBlk);
    
    pLinkOk = *(void **)pBuf;
    
    // We must get the same link as before allocation memory block
    if(pLink != pLinkOk)
    {
        return ERR_TEST_MAL_FREE;
    }
    
    return ERR_NONE;
}
/******************* (C) COPYRIGHT 2023  *********
*
* END OF FILE mem_alloc.c */