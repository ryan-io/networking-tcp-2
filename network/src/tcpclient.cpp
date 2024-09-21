#include "tcpclient.h"

#include <iostream>

Network::TcpClientPtr Network::TcpClient::New (io_context &context)
{
	return TcpClientPtr (new TcpClient (context));
}

void Network::TcpClient::Open ()
{
	if (m_isOpen)
	{
		std::cout << "Client is already open\n";
		return;
	}

	m_isOpen = true;
	boost::asio::connect (m_socket, m_endpoints);
	
	// connection to server in theory is now made
	// next logical step is to handle read/write operations
	for (;;)
	{
		std::array<char, 128> buf;
		boost::system::error_code error;

		// create a boost::asio::buffer from std::array<char>
		const auto len = m_socket.read_some (buffer (buf), error);

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

Network::TcpClient::TcpClient (io_context &context) : m_context (context), m_socket (context)
{
	auto resolver = tcp::resolver (context);
	auto endpoints = resolver.resolve (DEFAULT_ADDRESS, DEFAULT_TCP_PORT_STR);
	m_endpoints = std::move (endpoints);
}
