# Makefile v1.0
#
#
#

CC = gcc
CFLAGS = 
LDFLAGS= -lpthread
OBJ = server.o main.o pclient.o list.o pworker.o log.o ptimer.o \
		queue.o pclient_base.o
TAG = p2psrv


$(TAG) : $(OBJ)
	$(CC) $(OBJ) -o $@ ${LDFLAGS}

clean:
	rm -rf $(OBJ)
