#include "slab.h"
#include <time.h>

struct plist{
	void* p;
	int size;
	SLIST_ENTRY(plist) plists;
};

int main(int argc,char *argv[]){
	char* buffer = NULL;
    size_t len = 0;
	struct plist* pl; 
	char a;
	char b[10];
	char* temp;
	kmem_cache_t *res;
	char* filename = (char*)malloc(24);
	clock_t t;
    
	
	SLIST_HEAD(listh,plist) head = SLIST_HEAD_INITIALIZER(head); 
	SLIST_INIT(&head);
	
		
	if(argc == 2){
		strcpy(filename, argv[1]);
	}else{
		printf("Wrong input file.\n");
		return -1;
	}
	
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {printf("File error.\n");return -1;}
	
	kmem_cache_init(2, 4, 4*1024*1024);
	
	t = clock();
    
	while ((getline(&buffer, &len, fp)) != -1) {
		//reslove a&b
		temp = buffer;
		a = (char)*temp;
		
		temp++;
		strcpy(b,temp);	
		
		if(!a || !b) continue;
		
		if(a == '+'){
			res = kmem_cache_create("a", atoi(b), 0);
			pl = (struct plist*)malloc(sizeof(struct plist));
			pl->p = kmem_cache_alloc(res,0);
			pl->size = atoi(b);
			SLIST_INSERT_HEAD(&head,pl,plists);
			
		}else{
			pl = SLIST_FIRST(&head);
			res = kmem_cache_create("a",pl->size, 0);
			kmem_cache_free(res,pl->p);
			SLIST_REMOVE_HEAD(&head, plists);
			free(pl); 
		}
    
    }
	
	//printf("Remain in Queue:\n");
	//SLIST_FOREACH(pl, &head, plists){ printf("%d\n",pl->size);}
	t = clock() - t;

    fclose(fp);	
    if(buffer) free(buffer);
	
	double time_taken = 100*((double)t)/CLOCKS_PER_SEC; 
 
    printf("took %f ms.\n", time_taken);
	
	return 0;
}