make : rate_popg.c 
	gcc rate_popg.c - o rate 
run :
	./ rate input.txt 
	gcc - g -o a.out rate_popg.c 
clean : 
	rm - rf rate
