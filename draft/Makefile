all: DPW_matcher DPW_backup DPW_forwarder DPW_generator DPW_receiver

stockex.o: stockex.h stockex.c
	gcc -c -o stockex.o stockex.c

NTU_socket.o: NTU_socket.c
	gcc -c NTU_socket.c -o NTU_socket.o

DPW_matcher: DPW_matcher.c NTU_connection.o NTU_socket.o stockex.o
	gcc DPW_matcher.c NTU_socket.o NTU_connection.o stockex.o -pthread `pkg-config --cflags --libs glib-2.0` -o DPW_matcher

DPW_backup: DPW_backup.c NTU_connection.o NTU_socket.o stockex.o
	gcc DPW_backup.c NTU_socket.o NTU_connection.o stockex.o -pthread -o DPW_backup

DPW_forwarder: DPW_forwarder.c NTU_connection.o NTU_socket.o stockex.o
	gcc DPW_forwarder.c NTU_socket.o NTU_connection.o stockex.o -pthread -o DPW_forwarder

DPW_generator: DPW_generator.c NTU_connection.o NTU_socket.o stockex.o
	gcc DPW_generator.c NTU_socket.o NTU_connection.o stockex.o -pthread -o DPW_generator

DPW_receiver: DPW_receiver.c NTU_connection.o NTU_socket.o stockex.o
	gcc DPW_receiver.c NTU_socket.o NTU_connection.o stockex.o -pthread -o DPW_receiver
NTU_connection.o: NTU_connection.c
	gcc -c -o NTU_connection.o NTU_connection.c

clean:
	rm -f *.o
	rm -f DPW_matcher
	rm -f DPW_backup
	rm -f DPW_forwarder
	rm -f DPW_generator
	rm -f DPW_receiver
