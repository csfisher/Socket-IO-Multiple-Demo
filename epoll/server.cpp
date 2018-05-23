#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

using namespace std;

#define SERVER_PORT 18897
#define SERVER_IP "127.0.0.1"

int main()
{
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
	// create one epoll fd to spy on all client fd
	int epollFd = epoll_create(1024);
	// add server_fd into epoll
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = server_fd;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, server_fd, &event);

	// event array
	struct epoll_event evs[1024];
	while(1)
	{
		/* 
		int epoll_wait (int __epfd, struct epoll_event *__events, 
               int __maxevents, int __timeout); 
        */
        int retval = epoll_wait(epollFd, evs, 100, -1);
        for(int i=0; i<retval; i++)
		{
			int fd = evs[i].data.fd;
			if((fd == server_fd) && (evs[i].events & EPOLLIN))
			{
				// new connection
				int connfd;
				if((connfd = accept(server_fd,(struct sockaddr*)NULL,NULL)) == -1)
				{
					cout<<"accpet error"<<endl;
				}else
				{
					struct epoll_event e;  
	                e.events = EPOLLIN;  
	                e.data.fd = connfd;
	                // add current clientFd into epollFds
	                epoll_ctl(epollFd, EPOLL_CTL_ADD, connfd, &e);  
	                cout<<"client "<<connfd<<" connect"<<endl;
	                continue;
				}
			}
			if(evs[i].events & EPOLLIN)
			{
				// recv message
				char recv_buffer[1024];
				memset(recv_buffer, 0, 1024);
				retval = recv(evs[i].data.fd, recv_buffer, 1024, 0);
				if(!retval)
				{
					// disconnect
					cout<<"client "<<evs[i].data.fd<<" disconnect"<<endl;
					// remove clientFd
					epoll_ctl(epollFd, EPOLL_CTL_DEL, evs[i].data.fd, &evs[i]);
					close(evs[i].data.fd);
				}
				else
				{
					// print message
					cout<<"client "<<evs[i].data.fd<<" :"<<recv_buffer<<endl;
				}
			}
		}		
	}
	close(server_fd);
	
	return 0;
}
