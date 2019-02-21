
all : server client-sync client-asyn

run : server client-sync
	./server &
	./client-sync localhost 100000

server : server.c util.h
	gcc -o $@ $@.c -pthread

client-sync : client-sync.c util.h
	gcc -o $@ $@.c -pthread

client-asyn : client-asyn.c util.h
	gcc -o $@ $@.c -pthread


clean :
	rm server client-sync client-asyn