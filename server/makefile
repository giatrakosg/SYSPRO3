# In order to execute this "Makefile" just type "make"
#

OBJS 	=  main.o Server.o
SOURCE	= main.cpp Server.cpp
HEADER  = Server.hpp
OUT  	= dropbox_server
CC	= g++
FLAGS   = -g -c -pedantic -ansi  -Wall -Wextra -Wno-missing-field-initializers \
-Wno-unused-parameter
# -g option enables debugging mode
# -c flag generates object code for separate files


# create/compile the individual files >>separately<<
dropbox_server : main.o Server.o
	$(CC) main.o Server.o -o dropbox_server
main.o : main.cpp
	$(CC) $(FLAGS) main.cpp
Server.o : Server.cpp
	$(CC) $(FLAGS) Server.cpp

# clean house
all : dropbox_server
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
