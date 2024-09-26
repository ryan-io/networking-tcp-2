#include <iostream>

#include "tcpclient.h"
#include "tcpconnection.h"
#include "tcplogging.h"

#pragma region PIMPL_IDIOM

struct Network::TcpClient::TcpClientImpl
{
	explicit TcpClientImpl (const string_view host, const string_view port, const int bufferSize, const TcpLogging *logger = nullptr)
		: Logger (logger), Context (io_context{}), Socket (Context), Buffer (bufferSize), IsOpen (false)
	{
		// resolver allows us to determine the address from a hostname
		auto resolver = tcp::resolver (Context);
		Endpoints = resolver.resolve (host, port);
	}

	tcp::socket &GetSocket () { return Socket; }

	const TcpLogging *Logger;
	io_context Context; 
	tcp::socket Socket;
	tcp::resolver::results_type Endpoints;
	streambuf Buffer;
	bool IsOpen;
};

#pragma endregion

#pragma region OPEN_CLOSE

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

#pragma endregion

#pragma region WRITE

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

void Network::TcpClient::AsyncWrite ()
{ }

#pragma endregion

#pragma region READ

void Network::TcpClient::AsyncRead () const
{
	InternalLogMsg ("TcpClient read!");
}

#pragma endregion

#pragma region LOGGING

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

#pragma endregion

#pragma region HELPERS

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

#pragma endregion

#pragma region CONSTRUCTOR_DESTRUCTOR

Network::TcpClientPtr Network::TcpClient::New (const string_view host, const string_view port, const int bufferSize, const TcpLogging *logger)
{
	return TcpClientPtr (new TcpClient (host, port, bufferSize, logger));
}

Network::TcpClient::TcpClient (const string_view host, const string_view port, const int bufferSize, const TcpLogging *logger)
	: m_impl (std::make_unique<TcpClientImpl> (host, port, bufferSize, logger)) { }

Network::TcpClient::~TcpClient ()
{
	boost::system::error_code ec;	// silence any errors

	m_impl->Socket.close (ec);
	m_impl->Context.stop ();
}

#pragma endregion