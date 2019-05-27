# In order to execute this "Makefile" just type "make"
#

OBJS 	=  main.o
SOURCE	= main.cpp inet_str_client.c
HEADER  =
OUT  	= dropbox_client test_client
CC	= g++
FLAGS   = -g -c -pedantic -ansi  -Wall -Wextra -Wno-missing-field-initializers \
-Wno-unused-parameter -lpthread
# -g option enables debugging mode
# -c flag generates object code for separate files


# create/compile the individual files >>separately<<
test_client : inet_str_client.c
	$(CC) inet_str_client.c -o test_client
dropbox_client : main.o
	$(CC) main.o -o dropbox_client -lpthread
main.o : main.cpp
	$(CC) $(FLAGS) main.cpp
# clean house
all : dropbox_client test_client
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
