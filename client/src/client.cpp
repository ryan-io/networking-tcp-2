#include "client.h"

#include <iostream>

namespace App {
	void Client::Start() const {
		std::cout << "Starting client...\n";
		m_tcpClient->Open();
	}

	void Client::Stop() const
	{
		std::cout << "Stopping client...\n";
		m_tcpClient->Close ();
	}

	void Client::Post(std::string&& msg) const
	{
		std::cout << "Posting message...\n";
		m_tcpClient->Post (std::move(msg));
	}
}
