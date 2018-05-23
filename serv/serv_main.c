#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERV_PORT 6900
#define MAXBUFFSIZE 4096

int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;

	if( argc > 1 ){
		printf("Format:%s\n",argv[0]);
		exit(-1);
	}

	if( (sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0 ){
		perror("socket error");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if( bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ){
		perror("bind error");
		exit(-1);
	}
	printf("start do_serv\n");
	
	int w_byte, r_byte;
	char recvbuff[MAXBUFFSIZE];
	struct sockaddr_in cliaddr;
	socklen_t cliaddrlen;

	for(;;){
		bzero(recvbuff,sizeof(recvbuff));
		memset(&cliaddr, 0 , sizeof(cliaddr));
		cliaddrlen = sizeof(cliaddr);//here is important (errno = 22)
lab:
		r_byte = recvfrom(sockfd, recvbuff, MAXBUFFSIZE, 0,
				 (struct sockaddr *)&cliaddr, &cliaddrlen);
		if( r_byte < 0 ){
			if( errno == EINTR ){
				goto lab;
			}else{
				perror("read error");
				exit(-1);
			}
		}

		printf("recvfrom %s\n",recvbuff);
		w_byte = sendto(sockfd, recvbuff, r_byte, 0,
			 (struct sockaddr *)&cliaddr, cliaddrlen);
		if( w_byte < 0 ){
			perror("sendto error");
			printf("errno = %d\n",errno);
			exit(-1);
		}
		printf("sendto return w_byte = %d\n",w_byte);
	}//end of for
	close(sockfd);
	exit(EXIT_SUCCESS);
}
