#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>

/* Some constants that you can use */
#define ROT 13
#define BUFFER_SIZE 256

/* The port number you must setup the TCP socket on */
const unsigned short PORT = 2200;

/* Function prototype for the scramble string method */
static void scramble_string(char *str);

/* 
 * The main method. You must fill this out as described in the project description PDF
 */
int main()
{
    // #### YOUR CODE GOES HERE ####
    int sockfd,client_sock;
    struct sockaddr_in server, client;
    char client_msg[255];

    //creat a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("ERROR opening socket: %s\n",strerror(errno));
        exit(-1);
    }
    int tmp = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &tmp,sizeof(tmp));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);


    //bind the server
    if (bind(sockfd,(struct sockaddr*)&server,sizeof(server)) < 0) {
        printf("Bind failed: %s\n",strerror(errno));
        exit(-1);
    }

    //Listen
    if (listen(sockfd , 50) < 0) {// here 50 can be changed
        printf("Litsen failed: %s", strerror(errno));
    } 
    printf("Server waiting...");

    while (1) {
        socklen_t c = sizeof(struct sockaddr*);
        //Accept client's call
        if ((client_sock = accept(sockfd, (struct sockaddr*)&client, &c)) < 0) {
            printf("Accept failed: %s\n", strerror(errno));
            exit(-1);
        } else {

            // Receive client's message
            if (recv(client_sock, client_msg, 256,0) <= 0) {
                printf("Receive failed: %s\n", strerror(errno));
            }

            printf("Client message recieved: %s\n", client_msg);
            scramble_string(client_msg);
            //Send reply after receive the information
            if (send(client_sock, client_msg, 256,0) <= 0) {
                printf("Send failed: %s\n",strerror(errno));
                close(client_sock);
                return 1;
            }
            close(client_sock);
        }
    }

    return 0;
}

/*
 * This function takes in a NULL terminated ASCII, C string and scrambles it
 * using the popular ROT13 cipher. MODIFY AT YOUR OWN RISK
 *
 * @param str C-style string of maximum length 2^31 - 1
 */
static void scramble_string(char *str)
{
    int i;
    char t;
    for (i = 0; str[i]; i++) {
        t = str[i];
        if (t >= 'A' && t <= 'Z') {
            if ((t + ROT) <= 'Z') {
                str[i] += ROT;
            } else {
                str[i] -= ROT;
            }
        } else if (t >= 'a' && t <= 'z') {
            if ((t + ROT) <= 'z') {
                str[i] += ROT;
            } else {
                str[i] -= ROT;
            }
        }
    }
}