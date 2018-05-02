# change application name here (executable output name)
TARGET=dictionary
 
# compiler
CC=gcc

main: src/main.c
	$(CC) -w -o $(TARGET) src/main.c src/dctnr.c src/bt-5.0.0/lib/libbt.a -pthread `pkg-config gtk+-3.0 --cflags --libs` -export-dynamic
run: main
	./dictionary 
clean:
	rm -f dictionary
