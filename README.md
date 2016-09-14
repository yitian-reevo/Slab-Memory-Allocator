# Memory allocation
From point of a C programmer's view, memory allocation might be straightforward, it's `malloc()` and `free()`. Os will allocate a suitable memory segment based on the size you apply, then give you the address of this memory segment. When you call `free()`, os identifies its corresponded `malloc()`, and take back the memory. This process is more complicated in real situation, but today we focus on the methodology of Slab Memory Allocation, so we simply forget these details temporarily.

However, frequently memory allocation and free are expensive, which leads to poor performance. Thus we need slab memory allocation (SMA).

# How SMA works
Principle of SMA is simple, if you consider SMA as a process has higher authority than regular processes; this means all regular processes have to communicate to SMA when they need new memory. What SMA does is, when it is initialized, it applies a large memory from OS as a resource pool, divides it into small pieces with fixed sizes and  manage them. When a process needs new memory, SMA allocates a memory segment in its local pool to it and mark this segment in the pool as *used*. When the process frees memory they applied, SMA takes back its memory, mark this memory segment in the pool as *free* (so it can be allocated to other processes), but NOT actually call `free()` function. It's obvious that, by doing this, processes only need to apply for memory one time ideally. Alternatively, SMA doesn't applies a large memory first, instead it applies memory from OS when other processes need to, the key point is when other processes free their memory, **SMA doesn't really call free() function**. Sometimes, we call SMA `cache of caches`.

Certainly, SMA has other features to make it more efficient and I will talk about them later. 

# Implementation
I found a figure on the Internet which is really helpful to understand the basic structure of SMA, and I changed it a bit for my own implementation, which is:



In this figure, all things starts from `cache_chain`, it links memory caches, each cache has three types of slabs queues; each slabs queue contains 0~N slabs and each slab contains a certain number of chunks which is the smallest unit allocated to other processes eventually. Note that different chunks belongs to different cache have different sizes which depend on the parameters when you initialized SMA.

There are three types of slabs queue, `slabs_full` contains slabs that all chunks in slabs are used; `slabs_partial` contains slabs that some chunks in slabs are used and `slabs_empty` contains slabs that all chunks in slabs are not used.

Consider this scenario, we initialize a SMA with parameters `min_size(4), max_size(4*1024*1024), growth_factor(2)`. Then the memory cache #1 will contain chunks with size `4`, cache #2 will contain chunks with size `4*2=8`, cache #3 is `8*2-16`, ..., until cache #N has chunks with size `4M`.

When a process apply for a new memory of 7 Bytes, it firstly resolves an address of the cache about to be used, this cache will have the chunks size of `smallest but larger than 7`. Then SMA checks the `slabs_partial` queue in this cache to find the first chunk that is not used and return its address to the process, mark this chunk as `used`. After that, SMA runs a inner check to check if the slab is full, if YES, move this slab to `slabs_full` queue.

When the process needs to free the memory it applied before. Similarly, SMA locates the cache about to be used, and check its `slabs_full` and `slabs_partial` queues to find out the location of this memory segment and set it as `free`, without call `free()` function. After that, SMA runs a similar check, here we have two situations: (1) the memory segment is in `slabs_full` queue, then move this slab to `slabs_partial` queue; (2) the memory segment is in `slabs_partial` queue, then check if this queue is currently empty, if YES, move it to `slabs_empty` queue.

In my implementation, I ignore the situation about `memory pressure`. So I will free the slab immediately when the slab becomes empty, in other words, the `slabs_empty` queue will always be empty.

## Data structure
```c
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
```

## API
```c
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
```

## Source Code
All source codes including test codes have been upload to [Slab Memory Allocator](https://github.com/wfgydbu/Slab-Memory-Allocator) on GitHub. The source code is in `/src` directory.


# Evaluation
I also write some test codes to test my SMA, both for functionalities and performance. They are all in `/test` directory on GitHub. Execute `/test/compile.sh` to compile all test programs.

## Functionality
Test program `function_slab` is used to test the functionalities covering all normal use cases.

## Performance
Test program `generate [amount]` is used to generate resource file contains two kinds of records `+ Integer` or `-`. The former case indicates applying for random `Integer` size memory while the latter case indicates free a memory applied before. The parameter `amount` refers how many records you'd like to generate, note that numbers of two kinds of records should be identical. For example, use `generate 1000` to generate a resouce fine named `resource_1000` that contains 1000 records.

Test program `performance_slab [filename]` is used to calculate how much time it costs to allocate and free of the records in resource file. For example, `performance_slab resource_100000`.

Test program `performance_no_slab [filename]` is used to calculate how much time it costs to allocate and free of the records in resource file, by using `malloc()` and `free()`. For example, `performance_no_slab resourse_100000`.

Some test results of performance are:

| No. | # of records | time cost with SMA(ms) | time cost without SMA (ms) |
|-----|--------------|------------------------|----------------------------|
| 1   | 10000        | 5.13                   | 150.83                     |
| 2   | 10000        | 11.02                  | 161.38                     |
| 3   | 10000        | 4.73                   | 123.02                     |

We can see, using SMA actually leads to a remarkable improvement of performance.

# References
1. [memcached](https://github.com/memcached/memcached#contributing)
2. [memcached-内存管理机制slab allocator](http://blog.csdn.net/cyningsun/article/details/8782348)
3. [Slab Allocator](https://www.kernel.org/doc/gorman/html/understand/understand011.html)
4. [Slab Allocator内存管理原理](http://blog.sina.com.cn/s/blog_72995dcc01018t2t.html)
5. [Linux内核中内存cache的实现](http://cxw06023273.iteye.com/blog/867312)
6. [linux内核内存分配(struct slab和struct kmem_cache)](http://blog.csdn.net/yuzhihui_no1/article/details/47305361)
