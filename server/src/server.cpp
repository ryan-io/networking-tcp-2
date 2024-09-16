#include "server.h"

#include <iostream>

void App::Server::Start() const {
	m_tcpServer->Run();
	m_context.run();
	std::cout << "Server started\n";
}
