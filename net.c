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
#define SERV_IP_ADDR "127.0.0.1"

int main()
{
    /*int status = system("./client*/
    
    
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in sa; 
    /*socklen_t length;*/
	char recvBuff[RECV_BUFF_SIZE];
	ssize_t recsize;
	socklen_t fromlen;
	char sendBuff[SEND_BUFF_SIZE];
	int numrv;
	
	memset(sendBuff, 0, sizeof(sendBuff));
	
	memset(&sa, 0, sizeof sa);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(PORT_NO);
	fromlen = sizeof(sa);
	
	if (-1 == bind(sock, (struct sockaddr *)&sa, sizeof sa)) 
	{
		perror("error bind failed");
		close(sock);
		return 1;
  	}
  	
    /*receive file size*/
    /*keep recving untill filesize is recieved properly*/
    /*redundancy with if statement (change to ensure not send empty*/
    /*if don't get right away fail (get rid of do while)*/
    do
    {
        memset(recvBuff, 0, sizeof(recvBuff));
        
        
        recsize = recvfrom(sock, (void*)recvBuff, sizeof recvBuff, 0, (struct sockaddr*)&sa, &fromlen);
        if (recsize < 0) 
		{
			fprintf(stderr, "%s\n", strerror(errno));
			return 1;
		}
        printf("%s\n", recvBuff);
    }while(recsize == 0);
    
    
    fsize = atoi(recvBuff)
    
    int bytesReceived = 0;
    
    memset(recvBuff, 0, sizeof(recvBuff));
    
    /*how to we specify its ok to drop packets as we will get less than filesize*/
    while(fsize > 0)
    {
        bytesReceived = recvfrom(sock, (void*)recvBuff, sizeof recvBuff, 0, (struct sockaddr*)&sa, &fromlen);
        
        /*Need to figure out how to handle what we do with received img parts*/
        /*store recvBuff in larger plane before deleting*/
        
        /*for now we will print what we reveived*/
        printf("recvBuff = %s\n", recvBuff);
        
        
        memset(recvBuff, 0, sizeof(recvBuff));
        
        fsize = fsize - bytesReceived;
        
    }
    
    /*now have received now turn to send*/
    
    /*send filesize*/
    strcpy(sendBuff, "Welcome to BC Ferries");
    
    bytesSent = sendto(sock, sendBuff, strlen(sendBuff), 0, (struct sockaddr*)&sa, sizeof sa);
    
    /*start sending the file piece by piece (1 byte at a time)*/
    /*Assuming filesize is specified in bytes
    In that case subtract filesize by 1 for each packet sent*/
    
    /*for now arbitrary guess (1kb)*/
    fSendSize = 1024;
    
    whie(fSendSize > 0)
    {
        /*Read each part of the file and fill sendBuff then send*/
        bytesSent = sendto(sock, sendBuff, strlen(sendBuff), 0, (struct sockaddr*)&sa, sizeof sa);
    }
    
    
    
    
    
    
}
