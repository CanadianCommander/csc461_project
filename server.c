#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define RECV_BUFF_SIZE 100
#define SEND_BUFF_SIZE 1024
#define PORT_NO 8080
#define SERV_IP_ADDR "127.0.0.1"
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
  	
  	/*int test = 5;*/
  	while(1)
  	{
      	/*receive file size*/
      	memset(recvBuff, 0, sizeof(recvBuff));
            
        recsize = recvfrom(sock, (void*)recvBuff, sizeof recvBuff, 0, (struct sockaddr*)&sa, &fromlen);
      	if (recsize < 0) 
        {
	        fprintf(stderr, "%s\n", strerror(errno));
            return 1;
        }
        printf("%s\n", recvBuff);
      	
      	int fsize = atoi(recvBuff);
      	
      	/*memset(recvBuff, 0, sizeof(recvBuff));*/
      	
      	while(fsize > 0)
      	{
      	    memset(recvBuff, 0, sizeof(recvBuff));
      	    recsize = recvfrom(sock, (void*)recvBuff, sizeof recvBuff, 0, (struct sockaddr*)&sa, &fromlen);
      	    
      	    
      	    
      	    fsize = fsize - recsize;
      	    
      	    printf("recvBuff = %s, fsize = %d\n", recvBuff, fsize);
      	}
      	
      	/*test--;*/
    }
  	
  	
  	/*
  	while(1)
  	{
        memset(recvBuff, 0, sizeof(recvBuff));
        
        recsize = recvfrom(sock, (void*)recvBuff, sizeof recvBuff, 0, (struct sockaddr*)&sa, &fromlen);
        if (recsize < 0) 
		{
			fprintf(stderr, "%s\n", strerror(errno));
			return 1;
		}
        printf("%s\n", recvBuff);
  	}
  	*/

}
