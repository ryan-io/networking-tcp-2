#include "tcpserver.h"
#include <iostream>
#include <unordered_set>

#include "tcpconnection.h"
#include "tcpconstants.h"

struct Network::TcpServer::TcpServerImpl
{
	static constexpr int DEFAULT_BUFFER_SIZE = 1024;

	explicit TcpServer::TcpServerImpl (io_context &context) :
		Context (context),
		Acceptor (context, tcp::endpoint (tcp::v4 (), DEFAULT_TCP_PORT))
	{ }

	// data structure for holding all active connections
	std::unordered_set<TcpCntSharedPtr> Connections{};

	// provides core I/O functionality
	io_context &Context;

	// accepts client connections to this server object
	tcp::acceptor Acceptor;

	// used as a placeholder for new sockets that are then std::move
	//	 to the m_connections vector
	// this acts as a sort of dummy socket for the consumer to use
	std::optional<tcp::socket> Socket{};

	/*
	 * Callbacks for when a client joins, leaves, broadcasts a message, or receives a message
	 */
	std::vector<OnJoined> OnJoined;
	std::vector<OnLeft> OnLeft;

	streambuf Buffer{ DEFAULT_BUFFER_SIZE };
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
	m_impl->Context.run ();
}

void Network::TcpServer::Stop () const
{
	Log ("Stopping Tcp server...");
	m_impl->Context.stop ();

}

void Network::TcpServer::RegisterOnJoin (const OnJoined &onJoined) const
{
	m_impl->OnJoined.push_back (onJoined);
}

// broadcast a message to all connected clients
// should be invoked with a rvalue (std::move)
//	 internally, this method uses std::forward
// if std::move is not used, the message will be copied
// due to 'msg' being initialized as a lvalue
void Network::TcpServer::Post (std::string &&msg) const
{
	for (auto &connection : m_impl->Connections)
	{
		const auto msgSend = std::move (msg);
		connection->Post (static_cast<std::string>(std::move (msgSend)));
	}
}

auto Network::TcpServer::Loop () -> void
{
	// the intention here is the socket will be std::move'd into the connection object
	m_impl->Socket.emplace (m_impl->Context);		// this is the socket we will be waiting on
	// a new connection should be created within the acceptor async_accept block
	//		to ensure the connection remains in scope

	// we cast this to a shared_pointer because this connection can/will be cached
	// at this point, member variable m_socket of 'connection' is a valid socket

	m_impl->Acceptor.async_accept (*(m_impl->Socket),
		[this](const boost::system::error_code &ec)
		{
			auto connection = static_cast<TcpCntSharedPtr>(TcpConnection::New (
				std::move (*(m_impl->Socket)),
				TcpServerImpl::DEFAULT_BUFFER_SIZE));

			m_impl->Connections.insert (connection);
			RelayOnJoin (connection);

			TcpConnection::MsgHandler messagehandler = [this, connection](const std::string &msg)
				{
					std::cout << connection->GetName () << ": " << msg;
				};

			// std::weak_ptr for this?
			TcpConnection::ErrHandler errorhandler = [this, ptr = std::weak_ptr (connection)]
			(const boost::system::error_code &e)
				{
					std::cerr << "Error: " << e.what () << '\n';

					// if our connection is still valid AND m_connections contains it
					if (const auto ptrLock = ptr.lock (); ptrLock && m_impl->Connections.erase (ptrLock))
					{
						std::cout << "Connection " << ptrLock->GetName () << " was closed.\n";
						RelayOnLeft (ptrLock);
					}
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
	if (m_impl->OnJoined.empty ())
	{
		return;
	}

	for (auto &callback : m_impl->OnJoined)
	{
		callback (connection);
	}
}

// invokes all onLeft callbacks
void Network::TcpServer::RelayOnLeft (const TcpCntSharedPtr &connection) const
{
	if (m_impl->OnLeft.empty ())
	{
		return;
	}

	for (auto &callback : m_impl->OnLeft)
	{
		callback (connection);
	}
}

Network::TcpServer::~TcpServer () = default;

Network::TcpServer::TcpServer (io_context &context) :
	m_impl (std::make_unique<TcpServerImpl> (context)) { }

void Network::TcpServer::Log (const char *message) { std::cout << message << '\n'; }