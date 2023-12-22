CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -pthread
SHELL := bash

counter: counter.c counter.h main.c teller
	${CC} ${CFLAGS} main.c counter.c -o counter

teller:
	dd if=/dev/zero of=teller bs=64K count=$(shell shuf -i 50-2500 -n 1)
	echo "LAUCH" >> teller
	dd if=/dev/random of=teller bs=64K count=$(shell shuf -i 500-2500 -n 1) oflag=append conv=notrunc
	echo "LAUCH" >> teller
	dd if=/dev/zero of=teller bs=64K count=$(shell shuf -i 50-2500 -n 1) oflag=append conv=notrunc

.PHONY: new_teller
new_teller:
	dd if=/dev/zero of=teller bs=64K count=$(shell shuf -i 50-2500 -n 1)
	echo "LAUCH" >> teller
	dd if=/dev/random of=teller bs=64K count=$(shell shuf -i 500-2500 -n 1) oflag=append conv=notrunc
	echo "LAUCH" >> teller
	dd if=/dev/zero of=teller bs=64K count=$(shell shuf -i 50-2500 -n 1) oflag=append conv=notrunc

.PHONY: small_teller
small_teller:
	dd if=/dev/zero of=teller bs=64K count=$(shell shuf -i 1-5 -n 1)
	echo "LAUCH" >> teller
	dd if=/dev/random of=teller bs=64K count=$(shell shuf -i 1-5 -n 1) oflag=append conv=notrunc
	echo "LAUCH" >> teller
	dd if=/dev/zero of=teller bs=64K count=$(shell shuf -i 1-5 -n 1) oflag=append conv=notrunc

.PHONY: clean
clean:
	rm -f counter teller
