CC=gcc
CFLAGS=-g2
OBJS=main.o
TARGET=huffman


$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

main.o: main.c prj.h


clean:
	rm *.o
	rm $(TARGET)

