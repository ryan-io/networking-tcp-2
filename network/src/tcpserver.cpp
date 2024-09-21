#include "tcpserver.h"
#include <iostream>
#include <unordered_set>

#include "tcpconnection.h"
#include "tcpconstants.h"

struct Network::TcpServer::TcpServerImpl
{
	static constexpr int DEFAULT_BUFFER_SIZE = 1024;

	explicit TcpServer::TcpServerImpl (io_context &context) :
		m_context (context),
		m_acceptor (context, tcp::endpoint (tcp::v4 (), DEFAULT_TCP_PORT))
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

	/*
	 * Callbacks for when a client joins, leaves, broadcasts a message, or receives a message
	 */
	std::vector<OnJoined> m_onJoined;
	std::vector<OnLeft> m_onLeft;

	streambuf m_buffer{ DEFAULT_BUFFER_SIZE };
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
	Loop ();
	Log ("Tcp server now accepting connections");
}

// broadcast a message to all connected clients
// should be invoked with a rvalue (std::move)
//	 internally, this method uses std::forward
// if std::move is not used, the message will be copied
// due to 'msg' being initialized as a lvalue
void Network::TcpServer::Broadcast (std::string &&msg) const
{
	for (auto &connection : m_impl->m_connections)
	{
		connection->Post (std::forward <std::string> (msg));
	}
}

void Network::TcpServer::RegisterOnJoin (const OnJoined &onJoined) const
{
	m_impl->m_onJoined.push_back (onJoined);
}

auto Network::TcpServer::Loop () -> void
{
	m_impl->m_socket.emplace (m_impl->m_context);		// this is the socket we will be waiting on

	// a new connection should be created within the acceptor async_accept block
	//		to ensure the connection remains in scope

	// we cast this to a shared_pointer because this connection can/will be cached
	// at this point, member variable m_socket of 'connection' is a valid socket

	m_impl->m_acceptor.async_accept (*(m_impl->m_socket),
		[this](const boost::system::error_code &ec)
		{
			auto connection = static_cast<TcpCntSharedPtr>(TcpConnection::New (
				std::move (*(m_impl->m_socket)),
				TcpServerImpl::DEFAULT_BUFFER_SIZE));

			m_impl->m_connections.insert (connection);
			RelayOnJoin (connection);

			TcpConnection::MessageHandler messagehandler = [this, connection](const std::string &msg)
				{
					std::cout << "Received message: " << msg << '\n';
				};

			TcpConnection::ErrorHandler errorhandler = [this, connection](const boost::system::error_code &e)
				{
					std::cerr << "Error: " << e.message () << '\n';
				};

			if (!ec)
			{
				connection->Start (std::move (messagehandler), std::move (errorhandler));
			}

			this->Loop ();
		});
}

// invokes all onJoined callbacks
void Network::TcpServer::RelayOnJoin (const TcpCntSharedPtr &connection) const
{
	if (m_impl->m_onJoined.empty ())
	{
		return;
	}

	for (auto &callback : m_impl->m_onJoined)
	{
		callback (connection);
	}
}

// invokes all onLeft callbacks
void Network::TcpServer::RelayOnLeft (const TcpCntSharedPtr &connection) const
{
	if (m_impl->m_onLeft.empty ())
	{
		return;
	}

	for (auto &callback : m_impl->m_onLeft)
	{
		callback (connection);
	}
}

Network::TcpServer::~TcpServer () = default;

Network::TcpServer::TcpServer (io_context &context) :
	m_impl (std::make_unique<TcpServerImpl> (context)) { }

void Network::TcpServer::Log (const char *message) { std::cout << message << '\n'; }