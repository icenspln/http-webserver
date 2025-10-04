CC = gcc


server: main.o client_handler.o
	$(CC) main.o client_handler.o -o server

main.o: src/main.c
	$(CC) -c src/main.c -o main.o 

client_handler.o: src/client_handler.c
	$(CC) -c src/client_handler.c -o client_handler.o 

clean:
	rm main.o client_handler.o