CC = gcc
CFLAGS = `mysql_config --cflags --libs`

test:
	$(CC) -std=c99 mysql_disk_writes.c -o mysql_disk_writes.o $(CFLAGS) -I. 
