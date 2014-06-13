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
    "5110369009",
    /* First member's full name */
    "Zhang Zichen",
    /* First member's email address */
    "562826179@qq.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

typedef unsigned int UINT;

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*Basic constants and macros */
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 12)
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define PACK(size, alloc) ((size) | (alloc))
#define GET(p) (*(UINT *)(p))
#define PUT(p, val) (*(UINT *)(p) = (val))
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* prev and next free block in list */
#define GET_PREV(bp) (*(UINT *)(bp))
#define GET_NEXT(bp) (*((UINT *)(bp) + 1))
#define SET_PREV(bp, val) (*(UINT *)(bp) = (val))
#define SET_NEXT(bp, val) (*((UINT *)(bp) + 1) = (val))

static char *heap_listp;

/* free list hdrs */
static void *free_listp_16;
static void *free_listp_32;
static void *free_listp_64;
static void *free_listp_128;
static void *free_listp_256;
static void *free_listp_512;
static void *free_listp_1024;
static void *free_listp_2048;
static void *free_listp_4096;
static void *free_listp_etc;

static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void *place(void *bp, size_t asize);
static void *replace(void *bp, size_t asize); //used in realloc

/* seg free list ops. */
static void init_free(void);
static void rm_free(void *bp);
static void add_free(void *bp);
static void **get_free_hdr(size_t asize);
static void **get_larger_hdr(void **pre);


/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    init_free(); //initialize free list

    /* Create the initial empty heap */
    if((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0); //alignment padding
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); //prologue hdr
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); //prologue ftr
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1)); //eplogue hdr
    heap_listp += (2 * WSIZE);

    /* Extend the empty heap with a free block of CHUCKSIZE bytes */
    if(extend_heap(CHUNKSIZE / WSIZE) == NULL) return -1;

    
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{

    size_t asize; //adjusted block size
    size_t extendsize; //amount to extend heap if no fit
    char *bp;

    /* Ignore spurious requests */
    if(size == 0) return NULL;

    /* Adjust block size to include overhead and alignment reqs */
    if(size <= DSIZE) asize = 2 * DSIZE;
    else asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    /* Search the free list for a fit */
    if((bp = find_fit(asize)) != NULL) {
        bp = place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if((bp = extend_heap(extendsize / WSIZE)) == NULL) return NULL;
    bp = place(bp, asize);
    return bp;

}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{

    size_t oldsize = GET_SIZE(HDRP(ptr));
    size_t asize;
    void *newptr;

    if(ptr == NULL) return mm_malloc(size);
    else if(size == 0)
    {
        mm_free(ptr);
        return NULL;
    }

    if(size <= DSIZE) asize = 2 * DSIZE;
    else asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    
    if(asize <= oldsize)
    {
        ptr = replace(ptr, asize);
        return ptr;
    }
    else //next block
    {
    	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(ptr)));
        size_t next_size = GET_SIZE(HDRP(NEXT_BLKP(ptr)));

	    if(!next_alloc && (oldsize + next_size) >= asize)
	    {
            rm_free(NEXT_BLKP(ptr));
            PUT(HDRP(ptr), PACK(oldsize + next_size, 1));
            PUT(FTRP(ptr), PACK(oldsize + next_size, 1));
            return ptr;
        }
        else
        {
            newptr = mm_malloc(size);
            memcpy(newptr, ptr, oldsize);
            mm_free(ptr);
            return newptr;
        }
    }

}

static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if((long)(bp = mem_sbrk(size)) == -1) return NULL;

    /* Initialize free block hdr/ftr and the epilogue hdr */
    PUT(HDRP(bp), PACK(size, 0)); //Free block hdr
    PUT(FTRP(bp), PACK(size, 0)); //Free block ftr
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); //New epilogue hdr

    /* Coalesce if the previous block was free */
    return coalesce(bp);

}

