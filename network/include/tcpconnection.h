#pragma once

#include <boost/asio.hpp>

namespace Network {
	using boost::asio::io_context;	// for verbosity
	using boost::asio::ip::tcp;		// for verbosity

	// TcpConnection is a class that represents a single connection between two entities (i.e. server & client)
	struct TcpConnection : std::enable_shared_from_this<TcpConnection> {
		// requires a socket to communicate between the server and client
		using Pointer = std::unique_ptr<TcpConnection>;
		using PointerShr = std::shared_ptr<TcpConnection>;

		// creates a new tcp connection from he provided boost::asio::io_context
		// returns a unique_ptr that can easily be converted to a shared_ptr
		// 'socket' is a rvalue reference to a tcp::socket and should be passed with std::move
		static Pointer New(tcp::socket&& socket, const int bufferSize = 1024) {
			return Pointer(new TcpConnection(std::move(socket), bufferSize));	// cannot use std::make_shared here
		}

		// returns a constant reference to the connection's socket
		tcp::socket& GetSocket() { return this->m_socket; }

		// kicks off the read operation and begins a new "listening loop"
		void startRead();

		// kicks off the write operation and begins a new "listening loop"
		void startWrite();

	private:
		// initializes the TcpConnection with the provided socket and buffer size
		// socket should be passed as a rvalue reference
		// default buffer size is 1024 bytes
		explicit TcpConnection(tcp::socket &&socket, const int bufferSize = 1024)
			: m_socket(std::move(socket)), m_buffer(std::vector<char>(bufferSize)) {}

		// internal->the core loop function for reading
		void handleRead(const boost::system::error_code& e, size_t transferred);

		// internal->the core loop function for writing
		void handleWrite(const boost::system::error_code& e, size_t transferred);

		// the socket used for communication and is supplied by the consumer
		tcp::socket m_socket;

		// cached buffer->used for reading and writing; default size is 1024 bytes passed to constructor
		std::vector<char> m_buffer;
	};
}
