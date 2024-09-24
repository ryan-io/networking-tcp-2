#include "server.h"

#include <iostream>

void App::Server::Start() const {
	m_tcpServer->Start();
	std::cout << "Server started\n";
	m_context.run();
}

void App::Server::Post(const char* msg)
{
	m_tcpServer->Post (msg);
}
