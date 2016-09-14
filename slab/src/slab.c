/*
 * An simple implementation of Slab Memory Allocator. A logic structure can be 
 * simplified like this:
 *              
 *              |-- kmem_cache 
 *              |
 * kmem_chain --|-- kmem_cache   |-- slabs_full
 *              |                |
 *              |-- kmem_cache --|-- slabs_partial |-- slab
 *                               |                 |
 *                               |-- slabs_empty --|-- slab   |-- chunk
 *                                                 |          |
 *                                                 |-- slab --|-- chunk
 *                                                            |
 *                                                            |-- chunk
 * 
 * Author: Ethan Huang (huangyitian@gwu.edu)
 */
#include "slab.h"

int kmem_cache_init(const double growth_factor, const unsigned int min_cache_size, const unsigned int max_cache_size)
{
	unsigned int size = min_cache_size;
	kmem_cache_t *cc;
	int count = 1;
		
	if(	growth_factor <= 0.0 
		|| min_cache_size < 0 
		|| max_cache_size < 0 
		|| min_cache_size > max_cache_size
	) return -1;
	
	/* initialize queue cache_chain */
	for(; size <= max_cache_size; size *= growth_factor){
		
		/* initialize variable cache_chain */
		if(size == min_cache_size){
			cache_chain = (kmem_cache_t*)malloc(sizeof(kmem_cache_t));
			memset(cache_chain,0,sizeof(kmem_cache_t));
			
			cc = cache_chain;
			cc->size = size;
			cc->id = count++;
			cc->chunks = MAX_CHUNKS;
			
			cc->slabs_full_head= (slabs_t*)malloc(sizeof(slabs_t));
			cc->slabs_partial_head = (slabs_t*)malloc(sizeof(slabs_t));
			cc->slabs_empty_head = (slabs_t*)malloc(sizeof(slabs_t));
			
			continue;
		}		
		
		
		cc->next = (kmem_cache_t*)malloc(sizeof(kmem_cache_t));
		cc = cc->next;
		memset(cc,0,sizeof(kmem_cache_t));
		cc->size = size;
		cc->id = count++;
		cc->chunks = MAX_CHUNKS;
		
		cc->slabs_full_head= (slabs_t*)malloc(sizeof(slabs_t));
		cc->slabs_partial_head = (slabs_t*)malloc(sizeof(slabs_t));
		cc->slabs_empty_head = (slabs_t*)malloc(sizeof(slabs_t));		
	}	
	
	return 0;
}

kmem_cache_t *kmem_cache_create(char* name, size_t size, int align)
{
	kmem_cache_t *cc = cache_chain;
	
	while(cc->size < size){
		cc = cc->next;
	}
	
	return cc;
}

/*
 *  flags 
 */
void *kmem_cache_alloc(kmem_cache_t *cp, int flags)
{
	slab_t *sl;
	int count;
	slab_t* tmp_sl;

	if(cp == NULL) return;
	
	/* the slab is null, means this is the first object stored in this slab */
	if(cp->slabs_partial_head->slab_head == NULL){
		sl = newslab_add(cp->size);
		
		/* No available space */
		if(sl == NULL){
			return NULL;
		} 
		
		cp->slabs++;
		cp->slabs_partial_head->slab_head = sl;
		cp->slabs_partial_head->slab_tail = sl;
	}	
	
	tmp_sl = cp->slabs_partial_head->slab_head;
	
	while(tmp_sl){
		for(count = 0; count < MAX_CHUNKS; count++){
			if(tmp_sl->chunks[count].flag == false){
				memset(tmp_sl->chunks[count].value, 0, sizeof(cp->size));
				tmp_sl->chunks[count].flag = true;
				
				/* check full */
				if(slab_check_full(tmp_sl)){
					slab_queue_remove(cp->slabs_partial_head, tmp_sl);
					slab_queue_add(cp->slabs_full_head, tmp_sl);
				}
				
				return (void*)tmp_sl->chunks[count].value;
			}
		}
		tmp_sl = tmp_sl->next;
	}
	
	/* No available space */
	return NULL;
	
}

void *kmem_cache_free(kmem_cache_t *cp, void *buf)
{	
	int count;
	slab_t *sl;
	
	/* check slab_full_queue */
	if(cp->slabs_full_head != NULL){
		sl = cp->slabs_full_head->slab_head;
		while(sl){
			for(count = 0; count < MAX_CHUNKS; count++){
				if(buf == sl->chunks[count].value){
					sl->chunks[count].flag = false;
					
					/* move this slab from full queue to partial queue */
					slab_queue_remove(cp->slabs_full_head,sl);
					slab_queue_add(cp->slabs_partial_head,sl);
					
					return;					
				}
			}
			sl = sl->next;
		}
	}
	
	/* check slab_partial_queue */
	if(cp->slabs_partial_head != NULL){
		sl = cp->slabs_partial_head->slab_head;
		while(sl){
			for(count = 0; count < MAX_CHUNKS; count++){
				if(buf == sl->chunks[count].value){
					sl->chunks[count].flag = false;
					
					/* 
					 * if this queue becomes empty after free this chunk
					 * remove it from partial queue and free its memory directly.
					 */
					if(slab_check_empty(sl)){
						
						slab_queue_remove(cp->slabs_partial_head,sl);
						slab_free(cp->slabs_partial_head,sl,cp);
					}					
					
					return;					
				}
			}
			sl = sl->next;
		}
	}
	
	return;
	
}

