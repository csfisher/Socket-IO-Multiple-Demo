#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <sys/select.h>

using namespace std;

#define MAX_FD 1024
#define SERVER_PORT 18897
#define SERVER_IP "127.0.0.1"

struct server_context
{
	int server_fd;
	vector<int> clientFds;
	int maxClientFd;
};

static struct server_context* context = NULL;

int main()
{
	context = (struct server_context*)malloc(sizeof(struct server_context));
	memset(context, 0, sizeof(struct server_context));
	
	int server_fd;
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		cout<<"socket error"<<endl; return -1;
	}
	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(SERVER_PORT);
	sockaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	
	if(bind(server_fd,(struct sockaddr *) &sockaddr,sizeof(sockaddr)) == -1)
	{
		cout<<"bind error"<<endl; return -1;
	}
	if(listen(server_fd, 10) == -1)
	{
		cout<<"listen error"<<endl; return -1;
	}
	
	// insert server_fd into clientFds
	context->clientFds.push_back(server_fd);
	context->maxClientFd = server_fd;
	
	struct timeval tv;
	fd_set readFds;
	while(1)
	{
		// copy context->clientFds to readFds
		FD_ZERO(&readFds);
		for(int i=0; i<context->clientFds.size(); i++)
		{
			int tmp = context->clientFds[i];
			FD_SET(tmp, &readFds);
			context->maxClientFd = tmp > context->maxClientFd ? tmp : context->maxClientFd;
		}
		// set time limitation
		tv.tv_sec = 20;
		tv.tv_usec = 0;
		/* 
        int select (int __nfds, fd_set *__restrict __readfds,fd_set *__restrict __writefds, 
           fd_set *__restrict __exceptfds,struct timeval *__restrict __timeout); 
        */
        int retval = select(context->maxClientFd+1, &readFds, NULL, NULL, &tv);
        if(!retval)
        {
        	cout<<"select error"<<endl; return -1;
		}
		if(FD_ISSET(server_fd, &readFds))
		{
			// new connection
			int connfd;
			if((connfd = accept(server_fd,(struct sockaddr*)NULL,NULL)) == -1)
			{
				cout<<"accpet error"<<endl;
			}else
			{
				context->clientFds.push_back(connfd);
				context->maxClientFd = connfd > context->maxClientFd ? connfd : context->maxClientFd;
				cout<<"client "<<connfd<<" connect"<<endl;
			}
		}
		// ask clientFd one by one
		char recv_buffer[1024];
		for(int i=1; i<context->clientFds.size(); i++)
		{
			// start from 1, except server_fd at index 0
			if(FD_ISSET(context->clientFds[i], &readFds))
			{
				memset(recv_buffer, 0, 1024);
				retval = recv(context->clientFds[i], recv_buffer, 1024, 0);
				if(!retval)
				{
					// disconnect
					cout<<"client "<<context->clientFds[i]<<" disconnect"<<endl;
					// remove current clientFd from context->clientFds
					vector<int>::iterator it = context->clientFds.begin();
					while(it != context->clientFds.end())
					{
						if(*it == context->clientFds[i])
						{
							context->clientFds.erase(it);
							break;
						}
						else
							it++;
					}
					close(context->clientFds[i]);
				}
				else
				{
					// print message
					cout<<"client "<<context->clientFds[i]<<" :"<<recv_buffer<<endl;
				}
			}
		}		
	}
	close(server_fd);
	
	return 0;
}
