//
//  Server.hpp
//  SYSPRO3
//
//  Created by <author> on 22/05/2019.
//
//

#ifndef Server_hpp
#define Server_hpp

#include <stdio.h>
#include <sys/wait.h>	     /* sockets */
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */
#include <unistd.h>	         /* fork */
#include <stdlib.h>	         /* exit */
#include <ctype.h>	         /* toupper */
#include <signal.h>          /* signal */
#include <string.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <errno.h>

#define TRUE             1
#define FALSE            0


#include "ClientList.hpp"

class Server {
private:
    int port ;
    ClientList list ;
    int send_user_on(long ,short ,long ,short );
    int send_get_clients(int ); // file descriptor of the socket
public:
    Server(int ); // Create a server at listening at port
    void run_server(void) ;
    void run_ibm_server(void);
    ~Server();
protected:

};


#endif /* Server_hpp */
