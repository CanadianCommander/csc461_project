#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define SEND_BUFF_SIZE 100
#define RECV_BUFF_SIZE 1024
#define SERV_PORT_NO 8080
#define SERV_IP_ADDR "192.168.1.106"
int main(int argc, char **argv)
{
    printf("argc = %d \n", argc);
    int sockfd = 0;
    int  bytesReceived = 0;
    /*int bytesSent = 0;*/
    ssize_t bytesSent = 0;
    
    ssize_t recsize;
    char sendBuff[SEND_BUFF_SIZE];
    char recvBuff[RECV_BUFF_SIZE];
    //memset(recvBuff, '0', sizeof(recvBuff));
    struct sockaddr_in sa;
    socklen_t length;
    
    if((sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    
    /* Zero out socket address */
    memset(&sa, 0, sizeof sa);
    /* Initialize sockaddr_in data structure */
    sa.sin_family = AF_INET;
    sa.sin_port = htons(SERV_PORT_NO); // port
    sa.sin_addr.s_addr = inet_addr(SERV_IP_ADDR);
    length = sizeof sa;
    
    strcpy(sendBuff, argv[1]);
    
    bytesSent = sendto(sockfd, sendBuff, strlen(sendBuff), 0,(struct sockaddr*)&sa, sizeof sa);
}
