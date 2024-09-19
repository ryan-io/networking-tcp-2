#include "tcpserver.h"

#include "tcpconnection.h"

void Network::TcpServer::Run ()
{
// create a new TcpConnection object
// tell the acceptor to accept new connections

	Log ("Starting Tcp server...");
	DoRun ();
	Log ("Tcp server now accepting connections");
}

// broadcast a message to all connected clients
// should be invoked with a rvalue (std::move)
//	 internally, this method uses std::forward
// if std::move is not used, the message will be copied
// due to 'msg' being initialized as a lvalue
auto Network::TcpServer::Broadcast (std::string &&msg) -> void
{
	for (auto &connection : m_connections)
	{
		//connection->Send (std::forward<std::string>(msg));
	}
}

auto Network::TcpServer::DoRun () -> void
{
	std::cout << "TcpServer::DoRun\n";

	m_socket.emplace (m_context);		// this is the socket we will be waiting on

	auto connection = static_cast<TcpCntSharedPtr>(TcpConnection::New (std::move (m_socket.value ())));

	// we cast this to a shared_pointer because this connection can/will be cached
	// at this point, member variable m_socket of 'connection' is a valid socket
	auto fncBind = std::bind (&TcpServer::AcceptHandler, this, connection,
		placeholders::error);
	m_acceptor.async_accept (*m_socket, fncBind);
}

void Network::TcpServer::AcceptHandler (const TcpCntSharedPtr &connection,
	const boost::system::error_code &e)
{
	//m_connections.emplace (connection);		// use of emplace allows constructing in place
	m_connections.insert (connection);	// overload with const ref or rvalue ref
// an error tells us whether to start the connection or not

	if (!e)
	{
		connection->startRead ();
	}

	DoRun ();
}

Network::TcpServer::TcpServer (io_context &context) :
	m_context (context),
	m_acceptor (context,
		tcp::endpoint (tcp::v4 (), DEFAULT_TCP_PORT))
{ }
