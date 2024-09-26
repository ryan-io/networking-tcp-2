#include "server.h"

void App::Server::Start() const {
	m_tcpServer->Start();
}

void App::Server::Stop() const
{
	m_tcpServer->Stop ();
}

void App::Server::Post(std::string&& msg) const
{
	m_tcpServer->Post (std::move(msg));
}
