#include "Client.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <iostream>

void getArgs(int argc,char **argv,char *& dir_name, short &port,
int & worker_threads,int & buffer_size,short & server_port,char *& server_ip) {
    /* Parse cmd line parameters using getopts
     * Modified example found at :
     * https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html#Getopt-Long-Option-Example
     */
    int c ;
    while (1) {
        static struct option long_options[] =
        {
            /* These options don’t set a flag.
            We distinguish them by their indices. */
            {"dir"         , required_argument , 0 , 'd'},
            {"port"        , required_argument , 0 , 'p'},
            {"wrkthrds"    , required_argument , 0 , 'w'},
            {"b"           , required_argument , 0 , 'b'},
            {"sp"          , required_argument , 0 , 's'},
            {"sip"         , required_argument , 0 , 'i'},
            {0, 0, 0, 0}
        };

        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "d:p:w:b:s:i:",
        long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;
        switch (c) {
            case 'd':
                dir_name = new char[(strlen(optarg) + 1)] ;
                strcpy(dir_name,optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'w':
                worker_threads = atoi(optarg);
                break;
            case 'b':
                buffer_size = atoi(optarg);
                break;
            case 's':
                server_port = atoi(optarg);
                break;
            case 'i':
                server_ip = new char[(strlen(optarg) + 1)] ;
                strcpy(server_ip,optarg);
                break;

            case '?':
            /* getopt_long already printed an error message. */
                break;
            default:
                abort ();
        }
    }
}

int main(int argc, char **argv) {
    char *dir ;
    short port ,server_port ;
    int wrk_ts , buff;
    char * sip ;
    getArgs(argc,argv,dir,port,wrk_ts,buff,server_port,sip);
    printf("%s %d %d %d %d %s\n", dir,port,wrk_ts,buff,server_port,sip);
    return 0;
}