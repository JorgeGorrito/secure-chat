client: client.c
	gcc client.c -o client
	
server: server.c libclientlib.a
	gcc server.c -o server -L. -lclientlib

libclientlib.a: clientlib.o
	ar rcs libclientlib.a clientlib.o

clientlib.o: clientlib.c clientlib.h
	gcc -c clientlib.c

clientlib: clientlib.c clientlib.h
	gcc clientlib.c -o clientlib