static slab_t* newslab_add(const unsigned int size)
{
	int count;
	
	slab_t *sl;
	
	/* kmem_cache_grow(...) */
	sl = (slab_t*)malloc(sizeof(slab_t));
	
	/* initialize chunks */
	for(count = 0; count < MAX_CHUNKS; count++){
		sl->chunks[count].flag = false;
		sl->chunks[count].value = (char*)malloc(sizeof(size));
		memset(sl->chunks[count].value, 0, sizeof(size));
	}
	
	return sl;	
}

static int slab_queue_add(slabs_t *sls, slab_t *sl)
{
	/* the queue is empty */
	if(sls->slab_head == NULL){
		sls->slab_head = sl;
		sls->slab_tail = sl;
		sl->next = NULL;
		return 0;
	}
	
	/* Or, add this slab to tail */
	sls->slab_tail->next = sl;
	sls->slab_tail = sl;
	sl->next = NULL;
	
	return 0;	
}

static int slab_queue_remove(slabs_t *sls, slab_t *sl)
{
	slab_t* tmp_sl;
	slab_t* tmp_sl2;
	
	/* only one slab */
	if(sls->slab_head == sls->slab_tail){
		sls->slab_head = NULL;
		sls->slab_tail = NULL;
		sl->next = NULL;
		return 0;
	}
	
	/* sl is head */
	if(sls->slab_head == sl){
		
		
		sls->slab_head = sls->slab_head->next;
			
		sl->next = NULL;
		return 0;
	}
	
	tmp_sl = sls->slab_head;
	tmp_sl2 = tmp_sl;
	
	while(tmp_sl != sl){
		tmp_sl2 = tmp_sl;
		tmp_sl = tmp_sl->next;
	}	
	
	/* sl is tail */
	if(sl == sls->slab_tail){
		tmp_sl2->next = NULL;
		//sl->next = NULL;
		return 0;
	}
	
	/* other */
	tmp_sl2->next = sl->next;
	sl->next = NULL;
	return 0;
	
}

static int slab_free(slabs_t *sls, slab_t *sl, kmem_cache_t *cc)
{
	int count;
	
	for(count = 0; count < MAX_CHUNKS; count++){
		free(sl->chunks[count].value);
	}
	
	free(sl);	
	cc->slabs--;
	
	return 0;	
}

static bool slab_check_full(slab_t *sl){
	int count;
	bool res = true;
	
	for(count = 0; count < MAX_CHUNKS; count++){
		if(sl->chunks[count].flag == false){
			res = false;
		}
	}
	
	return res;
}

static bool slab_check_empty(slab_t *sl){
	int count;
	bool res = true;
	
	for(count = 0; count < MAX_CHUNKS; count++){
		if(sl->chunks[count].flag == true){
			res = false;
		}
	}
	
	return res;
}

void statistic(int id)
{		
	kmem_cache_t *cc = cache_chain;
	
	slab_t *sl;
	int count;
	
	while(cc){
		if(cc->id != id && id != 0) {
			cc = cc->next;
			continue;		
		}
		
		printf("cache:%3d, size:%9d, slabs:%3d, chunks:%d\n",cc->id,cc->size,cc->slabs,cc->chunks);
		
		//full
		printf("  |Full slabs:\n");
		
		if(cc->slabs_full_head != NULL){
			sl = cc->slabs_full_head->slab_head;
			while(sl){
				printf("    ||Slab:\n      chunk");
				for(count = 0; count < MAX_CHUNKS; count++){
					printf("[%d]:%s  ",count,sl->chunks[count].flag ? "used":"free");
				}
				printf("\n");
				sl = sl->next;
			}
		}
		//partial
		printf("  |Partial slabs:\n");
		
		if(cc->slabs_partial_head != NULL){ 
			sl = cc->slabs_partial_head->slab_head;
			while(sl){
				printf("    ||Slab:\n      chunk");
				for(count = 0; count < MAX_CHUNKS; count++){
					printf("[%d]:%s  ",count,sl->chunks[count].flag ? "used":"free");
				}
				printf("\n");
				sl = sl->next;
			}
		}
		
		/* no empty slabs because empty slabs will be released immediately. */
		
	
		cc = cc->next;
	}
}




