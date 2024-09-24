#include "tcpclient.h"

#include <iostream>
#include "tcpconnection.h"
#include "tcplogging.h"

struct Network::TcpClient::TcpClientImpl
{
	explicit TcpClientImpl (io_context &context, const TcpLogging *logger = nullptr)
		: Logger (logger), Context (context), Socket (context)
	{
		auto resolver = tcp::resolver (context);
		Endpoints = resolver.resolve (DEFAULT_ADDRESS, DEFAULT_TCP_PORT_STR);
	}

	tcp::socket &GetSocket () { return Socket; }

	bool IsOpen = false;
	const TcpLogging *Logger;
	io_context &Context;
	tcp::socket Socket;
	ip::basic_resolver_results<tcp> Endpoints;
	streambuf Buffer{ 1024 };
};

Network::TcpClientPtr Network::TcpClient::New (io_context &context)
{
	return TcpClientPtr (new TcpClient (context));
}

void Network::TcpClient::Open ()
{
	if (IsOpen ())
	{
		InternalLogMsg ("Tcp client is already open.");
		return;
	}

	SetOpen ();
	async_connect (m_impl->GetSocket (), m_impl->Endpoints,
		[this](const boost::system::error_code &error, const tcp::endpoint &endpoint)
		{
			std::cout << "Sending endpoint " << endpoint << " to " << m_impl->GetSocket ().remote_endpoint ();
			if (error)
			{
				// error handling
				std::stringstream ss;
				ss << "Error: " << error.message ();
				InternalLogErr (ss.str ());
				return;
			}

			std::cout << "Client " << m_impl->GetSocket ().remote_endpoint () << " is now opening..." << std::endl;
			InternalLogMsg ("Tcp client is now opened.");
			Loop ();
		});

	m_impl->Context.run ();
}

void Network::TcpClient::InternalLogMsg (const std::string &msg) const
{
	InternalLogMsg (msg.c_str ());
}

void Network::TcpClient::InternalLogMsg (const char *msg) const
{
	if (m_impl->Logger)
	{
		m_impl->Logger->LogErr (msg);
	}
}

void Network::TcpClient::InternalLogErr (const std::string &err) const
{
	InternalLogErr (err.c_str ());
}

void Network::TcpClient::InternalLogErr (const char *err) const
{
	if (m_impl->Logger)
	{
		m_impl->Logger->LogErr (err);
	}
}

void Network::TcpClient::Close () const
{
	if (!IsOpen ())
	{
		InternalLogMsg ("Client is now closing...");
		return;
	}

	boost::system::error_code ec;	// this will silence any errors
	SetClose ();
	m_impl->Socket.close (ec);
	m_impl->Context.stop ();

	if (ec)
	{
		// error handling

		InternalLogMsg (ec.what ());
	}
}

// takes an rvalue reference to a string and queries a Tcp connection to post it
void Network::TcpClient::Post (std::string &&message) const
{
	if (!IsOpen ())
	{
		InternalLogMsg ("Client is now opening...");

		return;
	}

	//m_impl->Connection->Post (std::move (message));
}

auto Network::TcpClient::Loop () -> void
{
	//auto ptr = m_impl->Connection->shared_from_this ();

	//TcpConnection::MessageHandler messagehandler = [this, ptr](const std::string &msg)
	//	{
	//		InternalLogMsg (msg);
	//	};

	//TcpConnection::ErrorHandler errorhandler = [this, ptr = std::weak_ptr (ptr)](const boost::system::error_code &e)
	//	{
	//		InternalLogErr (e.what ());

	//		// if our connection is still valid AND m_connections contains it
	//		if (const auto ptrLock = ptr.lock (); ptrLock)
	//		{
	//			std::stringstream ss;
	//			ss << "Connection " << ptrLock->GetName () << " was closed.";
	//			InternalLogMsg (ss.str ());
	//		}
	//	};

	//InternalLogMsg ("TcpClient starting read");
	//m_impl->Connection->Start (std::move (messagehandler), std::move (errorhandler));
}

void Network::TcpClient::SetOpen () const
{
	m_impl->IsOpen = true;
}

void Network::TcpClient::SetClose () const
{
	m_impl->IsOpen = false;
}

bool Network::TcpClient::IsOpen () const
{
	return m_impl->IsOpen;
}

void Network::TcpClient::AsyncRead () const
{
	InternalLogMsg ("TcpClient read!");
}

void Network::TcpClient::AsyncWrite ()
{ }

Network::TcpClient::TcpClient (io_context &context, const TcpLogging *logging)
	: m_impl (std::make_unique<TcpClientImpl> (context)) { }

Network::TcpClient::~TcpClient ()
{
	boost::system::error_code ec;	// silence any errors

	m_impl->Socket.close (ec);
	m_impl->Context.stop ();
}