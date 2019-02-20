
all : server cient

run : server client
	./server &
	./client localhost 100000

server : server.c
	gcc -o server server.c -pthread

client : client.c
	gcc -o client client.c -pthread
