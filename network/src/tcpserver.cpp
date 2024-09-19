#include "tcpserver.h"
#include <iostream>
#include <unordered_set>

#include "tcpconnection.h"
#include "tcpconstants.h"

struct Network::TcpServer::TcpServerImpl
{
	explicit Network::TcpServer::TcpServerImpl(io_context &context) :
		m_context (context),
		m_acceptor (context, tcp::endpoint(tcp::v4 (), DEFAULT_TCP_PORT))
	{ }

	// data structure for holding all active connections
	std::unordered_set<TcpCntSharedPtr> m_connections{};

	// provides core I/O functionality
	io_context &m_context;

	// accepts client connections to this server object
	tcp::acceptor m_acceptor;

	// used as a placeholder for new sockets that are then std::move
	//	 to the m_connections vector
	std::optional<tcp::socket> m_socket{};
};

Network::TcpSrvPtr Network::TcpServer::New (io_context &context)
{
	return TcpSrvPtr (new TcpServer (context));	// cannot use std::make_shared here
}

void Network::TcpServer::Start ()
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
	for (auto &connection : m_impl->m_connections)
	{
		//connection->Send (std::forward<std::string>(msg));
	}
}

auto Network::TcpServer::DoRun () -> void
{
	m_impl->m_socket.emplace (m_impl->m_context);		// this is the socket we will be waiting on

	auto connection = static_cast<TcpCntSharedPtr>(TcpConnection::New (std::move (m_impl->m_socket.value ())));

	// we cast this to a shared_pointer because this connection can/will be cached
	// at this point, member variable m_socket of 'connection' is a valid socket
	auto fncBind = std::bind (
		&TcpServer::HandleConnection,
		this,
		connection,
		placeholders::error);

	m_impl->m_acceptor.async_accept (*(m_impl->m_socket), fncBind);
}

Network::TcpServer::~TcpServer () = default;

void Network::TcpServer::HandleConnection (const TcpCntSharedPtr &connection,
										   const boost::system::error_code &e)
{
	//m_connections.emplace (connection);		// use of emplace allows constructing in place
	m_impl->m_connections.insert (connection);	// overload with const ref or rvalue ref
	// an error tells us whether to start the connection or not

	if (!e)
	{
		Log ("New connection established");
		connection->startRead ();
	}

	DoRun ();
}

Network::TcpServer::TcpServer (io_context &context) :
	m_impl (std::make_unique<TcpServerImpl> (context)) { }

void Network::TcpServer::Log (const char *message) { std::cout << message << '\n'; }
