# In order to execute this "Makefile" just type "make"
#

OBJS 	=  main.o Server.o ClientList.o
SOURCE	= main.cpp Server.cpp ClientList.cpp
HEADER  = Server.hpp ClientList.hpp
OUT  	= dropbox_server
CC	= g++
FLAGS   = -g -c -pedantic -ansi  -Wall -Wextra -Wno-missing-field-initializers \
-Wno-unused-parameter
# -g option enables debugging mode
# -c flag generates object code for separate files


# create/compile the individual files >>separately<<
dropbox_server : main.o Server.o ClientList.o
	$(CC) main.o Server.o ClientList.o -o dropbox_server
main.o : main.cpp
	$(CC) $(FLAGS) main.cpp
Server.o : Server.cpp
	$(CC) $(FLAGS) Server.cpp
ClientList.o : ClientList.cpp
	$(CC) $(FLAGS) ClientList.cpp
# clean house
all : dropbox_server
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
