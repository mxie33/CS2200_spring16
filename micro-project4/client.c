#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <arpa/inet.h>

/* The port number you must create a TCP socket at */
const unsigned short PORT = 2200;

/*
 * The main function where you will be performing the tasks described under the
 * client section of the project description PDF
 */
int main(int argc, char **argv)
{
    /* Making sure that the correct number of command line areguments are being passed through */
    if (argc < 3) {
        fprintf(stderr, "client usage: ./client <server IP address> <string to send>\n");
        exit(-1);
    }
    int sockfd;
    struct sockaddr_in server;
    struct in_addr addr;
    char msg[256];
    char reply[256];

    if (argc != 4 && argc != 3) {
        fprintf(stderr, "%s <dotted-address>\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    //here we are checking if the address is valid
    if (inet_aton(argv[1],&addr) == 0) {
        fprintf(stderr, "Invalid address\n");
        exit(EXIT_FAILURE);
    }
    //printf("%u\n",addr.s_addr);

    // get the message
    if (argc == 3) {
        sprintf(msg, "%s", argv[2]);
    } else {
        sprintf(msg, "%s %s",argv[2],argv[3]);
    }

    //create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("ERROR opening socket: %s\n",strerror(errno));
        exit(-1);
    }
    memset(&server,0,sizeof(server));// reset the sockaddrin for server to 0
    // link the server info with its address.
    server.sin_addr.s_addr = addr.s_addr;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // now we are connecting to the server
    if (connect(sockfd,(struct sockaddr*)&server,sizeof(server)) < 0) {
        printf("Connect failed: %s\n",strerror(errno));
        exit(-1);
    }
    printf("Server connected!\n");

    // check if send fail
    if (send(sockfd, msg, 256,0) <= 0) {
        printf("Send failed: %s\n",strerror(errno));
        close(sockfd);
        return 1;
    }
    printf("Send successfully.\n");

    // if send successfully, we will receive a reply from the server
    if (recv(sockfd, reply, 256,0) <= 0) {
        printf("Receive failed: %s\n", strerror(errno));
        exit(-1);
    }
    printf("Server reply recived: %s\n", reply);
    close(sockfd);
    return 0;
}