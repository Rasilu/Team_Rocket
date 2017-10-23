/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Team Rocket",
    /* First member's full name */
    "Mario Stöckli",
    /* First member's email address */
    "stmario@student.ethz.ch",
    /* Second member's full name (leave blank if none) */
    "Rasmus Lüscher",
    /* Second member's email address (leave blank if none) */
    "lrasmus@student.ethz.ch"
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
//define size of internal fragmentation
#define INTERNAL_FRAGMENTATION (SIZE_T_SIZE * 2)

/* mask to read free bit or size of block*/
#define FREE_BIT 0x1
#define SIZE_BITS (~FREE_BIT)

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	void *p = mem_heap_lo();
		if (p == NULL)
			return -1;
	assert(mem_heapsize() == 0);
	mem_sbrk(INTERNAL_FRAGMENTATION);
	assert(mem_heapsize() == INTERNAL_FRAGMENTATION);
	*(size_t *)p = mem_heapsize() - INTERNAL_FRAGMENTATION; //size of payload == 0
	assert(*(size_t *)p == 0);
    return *(size_t *)p;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
	void *p = mem_heap_lo(); //First-fit;
	size_t new_block_size = ALIGN(size + INTERNAL_FRAGMENTATION);
	size_t old_size =(*(size_t *)p & SIZE_BITS);
	size_t old_block_size = ALIGN(old_size + INTERNAL_FRAGMENTATION);
	//while p not past heap and block not free or too small
	while (((char *)p < (char *)mem_heap_hi()) && 
	((*(size_t *)p & FREE_BIT) ||  (old_block_size < new_block_size))) {
		old_size = *(size_t *)p & SIZE_BITS;
		old_block_size = ALIGN(old_size + INTERNAL_FRAGMENTATION);
		p = (void *)((char *)p + old_block_size); // go to next block
	}
	if ((char *)p > (char *)mem_heap_hi() ) { //if pointer ist past heap
		p = (void *)((char *)p - old_block_size);
		if (*(size_t *)p & FREE_BIT){
			p = (void *)((char*)mem_sbrk(new_block_size));
		} else { //if last block in heap was free, but size too small
			assert(ALIGN((char *)mem_heap_hi() - (char *)p) == ((char *)mem_heap_hi() + 1 - (char *)p));
			int addsize = new_block_size - ALIGN((char *)mem_heap_hi() - (char *)p);
			mem_sbrk(addsize);
		}
//		assert ((char *)p + new_block_size == (char *)mem_heap_hi() + 1);
    	}
	if (old_block_size > new_block_size){ //if new block is smaller then old block we have to split
		*(size_t *)((char *)p + new_block_size) = old_size - new_block_size;
//		*(size_t *)((char *)p + old_block_size - SIZE_T_SIZE) = old_size - new_block_size;
	}
	*(size_t *)p = size | FREE_BIT;
	*(size_t *)((char *)p + new_block_size - SIZE_T_SIZE) = size | FREE_BIT;
	return (void *)((char *)p + SIZE_T_SIZE);
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
	void *p = (void *)((char *)ptr - SIZE_T_SIZE); //pointer to this block
	size_t sp = ALIGN(*(size_t *)p & SIZE_BITS);
	void *q = (void *)((char *)p  + INTERNAL_FRAGMENTATION + sp); //pointer to next block
	size_t sq = ALIGN(*(size_t *)q & SIZE_BITS);
	if (*(size_t *)q & FREE_BIT){
		*(size_t *)p = sp; //mark this block as free
	} else {
		*(size_t *)p = sp + sq; //coalece this block with next
	}
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














