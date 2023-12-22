CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11

copyfile: main.c copyfile.c copyfile.h
	${CC} ${CFLAGS} main.c copyfile.c -o copyfile

testfile:
	dd if=/dev/random of=testfile bs=1G count=1

runBench: copyfile testfile
	./copyfile 1 testfile testfileout
	./copyfile 2 testfile testfileout
	./copyfile 3 testfile testfileout

clean:
	rm -f copyfile testfile testfileout

.PHONY: clean runBench
