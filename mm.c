/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 *

We use an implicit doubly linked list. Each block has the following format:
________________________________________________________
|size|free_bit|		payload		|size|free_bit|
________________________________________________________
--> Space required:
sizeof(size_t) + size + sizeof(size_t)
Because size_t gets aligned, we store the free_bit in the first (LSB) 3 bits of size --> we get the free bit with:
*(size_t *)(ADRESS_OF_SIZE) & 0x7

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

//definitions for easier pointer arithmetic later.
#define BLOCK_SIZE(size) (ALIGN(SIZE_T_SIZE + size + SIZE_T_SIZE))

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	void *p = mem_heap_lo // initialize whole heap as free block of heapsize
	size_t sz = ALIGN(mem_heapsize());
	*(size_t *)p = sz; // store size of free block
	*(size_t *)p = 1; //it is free
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
	char free = 0;
    int newsize = BLOCK_SIZE(size); //sizeof char because we store the free bit at the start.
	void *s = mem_heap_lo(); //s initialized to list start. in while loop iterates through list.
	void *hi = mem_heap_hi();
	
	//check first separately, so we can use while loop after
	if ((*(size_t *)s & 1) && ((*(size_t *)s & ~0x7) >= size){ // return pointer to place, set free bit to 0, store size of allocated block;
		*(size_t *)s = 0;
		// TODO: store size of block
		// return pointer to start of payload
		}
	
	
	while (*(size_t *)s /= 1){ 
	//loop through implicit list until place found or list full
	
		//TODO: increment pointer s by block size + 2xsizeof(char)


		if ((*(size_t *)s & 1) && ((*(size_t *)s & ~0x7) >= size){
			// TODO: return pointer to payload, set free bit to 0, store size of allocated block;
		}
		else if (s + *(s + SIZE_T_SIZE) >= hi){ //if pointer address + block size are bigger than adress of last byte in memory, allocate new memory with sbrk

		// TODO: store free bit, store size, change return pointer arithmetic to additional free byte and size_t bytes.
			void *p = mem_sbrk(newsize);
		    if (p == (void *)-1)
			return NULL;
		    else {
			*(size_t *)p = size;
			return (void *)((char *)p + SIZE_T_SIZE);
		    }
		}
	}
	
    
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
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














