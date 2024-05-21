#include "Server.hpp"

Server::Server()
{
	setup(DEFAULT_PORT);
}

Server::Server(int port)
{
	setup(port);
}

Server::Server(const Server &orig)
{
}

Server::~Server()
{
	close(mastersocket_fd);
}

void Server::setup(int port)
{
	mastersocket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (mastersocket_fd < 0)
	{
		perror("Socket creation failed");
	}

	FD_ZERO(&masterfds);
	FD_ZERO(&tempfds);

	memset(&servaddr, 0, sizeof(servaddr)); // bzero
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(port);

	bzero(input_buffer, INPUT_BUFFER_SIZE); // zero the input buffer before use to avoid random data appearing in first receives
}

void Server::initializeSocket()
{
	int opt_value = 1;
	int ret_test = setsockopt(mastersocket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt_value, sizeof(int));
	if (ret_test < 0)
	{
		perror("[SERVER] [ERROR] setsockopt() failed");
		shutdown();
	}
}

void Server::bindSocket()
{
	int bind_ret = bind(mastersocket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (bind_ret < 0)
	{
		perror("[SERVER] [ERROR] bind() failed");
	}
	FD_SET(mastersocket_fd, &masterfds); // insert the master socket file-descriptor into the master fd-set
	maxfd = mastersocket_fd;			 // set the current known maximum file descriptor count
}

void Server::startListen()
{
	int listen_ret = listen(mastersocket_fd, 3);
	if (listen_ret < 0)
	{
		perror("[SERVER] [ERROR] listen() failed");
	}
}

void Server::shutdown()
{
	int close_ret = close(mastersocket_fd);
}

void Server::handleNewConnection()
{
	socklen_t addrlen = sizeof(client_addr);
	tempsocket_fd = accept(mastersocket_fd, (struct sockaddr *)&client_addr, &addrlen);

	if (tempsocket_fd < 0)
	{
		perror("[SERVER] [ERROR] accept() failed");
	}
	else
	{
		FD_SET(tempsocket_fd, &masterfds);
		// increment the maximum known file descriptor (select() needs it)
		if (tempsocket_fd > maxfd)
		{
			maxfd = tempsocket_fd;
		}
	}
	newConnectionCallback(tempsocket_fd); // call the callback
}

void Server::recvInputFromExisting(int fd)
{
	int nbytesrecv = recv(fd, input_buffer, INPUT_BUFFER_SIZE, 0);
	if (nbytesrecv <= 0)
	{
		// problem
		if (0 == nbytesrecv)
		{
			disconnectCallback((uint16_t)fd);
			close(fd); // well then, bye bye.
			FD_CLR(fd, &masterfds);
			return;
		}
		else
		{
			perror("[SERVER] [ERROR] recv() failed");
		}
		close(fd);				// close connection to client
		FD_CLR(fd, &masterfds); // clear the client fd from fd set
		return;
	}
	receiveCallback(fd, input_buffer);
	bzero(&input_buffer, INPUT_BUFFER_SIZE); // clear input buffer
}

void Server::loop()
{
	tempfds = masterfds; // copy fd_set for select()
	int sel = select(maxfd + 1, &tempfds, NULL, NULL, NULL); // blocks until activity
	if (sel < 0)
	{
		perror("[SERVER] [ERROR] select() failed");
		shutdown();
	}

	// no problems, we're all set

	// loop the fd_set and check which socket has interactions available
	for (int i = 0; i <= maxfd; i++)
	{
		if (FD_ISSET(i, &tempfds))
		{ // if the socket has activity pending
			if (mastersocket_fd == i)
			{
				// new connection on master socket
				handleNewConnection();
			}
			else
			{
				// exisiting connection has new data
				recvInputFromExisting(i);
			}
		} // loop on to see if there is more
	}
}

void Server::init()
{
	initializeSocket();
	bindSocket();
	startListen();
}

void Server::onInput(void (*rc)(uint16_t fd, char *buffer))
{
	receiveCallback = rc;
}

void Server::onConnect(void (*ncc)(uint16_t))
{
	newConnectionCallback = ncc;
}

void Server::onDisconnect(void (*dc)(uint16_t))
{
	disconnectCallback = dc;
}

uint16_t Server::sendMessage(Connector conn, char *messageBuffer)
{
	return send(conn.source_fd, messageBuffer, strlen(messageBuffer), 0);
}

uint16_t Server::sendMessage(Connector conn, const char *messageBuffer)
{
	return send(conn.source_fd, messageBuffer, strlen(messageBuffer), 0);
}
