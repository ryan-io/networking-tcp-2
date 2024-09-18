#pragma once

#include <boost/asio.hpp>

namespace Network {
	using boost::asio::io_context;
	using boost::asio::ip::tcp;

	// TcpConnection is a class that represents a single connection between two entities (i.e. server & client)
	struct TcpConnection : std::enable_shared_from_this<TcpConnection> {
		// requires a socket to communicate between the server and client
		using Pointer = std::unique_ptr<TcpConnection>;
		using PointerShr = std::shared_ptr<TcpConnection>;

		// creates a new tcp connection from he provided boost::asio::io_context
		// returns a unique_ptr that can easily be converted to a shared_ptr
		static Pointer New(io_context& context) {
			return Pointer(new TcpConnection(context));	// cannot use std::make_shared here
		}

		// returns a constant reference to the connection's socket
		tcp::socket& GetSocket() { return m_socket; }

		void startRead();
		void startWrite();

	private:
		explicit TcpConnection(io_context& context, const int bufferSize = 1024)
			: m_socket(context), m_buffer(std::vector<char>(bufferSize)) {}

		void handleRead(const boost::system::error_code& e, size_t transferred);
		void handleWrite(const boost::system::error_code& e, size_t transferred);

		tcp::socket m_socket;
		std::vector<char> m_buffer;
	};
}
