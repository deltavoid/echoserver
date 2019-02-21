
all : server client

run : server client
	./server &
	./client localhost 100000

server : server.c util.h
	gcc -o server server.c -pthread

client : client.c util.h
	gcc -o client client.c -pthread

clean :
	rm server client