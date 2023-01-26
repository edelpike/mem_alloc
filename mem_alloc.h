/*!
 * \file    mem_alloc.h
 * \brief   Memory allocator header
 * \details Contains function declarations for memory allocator
 * \author  Evgenii Shchukin
 * \version 01.00
 * \date    25.01.2023
*/
#ifndef _MEM_ALLOC_H_INCLUDED_
#define _MEM_ALLOC_H_INCLUDED_

/*-------------------PARAMETERS----------*/
#define MEM_POOL_SIZE   256  // Memory pool size in bytes
#define MEM_BLK_SIZE    16   // Memory block size in bytes
/*-------------------ERRORS--------------*/
#define ERR_NONE        0    // no errors
#define ERR_OUT_MEM     1    // not enough memory error
/*-------------------MACROS--------------*/
#define NULL_PTR ((void *)0)

typedef struct MemPool
{
    void          *pFreeMem;  // free block pointer
    unsigned int  uiBlkNum;   // memory blocks number
    unsigned int  uiBlkFree;  // free memory blocks number
	unsigned char ucBlkSize;  // memory block size in bytes
}MemPool_T;

/*!
 * \brief Memory allocator initialization
 * \details Takes memory pool, cuts it on equal linked blocks
 * \param[in] pMem - memory pool structure pointer
 * \param[in] pBuf - static memory pool pointer
 * \return [ERR_NONE, ERR_OUT_MEM]
 */
int mem_alloc_init (MemPool_T * pMem, void * pBuf);

/*!
 * \brief Allocate memory
 * \details Returns free memory block pointer from the pool
 * \param[in] pMem - memory pool structure pointer
 * \return free memory block pointer
 */
void * mem_malloc(MemPool_T *pMem);

/*!
 * \brief Free memory
 * \details Frees block memory in the memory pool
 * \param[in] pMem - memory pool structure pointer
 * \param[in] pBlk - memory block pointer to get free
 */
void mem_free(MemPool_T *pMem, void *pBlk);

/*!
 * \brief Get free memory blocks number
 * \details Returns free memory blocks number 
 * \param[in] pMem - memory pool structure pointer
 * \return free memory blocks number
 */
unsigned int mem_avail(MemPool_T *pMem);

#endif /* _MEM_ALLOC_H_INCLUDED_ */

/******************* (C) COPYRIGHT 2023  *********
*
* END OF FILE mem_alloc.h */