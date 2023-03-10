client: client.c libmessage.a
	gcc client.c -o client -L. -lmessage -lssl -lcrypto
	
server: server.c libclientlib.a libmessage.a
	gcc server.c -o server -L. -lclientlib -lmessage

libclientlib.a: clientlib.o
	ar rcs libclientlib.a clientlib.o

clientlib.o: clientlib.c clientlib.h
	gcc -c clientlib.c

clientlib: clientlib.c clientlib.h
	gcc clientlib.c -o clientlib

libmessage.a: message.o
	ar rcs libmessage.a message.o

message.o:	message.h message.c
	gcc -c message.c

message: message.h message.c
	gcc message.c -o message

test: test.c
	gcc test.c -o test -lssl -lcrypto

clean: 
	rm client server clientlib.o libclientlib.a libmessage.a message.o test