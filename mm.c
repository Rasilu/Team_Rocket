/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 *

We use an implicit doubly linked list. Each block has the following format:
________________________________________________________
|size|ALLOC_BIT|		payload		|size|ALLOC_BIT|
________________________________________________________
--> Space required:
sizeof(size_t) + size + sizeof(size_t)
Because size_t gets aligned, we store the ALLOC_BIT in the first (LSB) 3 bits of size --> we get the free bit with:
*(size_t *)(ADRESS_OF_SIZE) & 0x1

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
#define ALLOC_BIT 0x1 //renamed FREE_BIT to avoid confusion
#define SIZE_BITS (~ALLOC_BIT)

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
	((*(size_t *)p & ALLOC_BIT) ||  (old_block_size < new_block_size))) {
		old_size = *(size_t *)p & SIZE_BITS;
		old_block_size = ALIGN(old_size + INTERNAL_FRAGMENTATION);
		p = (void *)((char *)p + old_block_size); // go to next block
	}
	if ((char *)p > (char *)mem_heap_hi() ) { //if pointer ist past heap
		p = (void *)((char *)p - old_block_size);
		if (*(size_t *)p & ALLOC_BIT){
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
	*(size_t *)p = size | ALLOC_BIT;
	*(size_t *)((char *)p + new_block_size - SIZE_T_SIZE) = size | ALLOC_BIT;
	return (void *)((char *)p + SIZE_T_SIZE);
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
	void *p = (void *)((char *)ptr - SIZE_T_SIZE); //pointer to this block
	size_t sp = ALIGN(*(size_t *)p & SIZE_BITS) + SIZE_T_SIZE;
	void *q = (void *)((char *)p  + INTERNAL_FRAGMENTATION + sp); //pointer to next block
	size_t sq = ALIGN(*(size_t *)q & SIZE_BITS) + SIZE_T_SIZE;
	void *r = (void *)((char *)p - INTERNAL_FRAGMENTATION); //pointer to previous block
	size_t sr = ALIGN(*(size_t *)r & SIZE_BITS) + SIZE_T_SIZE;
	q = (void *)((char *)q + sq);
	r = (void *)((char *)r - sr);
	
	*(size_t *)p = sp; //mark p-block as free
	*(size_t *)((char *)p + sp) = sp;
	if (q < mem_heap_hi() && r > mem_heap_lo()){
		if (!(*(size_t *)q & ALLOC_BIT)){
			*(size_t *)p = sp + sq; //coalece this block with next
			*(size_t *)q = sp + sq;
	/*
			if (r > mem_heap_lo() && !(*(size_t *)r & ALLOC_BIT)){
				*(size_t *)q = sp + sq + sr + 2*INTERNAL_FRAGMENTATION; //coalece this block with next
				*(size_t *)r = sp + sq + sr + 2*INTERNAL_FRAGMENTATION;
			}
	*/	}
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














