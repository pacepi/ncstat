LIB=-lm
INC=

all :
	@rm -rf ncstat
	@g++ ncstat.c cJSON.c $(LIB) $(INC) -O2 -static -o ncstat

clean :
	rm -rf ncstat
