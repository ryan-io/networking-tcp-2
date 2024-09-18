#include "tcpconnection.h"

void Network::TcpConnection::startRead()
{
	const auto boostBuffer = boost::asio::buffer(m_buffer, m_buffer.size());
	m_socket.async_read_some(boostBuffer,
		std::bind(&TcpConnection::handleRead, 
			shared_from_this(), 
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void Network::TcpConnection::startWrite()
{
}

void Network::TcpConnection::handleRead(const boost::system::error_code& e, size_t transferred)
{
}

void Network::TcpConnection::handleWrite(const boost::system::error_code& e, size_t transferred)
{
}
