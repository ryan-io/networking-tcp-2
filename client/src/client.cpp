#include "client.h"

#include <iostream>

namespace App {
	void Client::Start() const {
		std::cout << "Starting client...\n";
		m_tcpClient->Open();
	}
}