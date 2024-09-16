#include "tcpclient.h"

#include <iostream>

void Network::TcpClient::open() {
	if (m_isOpen) {
		std::cout << "Client is already open\n";
		return;
	}

	m_isOpen = true;
	boost::asio::connect(m_socket, m_endpoints);

	// connection to server in theory is now made
	// next logical step is to handle read/write operations
}
