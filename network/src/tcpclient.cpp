#include "tcpclient.h"

#include <iostream>
#include "tcpconnection.h"
#include "tcplogging.h"

struct Network::TcpClient::TcpClientImpl
{
	explicit TcpClientImpl (io_context &context, const TcpLogging *logger = nullptr)
		: Logger (logger), Context (context), Socket (context)
	{
		// resolver allows us to determine the address from a hostname
		auto resolver = tcp::resolver (context);
		Endpoints = resolver.resolve (DEFAULT_ADDRESS, DEFAULT_TCP_PORT_STR);
	}

	tcp::socket &GetSocket () { return Socket; }

	bool IsOpen = false;
	const TcpLogging *Logger;
	io_context &Context;
	tcp::socket Socket;
	tcp::resolver::results_type Endpoints;
	streambuf Buffer{ 1024 };
};

Network::TcpClientPtr Network::TcpClient::New (io_context &context)
{
	return TcpClientPtr (new TcpClient (context));
}

void Network::TcpClient::Open () const
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
			std::cout << "Welcome " << m_impl->GetSocket ().remote_endpoint () << "!\n";
			if (error)
			{
				// error handling
				std::stringstream ss;
				ss << "Error: " << error.message ();
				InternalLogErr (ss.str ());
				return;
			}

			InternalLogMsg ("Tcp client is now opened.");
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
		InternalLogErr ("Client needs to be opened before sending messages.");
		return;
	}

	async_write (m_impl->Socket, boost::asio::buffer (message),
		[this](const boost::system::error_code &error, size_t bytes_transferred)
		{
			std::stringstream ss;
			if (error)
			{
				// error handling
				ss << "Error: " << error.message ();
				InternalLogErr (ss.str ());
			}
			else
			{
				ss << "Sent " << bytes_transferred << " bytes.";
				InternalLogMsg (ss.str ());
			}
		});
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