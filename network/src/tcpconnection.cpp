#include "tcpconnection.h"

#include <iostream>
#include <queue>

struct Network::TcpConnection::TcpConnectionImpl
{
	static constexpr int DEFAULT_BUFFER_SIZE = 1024;
	static constexpr char DELIMITER = '\n';

	explicit TcpConnection::TcpConnectionImpl (tcp::socket &&socket, const int bufferSize = DEFAULT_BUFFER_SIZE) :
		Socket (std::move (socket)), Buffer (bufferSize)
	{
		boost::system::error_code ec;
		Name = GetName (ec);
		std::cout << "New connection: " << Name << std::endl;
	}

	// determines the name of the connection based on the remote endpoint
	std::string GetName (boost::system::error_code &ec) const
	{
		std::stringstream namestr;
		namestr
			<< Socket.local_endpoint (ec).address ().to_string ()
			<< ":"
			<< Socket.local_endpoint (ec).port ();

		return namestr.str ();
	}

	// the socket used for communication and is supplied by the consumer
	tcp::socket Socket;

	// cached buffer->used for reading and writing; default size is 1024 bytes passed to constructor
	boost::asio::streambuf Buffer;

	// UUID
	std::string Name{};

	// queue of messages to be sent to all clients
	std::queue<std::string> Messages;
};

Network::TcpConnection::Pointer Network::TcpConnection::New (tcp::socket &&socket, const int bufferSize = TcpConnectionImpl::DEFAULT_BUFFER_SIZE)
{
	return Pointer (new TcpConnection (std::move (socket), bufferSize));	// cannot use std::make_shared here
}

boost::asio::ip::tcp::socket &Network::TcpConnection::GetSocket () const { return this->m_impl->Socket; }

// starts the async read loop
void Network::TcpConnection::Start ()
{
	//std::cout << m_impl->Socket.local_endpoint().address ();
	AsyncRead ();
}

void Network::TcpConnection::Post (std::string &&msg)
{

}

Network::TcpConnection::~TcpConnection () = default;

Network::TcpConnection::TcpConnection (tcp::socket &&socket, int bufferSize)
{
	m_impl = std::make_unique<TcpConnectionImpl> (std::move (socket), bufferSize);
}

void Network::TcpConnection::AsyncRead ()
{
	async_read_until (
		m_impl->Socket,
		m_impl->Buffer,
		"\n",		// new line by default
		[ptr = shared_from_this ()] // capture a shared pointer to this object
		(boost::system::error_code e, const size_t transferred)
		{
			ptr->OnRead (e, transferred);
		}
	);
}

void Network::TcpConnection::OnRead (boost::system::error_code &e, size_t transferred)
{
	// logging
	std::cout << m_impl->GetName (e) << " - Transferred " << transferred << " bytes" << std::endl;

	if (e)
	{
		m_impl->Socket.close (e);	// Socket.close can throw an exception; pass 'e' to capture this
		std::cerr << "Error reading from socket: " << e.message () << std::endl;
		//error handler
		// logging
	
		return;
	}

	std::stringstream message;

	// boost::asio::streambuf is derived from std::streambuf
	message << std::istream (&(m_impl->Buffer)).rdbuf ();
	//m_impl->Buffer.consume (transferred);	rdbuf calls this internally -> it consumes the bytes on the buffer
	// if we DONT want the bytes to be read, we need to consume manually (do not call rdbuf for persistent)


	if (e)
	{
		m_impl->Socket.close (e);
		std::cerr << "Error reading from socket: " << e.message () << std::endl;

		// error handler
		// logging

		return;
	}

	// logging
	std::cout << "Message from " << m_impl->Name << ": " << message.str () << std::endl;

	// add message handler

	// kick the loop back off
	AsyncRead ();

}

void Network::TcpConnection::AsyncWrite ()
{ }

void Network::TcpConnection::OnWrite (boost::system::error_code &e, size_t transferred)
{ }
