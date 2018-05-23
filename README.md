# socket IO Multiple Demo

## Support
- multiple connections
- disconnect
- message receive

## select
		server.cpp
		client.cpp

function
		
		#include <sys/select.h>
		int select (int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
		// nfds : max fd + 1
		// readfds : readable fd, receive message
		// writefds : writable fd, send message
		// exceptfds: exception fd
		// timeout : time limitation

优缺点

- 监测的文件描述符数量有限，最大支持1024
- 调用select函数，会将fd集合从用户态拷贝到内核态
- 内核使用轮询机制判断fd是否就绪，若就绪，则返回，否则阻塞
- select函数返回后，readfds、writefds、exceptfds中仅保留就绪状态的fd，因此下次调用之前需要重新初始化对应的fds
- 配合FD_ZERO、FD_SET、FD_ISSET等宏定义使用


## poll
		struct pollfd 
	    { 
	        int fd;          File descriptor to poll. 
	        short int events;        Types of events poller cares about. 
	        short int revents;       Types of events that actually occurred. 
	    };

		int poll ( struct pollfd * fds, unsigned int nfds, int timeout);

优缺点

- 无fd集合大小限制
- 使用方法类似于select，仅仅是使用不同的数据结构
		
		if (clients[0].revents & POLLIN)
		{ ... }

## epoll

		#include <sys/epoll.h>
		int epoll_create (int size);
		int epoll_ctl (int epfd, int op, int fd, struct epoll_event *event);
		int epoll_wait (int epfd, struct epoll_event *events, int maxevents, int timeout);

优缺点

- 使用一个epoll描述符管理多个client描述符
- client fd增加、删除管理方便
- 内核使用回调的方式监测fd是否就绪，而非轮询
- 效率更高



		
 