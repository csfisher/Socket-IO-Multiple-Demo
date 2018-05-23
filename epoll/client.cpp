#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

#define LOCAL_IP "127.0.0.1"
#define LOCAL_PORT 18897

int main()
{   
    int socketfd;
	struct sockaddr_in sockaddr;
	if((socketfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		cout<<"socket error"<<endl; return -1;
	}
	memset(&sockaddr,0,sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(LOCAL_PORT);
	sockaddr.sin_addr.s_addr = inet_addr(LOCAL_IP);

	if((connect(socketfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))) == -1)
	{
		cout<<"connect error"<<endl; return -1;
	}
	char buf[6];
	memset(buf, 'a', 5);
	buf[5] = '\0';
	
	while(1)
	{
		send(socketfd, buf, 6, 0);
		sleep(5);
	}
    close(socketfd);
	return 0;
}
