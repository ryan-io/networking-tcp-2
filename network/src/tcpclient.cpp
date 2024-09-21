#include "tcpclient.h"

#include <iostream>

struct Network::TcpClient::TcpClientImpl
{
	explicit TcpClientImpl (io_context &context) : Context (context), Socket (context)
	{
		auto resolver = tcp::resolver (context);
		auto endpoints = resolver.resolve (DEFAULT_ADDRESS, DEFAULT_TCP_PORT_STR);
		Endpoints = std::move (endpoints);
	}

	bool IsOpen = false; 
	io_context &Context;
	tcp::socket Socket;
	ip::basic_resolver_results<tcp> Endpoints;
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
	boost::asio::connect (m_impl->Socket, m_impl->Endpoints);

	// connection to server in theory is now made
	// next logical step is to handle read/write operations
	for (;;)
	{
		std::array<char, 128> buf;
		boost::system::error_code error;

		// create a boost::asio::buffer from std::array<char>
		const auto len = m_impl->Socket.read_some (buffer (buf), error);

		if (error == error::eof)	// this is good, simply break
		{
			break;	// Connection closed cleanly by peer.
		}

		if (error)	// all other errors, throw
		{
			throw boost::system::system_error (error); // Some other error.
		}

		std::cout.write (buf.data (), static_cast<long long>(len));
	}
}

Network::TcpClient::TcpClient (io_context &context)
	: m_impl (std::make_unique<TcpClientImpl> (context)){}

Network::TcpClient::~TcpClient ()
{
	m_impl->Socket.close ();
	m_impl->Context.stop ();
}