#include "tcpconnection.h"

Network::TcpConnection::Pointer Network::TcpConnection::New (tcp::socket &&socket, const int bufferSize)
{
	return Pointer (new TcpConnection (std::move (socket), bufferSize));	// cannot use std::make_shared here
}

void Network::TcpConnection::startRead ()
{
	const auto boostBuffer = boost::asio::buffer (m_buffer, m_buffer.size ());
	m_socket.async_read_some (boostBuffer,
		std::bind (&TcpConnection::handleRead,
			shared_from_this (),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void Network::TcpConnection::startWrite ()
{ }

Network::TcpConnection::TcpConnection (tcp::socket &&socket, const int bufferSize)
	: m_socket (std::move (socket)), m_buffer (std::vector<char> (bufferSize))
{
	boost::system::error_code ec;
	m_name = GetName (ec);

}

void Network::TcpConnection::handleRead (const boost::system::error_code &e, size_t transferred)
{ }

void Network::TcpConnection::handleWrite (const boost::system::error_code &e, size_t transferred)
{ }

std::string Network::TcpConnection::GetName (boost::system::error_code &ec) const
{
	std::stringstream namestr;
	namestr
		<< m_socket.remote_endpoint (ec).address ().to_string ()
		<< ":"
		<< m_socket.remote_endpoint (ec).port ();

	return namestr.str ();
}

boost::asio::ip::tcp::socket &Network::TcpConnection::GetSocket () { return this->m_socket; }
