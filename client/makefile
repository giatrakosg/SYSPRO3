# In order to execute this "Makefile" just type "make"
#

OBJS 	=  main.o Client.o CircularBuffer.o
SOURCE	= main.cpp Client.cpp CircularBuffer.cpp
HEADER  = Client.hpp CircularBuffer.hpp
OUT  	= dropbox_client
CC	= g++
FLAGS   = -g -c -pedantic -ansi  -Wall -Wextra -Wno-missing-field-initializers \
-Wno-unused-parameter -lpthread -lssl -lcrypto
# -g option enables debugging mode
# -c flag generates object code for separate files


# create/compile the individual files >>separately<<
dropbox_client : main.o Client.o CircularBuffer.o
	$(CC) main.o Client.o CircularBuffer.o -o dropbox_client -lpthread  -lssl -lcrypto
main.o : main.cpp
	$(CC) $(FLAGS) main.cpp
Client.o : Client.cpp
	$(CC) $(FLAGS) Client.cpp
CircularBuffer.o : CircularBuffer.cpp
	$(CC) $(FLAGS) CircularBuffer.cpp


# clean house
all : dropbox_client
clean:
	rm -f $(OBJS) $(OUT) ; rm -r common_cl_* mirror*

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
