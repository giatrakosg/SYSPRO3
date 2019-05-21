#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <iostream>
// We parse the port argument
void getArgs(int argc,char **argv,int *port, char *& port_s) {
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
            {"port"         , required_argument , 0 , 'p'},
            {0, 0, 0, 0}
        };

        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "p:",
        long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;
        switch (c) {
            case 'p':
                port_s = new char[(strlen(optarg) + 1)] ;
                strcpy(port_s,optarg);
                *port = atoi(optarg);
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
    char *port_s ;
    int port ;
    getArgs(argc,argv,&port,port_s);
    printf("%s %d\n",port_s , port );
    return 0;
}
