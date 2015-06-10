CC = gcc
CFLAGS = `mysql_config --cflags --libs`

test:
	$(CC) -std=c99 test_mysql.c -o test_mysql.o $(CFLAGS) -I. 
