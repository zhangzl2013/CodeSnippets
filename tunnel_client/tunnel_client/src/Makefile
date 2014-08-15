client: client.o tunnel.o
	gcc -o client client.o tunnel.o -lssl

client.o: client.c common.h
	gcc -c client.c

tunnel.o: tunnel.c tunnel.h common.h
	gcc -c tunnel.c

clean:
	rm *.o client
