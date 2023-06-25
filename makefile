client_sources = Client.c Utilities.c
server_sources = Server.c Utilities.c
client_objs = $(client_sources:.c=.o)
server_objs = $(server_sources:.c=.o)
executables:
	gcc -c Client.c
	gcc -c Server.c
	gcc -c Utilities.c
	gcc -o server $(server_objs)
	gcc -o client $(client_objs)
clean:
	rm server client *.o