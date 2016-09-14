/* slabs memory allocation */
#ifndef SLAB_H
#define SLAB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/queue.h>

/* how many chunks per slab */
#define MAX_CHUNKS 5

/* the smallest unit */
typedef struct chunk{
	bool flag;					/* true: used; false:free */
	char* value;
}chunk_t;

/* Certain number of chunks construct a slab */
typedef struct slab{
	chunk_t chunks[MAX_CHUNKS];
	struct slab *next;
}slab_t;

/* struct to manage slabs. */
typedef struct slabs{
	slab_t *slab_head;
	slab_t *slab_tail;
	struct slabs *next;
}slabs_t;

/* struct of kmem_cache */
typedef struct kmem_cache{
	unsigned int id;			/* identifiers of caches */
    unsigned int size;      	/* sizes of each chunk */
    unsigned int chunks;   		/* how many chunks per slab */
	unsigned int slabs;     	/* how many slabs are currently allocated (full + partial) */
		
    slabs_t *slabs_full_head;	/* address of head of the queue for full/partial/empty slabs */
	slabs_t *slabs_partial_head;
	slabs_t *slabs_empty_head;
	
	struct kmem_cache *next;
	
}kmem_cache_t;


/* Queue for caches, all starts here. */
kmem_cache_t *cache_chain;

/** 
 * Init memory caches. 1st argument the growth factor. The first cache will contain
 * chunks of min_cahce_size (2nd argument); each cache will use a chunk size equal 
 * to the previous cache's chunk size times the growth factor; until the size of chunks 
 * reach max_cache_size (3rd argument). 
 * The expected factor should be at least great than 1.
 * Unit: byte.
 * Return: 0 - success, -1 - fail
 */
int kmem_cache_init(const double growth_factor, const unsigned int min_cache_size, const unsigned int max_cache_size);

/**
 * Given object name, size and align, return the address of cache to use when 
 * allocating/freeing memory for object.
 * Note: <name><align> not support
 */
kmem_cache_t *kmem_cache_create(char* name, size_t size, int align);

/* allocate memory of object return by kmem_Cache_create(...) 
 * Return value: an available address or NULL. 
 * Note: <flags> not support
 */
void *kmem_cache_alloc(kmem_cache_t *cp, int flags);

/* free memory of object return by kmem_Cache_create(...) */
void *kmem_cache_free(kmem_cache_t *cp, void *buf);

/* add a new slab */
static slab_t* newslab_add(const unsigned int size);

/* 
 * add/remove a slab to a queue. 
 * 1st argument refers the address of head of the queue. This queue can be either 
 * slabs_full_queue, or slabs_partial_queue, or slabs_empty_queue
 * 2nd argument refers the address of a slab waiting to be added/removed.
 * Return value: 0 - success; others - fail
 */
static int slab_queue_add(slabs_t *sls, slab_t *sl);
static int slab_queue_remove(slabs_t *sls, slab_t *sl);


static int slab_free(slabs_t *sls, slab_t *sl,kmem_cache_t *cc);

/* 
 * check whether a slab is full/empty or not.
 * the maximum of chunks that a slab can have depends on MAX_CHUNKS defined
 * at the head of this file.
 * Return value: true - is full/empty; false - opposite results.
 */
static bool slab_check_full(slab_t *sl);
static bool slab_check_empty(slab_t *sl);


#endif /* SLAB_H */