static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(prev_alloc && next_alloc) //101
    {
        add_free(bp);
        //return bp;
    }

    else if(prev_alloc && !next_alloc) //100
    {
        rm_free(NEXT_BLKP(bp));

        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));

        add_free(bp);
    }

    else if(!prev_alloc && next_alloc) //001
    {
        rm_free(PREV_BLKP(bp));        

        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);

        add_free(bp);
    }

    else //000
    {
        rm_free(PREV_BLKP(bp));
        rm_free(NEXT_BLKP(bp));

        size += GET_SIZE(HDRP(NEXT_BLKP(bp))) + 
                GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);

        add_free(bp);
    }

    return bp;
}

static void *find_fit(size_t asize)
{
    void **hp = get_free_hdr(asize);

    while(hp)
    {
        /* First fit search */
        void *bp = *hp;
        while(bp) 
        {
            if(GET_SIZE(HDRP(bp)) >= asize) return bp;
            bp = GET_NEXT(bp);
        }
        hp =  get_larger_hdr(hp);
    }
    return NULL; //No fit
}

static void *place(void *bp, size_t asize)
{
    //place it in the back
    
    size_t csize = GET_SIZE(HDRP(bp));

    if((csize - asize) >= (2 * DSIZE)) //split and alloc
    {
        rm_free(bp);
        PUT(HDRP(bp), PACK((csize - asize), 0));
        PUT(FTRP(bp), PACK((csize - asize), 0));
        add_free(bp);
        
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        return bp;
    }
    else //only alloc and remove
    {
        rm_free(bp);
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
        return bp;
    }
}

/* used in realloc */
static void *replace(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    if((csize - asize) >= (2 * DSIZE))
    {
        //contain old block data
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));

        void *p = NEXT_BLKP(bp);
        PUT(HDRP(p), PACK((csize - asize), 0));
        PUT(FTRP(p), PACK((csize - asize), 0));
    	add_free(p);
        return bp;
    }
    else return bp; //no action

}

static void init_free(void)
{
    free_listp_16 = 0;
    free_listp_32 = 0;
    free_listp_64 = 0;
    free_listp_128 = 0;
    free_listp_256 = 0;
    free_listp_512 = 0;
    free_listp_1024 = 0;
    free_listp_2048 = 0;
    free_listp_4096 = 0;
    free_listp_etc = 0;
}


static void rm_free(void *bp)
{
    //bp->prev->next = bp->next;
    //bp->next->prev = bp->prev;

    size_t asize = GET_SIZE(HDRP(bp));
    void **hp = get_free_hdr(asize);

    UINT *prev = GET_PREV(bp);
    UINT *next = GET_NEXT(bp);
    if(prev) SET_NEXT(prev, next);
    else *hp = next;
    if(next) SET_PREV(next, prev);
}

static void add_free(void *bp)
{
    size_t asize = GET_SIZE(HDRP(bp));
    void **hp = get_free_hdr(asize);
    //printf("%d %p %p\n", asize,  hp, *hp);

    if(*hp) SET_PREV(*hp, bp);
    SET_PREV(bp, 0);
    SET_NEXT(bp, *hp);
    *hp = bp;
}

static void **get_free_hdr(size_t asize)
{
    if(asize <= 16) return &free_listp_16;
    else if(asize <= 32) return &free_listp_32;
    else if(asize <= 64) return &free_listp_64;
    else if(asize <= 128) return &free_listp_128;
    else if(asize <= 256) return &free_listp_256;
    else if(asize <= 512) return &free_listp_512;
    else if(asize <= 1024) return &free_listp_1024;
    else if(asize <= 2048) return &free_listp_2048;
    else if(asize <= 4096) return &free_listp_4096;
    else return &free_listp_etc;
}

static void **get_larger_hdr(void **pre)
{
    if(pre == &free_listp_16) return &free_listp_32;
    else if(pre == &free_listp_32) return &free_listp_64;
    else if(pre == &free_listp_64) return &free_listp_128;
    else if(pre == &free_listp_128) return &free_listp_256;
    else if(pre == &free_listp_256) return &free_listp_512;
    else if(pre == &free_listp_512) return &free_listp_1024;
    else if(pre == &free_listp_1024) return &free_listp_2048;
    else if(pre == &free_listp_2048) return &free_listp_4096;
    else if(pre == &free_listp_4096) return &free_listp_etc;
    else return NULL;
}
