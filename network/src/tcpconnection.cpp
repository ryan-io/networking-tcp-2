#include "tcpconnection.h"

void Network::TcpConnection::startRead(const int bufferSize)
{
	std::vector<int> buffer(bufferSize);
	const auto boostBuffer = boost::asio::buffer(buffer, bufferSize);
	m_socket.async_read_some(boostBuffer,
		std::bind(&TcpConnection::handleRead, 
			shared_from_this(), 
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void Network::TcpConnection::startWrite(const int bufferSize)
{
}

void Network::TcpConnection::handleRead(const boost::system::error_code& e, size_t transferred)
{
}

void Network::TcpConnection::handleWrite(const boost::system::error_code& e, size_t transferred)
{
}
