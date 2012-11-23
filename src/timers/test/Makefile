all: bin

bin:
	gcc ../hpet_cntrs.c ../rdtsc_cntrs.c test.c  -o test -m32 -lrt

clean:
	rm -f ./test

run:
	./test
