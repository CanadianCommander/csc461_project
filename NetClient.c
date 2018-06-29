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

#define RECV_BUFF_SIZE 1024
#define SEND_BUFF_SIZE 1024
#define PORT_NO 8080
#define SERV_IP_ADDR "192.168.1.106"

int main()
{

    
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in sa; 
    /*socklen_t length;*/
	char recvBuff[RECV_BUFF_SIZE];
	ssize_t recsize;
	socklen_t fromlen;
	char sendBuff[SEND_BUFF_SIZE];
	int numrv;

    while(1)
    {
    /*send filesize*/
        strcpy(sendBuff, "Welcome to BC Ferries");
        
        int bytesSent = sendto(sock, sendBuff, strlen(sendBuff), 0, (struct sockaddr*)&sa, sizeof sa);
        
        break;
        
        
        /*start sending the file piece by piece (1 byte at a time)*/
        /*Assuming filesize is specified in bytes
        In that case subtract filesize by 1 for each packet sent*/
        
        /*for now arbitrary guess (1kb)*/
        int fSendSize = 1024;
        
        while(fSendSize > 0)
        {
            memset(sendBuff, 0, sizeof(sendBuff));
            
            /*Read each part of the file and fill sendBuff then send*/
            strcpy(sendBuff, "BC Ferres");
            
            bytesSent = sendto(sock, sendBuff, strlen(sendBuff), 0, (struct sockaddr*)&sa, sizeof sa);
            fSendSize = fSendSize - bytesSent;
            
            
        }
        break;
    }
}
