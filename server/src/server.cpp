#include "server.h"

#include <iostream>

void App::Server::Start() const {
	m_tcpServer->Start();
	m_context.run();
}

void App::Server::Post(const char* msg) const
{
	m_tcpServer->Post (msg);
}
