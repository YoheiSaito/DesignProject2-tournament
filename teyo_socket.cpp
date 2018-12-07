#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/socket.h>   
#include <netdb.h>
#include <errno.h>        
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_HOST const_cast<char*>("localhost")
#define DEFAULT_PORT const_cast<char*>("4444")

namespace teyo{
static int open_connection(char *host, char *service){
	int sock;
	struct addrinfo hints, *res, *ai;
	int err;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if((err = getaddrinfo(host, service, &hints, &res)) != 0){
		throw gai_strerror(err);
	}
	for(ai = res; ai; ai = ai->ai_next){
		sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if(sock < 0)
			continue;
		if(connect(sock, ai->ai_addr, ai->ai_addrlen)<0){
			close(sock);
			continue;
		}
		freeaddrinfo(res);
		return sock;
	}
	freeaddrinfo(res);
	throw "socket(2)/connect(2) failed";
}


};

int main(){
	int sock = teyo::open_connection(DEFAULT_HOST, DEFAULT_PORT);
	char buf[1024] = {0};
	read(sock, buf, sizeof(buf));
	printf("%s", buf);
	char dum;
	while(1){
		int n = scanf("%[^\n]", buf);
		scanf("%c", &dum);
                printf("q:%s\n", buf);
		write(sock, buf, sizeof(buf));
		sleep(0.1);
		read(sock, buf, sizeof(buf));
		printf("a:%s", buf);
                memset(buf, ')',sizeof(buf));
	}
	exit(0);

}
