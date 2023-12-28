CC = gcc

CFLAGS = -O1 -Wall -I .
LDLIBS = -lpthread -lm

COMMON_FILES=csapp.c

PROGS =	pthread_attr\
	rwlock\
	echoservert_pre\
	race\
	norace\

all: $(PROGS)

pthread_attr: pthread_attr.c
rwlock: ${COMMON_FILES} rwlock.c
echoservert_pre: ${COMMON_FILES} echoservert_pre.c echo_cnt.c sbuf.c 
race: ${COMMON_FILES} race.c
norace: ${COMMON_FILES} norace.c

clean:
	rm -f $(PROGS) *.o *~
