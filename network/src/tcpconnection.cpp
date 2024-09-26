#include "tcpconnection.h"

#include <iostream>
#include <queue>
#include <boost/thread/thread.hpp>

struct Network::TcpConnection::TcpConnectionImpl
{
	static constexpr int DEFAULT_BUFFER_SIZE = 1024;
	static constexpr char DELIMITER = '\n';

	explicit TcpConnection::TcpConnectionImpl (tcp::socket &&socket, const int bufferSize = DEFAULT_BUFFER_SIZE) :
		Socket (std::move (socket)), Buffer (bufferSize)
	{
		boost::system::error_code ec;
		Name = GetName (ec);
	}

	// determines the name of the connection based on the remote endpoint
	std::string GetName (boost::system::error_code &ec) const
	{
		std::stringstream namestr;
		namestr << Socket.remote_endpoint (ec);

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

	MsgHandler MessageHandler;
	ErrHandler ErrorHandler;
};

Network::TcpConnection::TcpCntPtr Network::TcpConnection::New (tcp::socket &&socket, const int bufferSize = TcpConnectionImpl::DEFAULT_BUFFER_SIZE)
{
	return TcpCntPtr (new TcpConnection (std::move (socket), bufferSize));	// cannot use std::make_shared here
}

boost::asio::ip::tcp::socket &Network::TcpConnection::GetSocket () const { return this->m_impl->Socket; }

std::string Network::TcpConnection::GetName () const
{
	return m_impl->Name;
}

// starts the async read loop
void Network::TcpConnection::Start (MsgHandler &&msgHndl, ErrHandler &&errHandl)
{
	try
	{
		m_impl->ErrorHandler = std::move (errHandl);
		m_impl->MessageHandler = std::move (msgHndl);

		boost::thread readThread{ [this] ()
		{
			AsyncRead ();
		} };

		boost::thread writeThread{ [this] ()
		{
			//AsyncWrite ();
		} };

		readThread.join ();
		writeThread.join ();
	}
	catch (std::exception &)
	{
		m_impl->Socket.close ();
	}

	//AsyncRead ();
}

void Network::TcpConnection::Post (std::string &&msg)
{
	const bool idle = m_impl->Messages.empty ();	// this check technically delays execution by '1 frame'

	m_impl->Messages.push (std::move (msg));

	if (idle)
	{
		AsyncWrite ();
	}
}

Network::TcpConnection::~TcpConnection ()
{
	m_impl->Socket.close ();
}

void Network::TcpConnection::BuildAndSendMessage (const std::string &msg, size_t transferred) const
{
	m_impl->MessageHandler (msg);
}

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
	if (e)
	{
		m_impl->Socket.close (e);	// Socket.close can throw an exception; pass 'e' to capture this

		if (m_impl->ErrorHandler)
		{
			m_impl->ErrorHandler (e);
		}

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

		if (m_impl->ErrorHandler)
		{
			m_impl->ErrorHandler (e);
		}

		return;
	}

	// logging
	if (m_impl->MessageHandler)
	{
		BuildAndSendMessage (message.str(), transferred);
	}

	// kick the loop back off
	AsyncRead ();
}

void Network::TcpConnection::AsyncWrite ()
{
	async_write (
		m_impl->Socket,
		boost::asio::buffer (m_impl->Messages.front ()),
		[ptr = shared_from_this ()] // capture a shared pointer to this object
		(boost::system::error_code e, const size_t transferred)
		{
			ptr->OnWrite (e, transferred);
		}
	);
}

void Network::TcpConnection::OnWrite (boost::system::error_code &e, size_t transferred)
{
	// logging
	if (m_impl->MessageHandler)
	{
		BuildAndSendMessage ("Write", transferred);
	}

	if (e)
	{
		m_impl->Socket.close (e);	// Socket.close can throw an exception; pass 'e' to capture this

		if (m_impl->ErrorHandler)
		{
			m_impl->ErrorHandler (e);
		}

		return;
	}

	// cache/pop the first message in the queue
	auto msg = m_impl->Messages.front ();
	m_impl->Messages.pop ();

	if (!m_impl->Messages.empty ())
	{
		AsyncWrite ();
	}
}
