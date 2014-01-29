# build client executable when user executes "make"
client: client.o
	$(CC) $(LDFLAGS) client.o -o client
client.o: client.c
	$(CC) $(CFLAGS) -c client.c

# remove object files and executable when user executes "make clean"
clean:
	rm *.o client