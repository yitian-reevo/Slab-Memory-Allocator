MAKE=make
CC=gcc
INCLUDE=src
PROM = sample_slab
SRC = $(INCLUDE)/slab.c sample.c

$(PROM): $(SRC)
	$(CC) -I $(INCLUDE) -o $(PROM) $(SRC)
	
clean:
	rm -rf $(PROM)