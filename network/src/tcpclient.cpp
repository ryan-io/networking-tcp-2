#include "tcpclient.h"

#include <iostream>
#include "tcpconnection.h"

struct Network::TcpClient::TcpClientImpl
{
	explicit TcpClientImpl (io_context &context) : Context (context)
	{
		auto resolver = tcp::resolver (context);
		auto endpoints = resolver.resolve (DEFAULT_ADDRESS, DEFAULT_TCP_PORT_STR);
		auto socket = tcp::socket (context);

		Endpoints = std::move (endpoints);  
		Connection = static_cast<TcpCntSharedPtr> (TcpConnection::New (std::move (socket), 1024));
	}

	bool IsOpen = false;
	io_context &Context;
	ip::basic_resolver_results<tcp> Endpoints;
	TcpCntSharedPtr Connection;
	streambuf Buffer{ 1024 };
};

Network::TcpClientPtr Network::TcpClient::New (io_context &context)
{
	return TcpClientPtr (new TcpClient (context));
}

void Network::TcpClient::Open ()
{
	if (m_impl->IsOpen)
	{
		std::cout << "Client is already open\n";
		return;
	}

	m_impl->IsOpen = true;
	async_connect (m_impl->Connection->GetSocket (), m_impl->Endpoints,
		[this](const boost::system::error_code &error, const tcp::endpoint &endpoint)
		{
			if (error)
			{
				// error handling
				std::cerr << "Error: " << error.message () << '\n';
				return;
			}
			std::cout << "TcpClient is now opened.";
			Loop ();
		});

	m_impl->Context.run ();
}

void Network::TcpClient::Close () const
{
	if (!m_impl->IsOpen)
	{
		// logging
		return;
	}

	boost::system::error_code ec;	// this will silence any errors
	m_impl->IsOpen = false;
	m_impl->Connection->GetSocket ().close (ec);

	if (ec)
	{
		// error handling
	}
}

// takes an rvalue reference to a string and queries a Tcp connection to post it
void Network::TcpClient::Post (std::string &&message) const
{
	if (!m_impl->IsOpen)
	{
		// logging
		return;
	}

	m_impl->Connection->Post (std::move (message));
}

auto Network::TcpClient::Loop () -> void
{
	auto ptr = m_impl->Connection->shared_from_this ();

	TcpConnection::MessageHandler messagehandler = [this, ptr](const std::string &msg)
		{
			std::cout << "Received message: " << msg << '\n';
		};

	TcpConnection::ErrorHandler errorhandler = [this, ptr = std::weak_ptr (ptr)](const boost::system::error_code &e)
		{
			std::cerr << "Error: " << e.what () << '\n';

			// if our connection is still valid AND m_connections contains it
			if (const auto ptrLock = ptr.lock (); ptrLock)
			{
				std::cout << "Connection " << ptrLock->GetName () << " was closed.\n";
				//RelayOnLeft (ptrLock);
			}
		};

	std::cout << "TcpClient starting read\n";
	m_impl->Connection->Start (std::move (messagehandler), std::move (errorhandler));

	//m_impl->Socket. (m_impl->Context);		// this is the socket we will be waiting on

	// a new connection should be created within the acceptor async_accept block
	//		to ensure the connection remains in scope

	// we cast this to a shared_pointer because this connection can/will be cached
	// at this point, member variable m_socket of 'connection' is a valid socket


}

void Network::TcpClient::AsyncRead () const
{
	std::cout << "TcpClient read\n";
}

void Network::TcpClient::AsyncWrite ()
{ }

Network::TcpClient::TcpClient (io_context &context)
	: m_impl (std::make_unique<TcpClientImpl> (context)) { }

Network::TcpClient::~TcpClient ()
{
	boost::system::error_code ec;	// silence any errors

	m_impl->Connection->GetSocket ().close (ec);
	m_impl->Context.stop ();
}