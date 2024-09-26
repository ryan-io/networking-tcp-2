#pragma once

#include <boost/asio.hpp>

namespace Network
{
	using boost::asio::io_context;	// for verbosity
	using boost::asio::ip::tcp;		// for verbosity

	// TcpConnection is a class that represents a single connection between two entities (i.e. server & client)
	class TcpConnection : public std::enable_shared_from_this<TcpConnection>
	{
	public:
		// requires a socket to communicate between the server and client
		using Pointer = std::unique_ptr<TcpConnection>;
		using PointerShr = std::shared_ptr<TcpConnection>;
		using MessageHandler = std::function<void (const std::string &)>;
		using ErrorHandler = std::function<void (const boost::system::error_code &)>;

		// creates a new tcp connection from he provided boost::asio::io_context
		// returns a unique_ptr that can easily be converted to a shared_ptr
		// 'socket' is a rvalue reference to a tcp::socket and should be passed with std::move
		static Pointer New (tcp::socket &&, int);

		// returns a constant reference to the connection's socket
		tcp::socket &GetSocket () const;

		// returns the derived name for the connection
		std::string GetName () const;

		// kicks off the read operation and begins a new "listening loop"
		void Start (MessageHandler &&, ErrorHandler &&);

		// post a message to all clients (connections)
		// this queues the message to be sent asynchronously
		void Post (std::string &&msg);

		~TcpConnection ();

	private:
		// PIMPL Idiom
		struct TcpConnectionImpl;
		std::unique_ptr<TcpConnectionImpl> m_impl;

		// initializes the TcpConnection with the provided socket and buffer size
		// socket should be passed as a rvalue reference
		// default buffer size is 1024 bytes
		explicit TcpConnection (tcp::socket &&socket, int bufferSize = 1024);

		// waits for a new message from a client
		void AsyncRead ();

		// when a message is received, this function is called
		// any callbacks subscribed to OnRead will be called
		void OnRead (boost::system::error_code &e, size_t transferred);

		// sends a message to all clients
		void AsyncWrite ();
		void BuildAndSendMessage(std::string &&, size_t transferred) const;

		// when a message is sent, this function is called
		// any callbacks subscribed to OnWrite will be called
		void OnWrite (boost::system::error_code &e, size_t transferred);
	};
}
