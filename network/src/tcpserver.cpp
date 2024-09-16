#include "tcpserver.h"
#include "tcpconnection.h"

void Network::TcpServer::Run() {
	// create a new TcpConnection object
	// tell the acceptor to accept new connections

	Log("Starting Tcp server...");

	auto connection = static_cast<TcpCntSharedPtr>(TcpConnection::New(m_context));
	m_connections.emplace_back(connection);

	// we cast this to a shared_pointer because this connection can/will be cached
	// at this point, member variable m_socket of 'connection' is a valid socket
	auto fncBind = std::bind(&TcpServer::AcceptHandler, this, connection, placeholders::error);
	m_acceptor.async_accept(connection->GetSocket(), fncBind);

	Log("Tcp server now accepting connections");
}

void Network::TcpServer::AcceptHandler(const TcpCntSharedPtr& connection, const boost::system::error_code& e) {
	// an error tells us whether to start the connection or not

	if (!e) {
		connection->startRead(1024);
	}

	Run();
}