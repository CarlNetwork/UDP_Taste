#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>


#define MAXBUFFSIZE 4096


int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;

	if( argc != 2 ){
		printf("Format:%s servIPv4addr\n",argv[0]);
		exit(-1);
	}	

	if( (sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0 ){
		perror("socket error\n");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6900);
	if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0 ){
		perror("inet_pton error");
		exit(-1);
	}
	printf("start do_cli\n");
	
	int r_byte, w_byte, n;
	char getbuff[MAXBUFFSIZE], recvbuff[MAXBUFFSIZE];
	socklen_t servlen;

	servlen = sizeof(servaddr);	
	while( (r_byte = read(STDIN_FILENO, getbuff, MAXBUFFSIZE)) ){
		if( r_byte == MAXBUFFSIZE ){
			printf("too much data\n");
		}
		getbuff[r_byte - 1] = '\0'; 
		printf("read return getbuff = %s\n",getbuff);
		n = sendto(sockfd, getbuff, strlen(getbuff), 0
				, (struct sockaddr *)&servaddr, servlen);
		printf("sendto return n = %d\n",n);
		if( n < 0 ){
			perror("sendto error");
			printf("errno = %d\n",errno);
			exit(-1);
		}
		
		n = recvfrom(sockfd, recvbuff, MAXBUFFSIZE, 0,
							NULL, NULL);
		printf("recvfrom return n = %d\n",n);
		write(STDOUT_FILENO, recvbuff, n);
		printf("\nwrite return\n");
	}
	printf("r_byte = %d\n",r_byte);

	close(sockfd);
	exit(EXIT_SUCCESS);
}
