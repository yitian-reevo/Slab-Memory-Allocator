#include "slab.h"

#define ECHO(x) printf(x);printf("\n");
#define BR printf("\n")

struct results {
	void* p;
};

unsigned int custom_cache_size;
struct results ins_resuls[20];

void test_alloc(kmem_cache_t *res, int times);
void test_free(kmem_cache_t *res, int id);


int main()
{
	void *p;
	kmem_cache_init(2, 4, 4*1024*1024);
	
	custom_cache_size = 4;
	kmem_cache_t *res = kmem_cache_create("a", custom_cache_size, 0);
	
	//test case 1
	ECHO("---------------------------------");
	ECHO("Test case 1: size 4, alloc() X 1 ");
	ECHO("---------------------------------");	
	test_alloc(res,1);	
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() X 1 ");
	ECHO("---------------------------------");
	test_free(res, 0);
	statistic(1);
	BR;
	
	//test case 2
	ECHO("---------------------------------");
	ECHO("Test case 2: size 4, alloc() X 4 ");
	ECHO("---------------------------------");	
	test_alloc(res,4);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 1st");
	ECHO("---------------------------------");
	test_free(res, 0);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() all");
	ECHO("---------------------------------");
	test_free(res, -1);
	statistic(1);
	BR;
	
	
	//test case 3
	ECHO("---------------------------------");
	ECHO("Test case 3: size 4, alloc() X 5 ");
	ECHO("---------------------------------");	
	test_alloc(res,5);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 1st");
	ECHO("---------------------------------");
	test_free(res, 0);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             alloc(p) X 1");
	ECHO("---------------------------------");
	p = kmem_cache_alloc(res, 0);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free(p)");
	ECHO("---------------------------------");
	kmem_cache_free(res, p);
	p = NULL;
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() all");
	ECHO("---------------------------------");
	test_free(res, -1);
	statistic(1);
	BR;
	
	//test case 4
	ECHO("---------------------------------");
	ECHO("Test case 4: size 4, alloc() X 6 ");
	ECHO("---------------------------------");	
	test_alloc(res,6);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 1st");
	ECHO("---------------------------------");
	test_free(res, 0);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 6th");
	ECHO("---------------------------------");
	test_free(res, 5);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() all");
	ECHO("---------------------------------");
	test_free(res, -1);
	statistic(1);
	BR;
	
	//test case 5
	ECHO("---------------------------------");
	ECHO("Test case 5: size 4, alloc() X 10");
	ECHO("---------------------------------");	
	test_alloc(res,10);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 1st");
	ECHO("---------------------------------");
	test_free(res, 0);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 6th");
	ECHO("---------------------------------");
	test_free(res, 5);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 10th");
	ECHO("---------------------------------");
	test_free(res, 9);
	statistic(1);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() all");
	ECHO("---------------------------------");
	test_free(res, -1);
	statistic(1);
	BR;
	
	//test case 6
	ECHO("---------------------------------");
	ECHO("Test case 5: size 4, alloc() X 20");
	ECHO("---------------------------------");	
	test_alloc(res,20);
	statistic(1);
	BR;
		
	ECHO("---------------------------------");
	ECHO("             free() all");
	ECHO("---------------------------------");
	test_free(res, -1);
	statistic(1);
	BR;
	
	custom_cache_size = 2000;
	res = kmem_cache_create("a", custom_cache_size, 0);
	
	//test case 7
	ECHO("-----------------------------------");
	ECHO("Test case 7: size 2000, alloc() X 1");
	ECHO("-----------------------------------");	
	test_alloc(res,1);	
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() X 1 ");
	ECHO("---------------------------------");
	test_free(res, 0);
	statistic(10);
	BR;
	
	//test case 8
	ECHO("-----------------------------------");
	ECHO("Test case 8: size 2000, alloc() X 4");
	ECHO("-----------------------------------");	
	test_alloc(res,4);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 1st");
	ECHO("---------------------------------");
	test_free(res, 0);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() all");
	ECHO("---------------------------------");
	test_free(res, -1);
	statistic(10);
	BR;
	
	
	//test case 9
	ECHO("-----------------------------------");
	ECHO("Test case 9: size 2000, alloc() X 5");
	ECHO("-----------------------------------");	
	test_alloc(res,5);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 1st");
	ECHO("---------------------------------");
	test_free(res, 0);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             alloc(p) X 1");
	ECHO("---------------------------------");
	p = kmem_cache_alloc(res, 0);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free(p)");
	ECHO("---------------------------------");
	kmem_cache_free(res, p);
	p = NULL;
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() all");
	ECHO("---------------------------------");
	test_free(res, -1);
	statistic(10);
	BR;
	
	//test case 10
	ECHO("-----------------------------------");
	ECHO("Test case 10: size 2000, alloc() X 6");
	ECHO("-----------------------------------");	
	test_alloc(res,6);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 1st");
	ECHO("---------------------------------");
	test_free(res, 0);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 6th");
	ECHO("---------------------------------");
	test_free(res, 5);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() all");
	ECHO("---------------------------------");
	test_free(res, -1);
	statistic(10);
	BR;
	
	//test case 11
	ECHO("-------------------------------------");
	ECHO("Test case 11: size 2000, alloc() X 10");
	ECHO("-------------------------------------");	
	test_alloc(res,10);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 1st");
	ECHO("---------------------------------");
	test_free(res, 0);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 6th");
	ECHO("---------------------------------");
	test_free(res, 5);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() 10th");
	ECHO("---------------------------------");
	test_free(res, 9);
	statistic(10);
	BR;
	
	ECHO("---------------------------------");
	ECHO("             free() all");
	ECHO("---------------------------------");
	test_free(res, -1);
	statistic(10);
	BR;
	
	//test case 12
	ECHO("-------------------------------------");
	ECHO("Test case 12: size 2000, alloc() X 20");
	ECHO("-------------------------------------");	
	test_alloc(res,20);
	statistic(10);
	BR;
		
	ECHO("---------------------------------");
	ECHO("             free() all");
	ECHO("---------------------------------");
	test_free(res, -1);
	statistic(10);
	BR;
	
	//test case 13
	custom_cache_size = 4;
	res = kmem_cache_create("a", custom_cache_size, 0);
	
	ECHO("-------------------------------------");
	ECHO("Test case 13:");
	ECHO("size 4,alloc() X 1");
	ECHO("-------------------------------------");	
	p = kmem_cache_alloc(res,0);
	statistic(1);
	BR;
	
	custom_cache_size = 2000;
	res = kmem_cache_create("a", custom_cache_size, 0);
	
	ECHO("-------------------------------------");
	ECHO("size 2000,alloc() X 1");
	ECHO("-------------------------------------");	
	test_alloc(res,1);
	statistic(1);
	statistic(10);
	BR;
	
		
	ECHO("-------------------------------------");
	ECHO("          free() all");
	ECHO("-------------------------------------");	
	test_free(res,-1);
	res = kmem_cache_create("a", 4, 0);
	kmem_cache_free(res,p);
	statistic(1);
	statistic(10);
	BR;
		
	return 0;

}

void test_alloc(kmem_cache_t *res, int times)
{
	int count;
	void *p;
	
	for(count = 0; count < times; count++){
		p = kmem_cache_alloc(res, 0);
		ins_resuls[count].p = p;
	}
}

void test_free(kmem_cache_t *res, int id){
	int count;
	void *p;
	
	if(id == -1){
		//free all
		for(count = 0; count < 20; count++){
			p = ins_resuls[count].p;
			if(p) kmem_cache_free(res,p);
			
		}
		return;
	}
	
	p = ins_resuls[id].p;
	kmem_cache_free(res,p);
	return;
}