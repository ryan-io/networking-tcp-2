#include <iostream>
#include <unordered_set>
#include "tcpserver.h"
#include "tcpconnection.h"
#include "tcpconstants.h"
#include "tcplogging.h"

#pragma region PIMPL_IDIOM

struct Network::TcpServer::TcpServerImpl
{
	static constexpr int DEFAULT_BUFFER_SIZE = 1024;

	explicit TcpServer::TcpServerImpl (const unsigned short port, const int bufferSize, const TcpLogging *logger) :
		Context (io_context{}),
		Acceptor (Context, tcp::endpoint (tcp::v4 (), port)),
		Buffer (bufferSize),
		Logger (logger)
	{ }

	// data structure fors holding all active connections
	std::unordered_set<TcpCntSharedPtr> Connections{};

	// provides core I/O functionality
	io_context Context;

	// accepts client connections to this server object
	tcp::acceptor Acceptor;

	// used as a placeholder for new sockets that are then std::move
	//	 to the m_connections vector
	// this acts as a sort of dummy socket for the consumer to use
	std::optional<tcp::socket> Socket{};

	/*
	 * Callbacks for when a client joins, leaves, broadcasts a message, or receives a message
	 */
	std::vector<OnJoin> OnJoined;
	std::vector<OnLeave> OnLeft;

	streambuf Buffer{ DEFAULT_BUFFER_SIZE };

	// optional logger
	const TcpLogging *Logger;
};

#pragma endregion

#pragma region CONSTRUCTOR_DESTRUCTOR

Network::TcpSrvPtr Network::TcpServer::New (const unsigned short port, const int bufferSize, const TcpLogging *logger)
{
	return TcpSrvPtr (new TcpServer (port, bufferSize, logger));	// cannot use std::make_shared here
}

Network::TcpServer::~TcpServer () = default;

Network::TcpServer::TcpServer (const unsigned short port, const int bufferSize, const TcpLogging *logger)
	: m_impl (std::make_unique<TcpServerImpl> (port, bufferSize, logger)) { }

#pragma endregion

#pragma region START_STOP

/// <summary>
/// Starts the Tcp server. This method also invokes io_context::run().
/// </summary>
boost::thread Network::TcpServer::StartThread ()
{
// create a new TcpConnection object
// tell the acceptor to accept new connections

	boost::thread t{ [&] () { StartBlocking (); } };
	return t;
}

void Network::TcpServer::StartBlocking ()
{
	InternalLogMsg ("Starting Tcp server...");
	Loop ();
	InternalLogMsg ("Tcp server now accepting connections");

	m_impl->Context.run ();
}

void Network::TcpServer::Stop () const
{
	InternalLogMsg ("Stopping Tcp server...");
	m_impl->Context.stop ();

}

#pragma endregion

#pragma region CALLBACKS

void Network::TcpServer::RegisterOnJoin (const OnJoin &onJoin) const
{
	m_impl->OnJoined.push_back (onJoin);
}

void Network::TcpServer::RegisterOnLeave (const OnLeave &onLeave) const
{
	m_impl->OnLeft.push_back (onLeave);
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

#pragma endregion

#pragma region POST

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

#pragma endregion

#pragma region LOOP

auto Network::TcpServer::Loop () -> void
{
	// the intention here is the socket will be std::move'd into the connection object
	m_impl->Socket.emplace (m_impl->Context);		// this is the socket we will be waiting on
	// a new connection should be created within the acceptor async_accept block
	//		to ensure the connection remains in scope

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


#pragma endregion

#pragma region LOGGING

void Network::TcpServer::InternalLogMsg (const std::string &msg) const
{
	InternalLogMsg (msg.c_str ());
}

void Network::TcpServer::InternalLogMsg (const char *msg) const
{
	if (m_impl->Logger)
	{
		m_impl->Logger->LogErr (msg);
	}
}

void Network::TcpServer::InternalLogErr (const std::string &err) const
{
	InternalLogMsg (err.c_str ());
}

void Network::TcpServer::InternalLogErr (const char *err) const
{
	if (m_impl->Logger)
	{
		m_impl->Logger->LogErr (err);
	}
}

#pragma endregion