#include "server.h"

#include <iostream>

void App::Server::Start() const {
	m_tcpServer->Start();
	std::cout << "Server started\n";
	m_context.run();
}
