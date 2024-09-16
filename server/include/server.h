#pragma once
#include "tcpserver.h"

namespace App {
	class Server {
	public:
		explicit Server(boost::asio::io_context& context) :
			m_tcpServer(Network::TcpServer::New(context)),
			m_context(context) {}

		void Start() const;

	private:
		Network::TcpSrvPtr m_tcpServer;
		boost::asio::io_context& m_context;
	};
}