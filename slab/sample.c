/*
 * This is a sample code for using slab memory allocator, briefly demo
 * how to use it. 
 * Author: Ethan Huang (huangyitian@gwu.edu)
 */

#include "slab.h"

#define ECHO(x) printf(x);printf("\n");

int main()
{	
	/* Address of the memory you allocate */
	void *p;
	kmem_cache_t *cc;
	
	/* initialize the cache starts from 4, 4*2, 4*2^2, ..., 4*2^20 */
	kmem_cache_init(2, 4, 4*1024*1024);
	
	/* create the object with name 'a', size '4' and no align */
	cc = kmem_cache_create("a", 4, 0);
	
	/* allocate for memory */
	p = kmem_cache_alloc(cc, 0);
	
	/* statistic of cache after allocating memory 
	 * 1 refers to the first cache, it may be different depending 
	 * on which size of cache you used above.
	 * 0 refers to all caches.
	 */
	ECHO("********* After allocation *********");
	statistic(1);
	ECHO("");
	/* 
	 * use memory here
	 */
	 
	
	/* free memory */
	kmem_cache_free(cc,p);
	
	/* statistic of cache after freeing memory */
	ECHO("********* After free *********");
	statistic(1);
	ECHO("");
	
	
	return 0;
}