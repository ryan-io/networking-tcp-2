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
		// requires a Socket to communicate between the server and client
		using TcpCntPtr = std::unique_ptr<TcpConnection>;
		using TcpCntPtrShrd = std::shared_ptr<TcpConnection>;
		using MsgHandler = std::function<void (const std::string &)>;
		using ErrHandler = std::function<void (const boost::system::error_code &)>;

		// creates a new tcp connection from he provided boost::asio::io_context
		// returns a unique_ptr that can easily be converted to a shared_ptr
		// 'Socket' is a rvalue reference to a tcp::Socket and should be passed with std::move
		static TcpCntPtr New (tcp::socket &&, int);

		// returns a constant reference to the connection's Socket
		tcp::socket &GetSocket () const;

		// returns the derived name for the connection
		std::string GetName () const;

		// kicks off the read operation and begins a new "listening loop"
		void Start (MsgHandler &&, ErrHandler &&);

		// post a message to all clients (connections)
		// this queues the message to be sent asynchronously
		void Post (std::string &&msg);

		~TcpConnection ();

	private:
		// PIMPL Idiom
		struct TcpConnectionImpl;
		std::unique_ptr<TcpConnectionImpl> m_impl;

		// initializes the TcpConnection with the provided Socket and Buffer size
		// Socket should be passed as a rvalue reference
		// default Buffer size is 1024 bytes
		explicit TcpConnection (tcp::socket &&socket, int bufferSize = 1024);

		// waits for a new message from a client
		void AsyncRead ();

		// when a message is received, this function is called
		// any callbacks subscribed to OnRead will be called
		void OnRead (boost::system::error_code &e, size_t transferred);

		// sends a message to all clients
		void AsyncWrite ();

		// when a message is sent, this function is called
		// any callbacks subscribed to OnWrite will be called
		void OnWrite (boost::system::error_code &e, size_t transferred);

		// for invoking any message handler or processing a message before outputting to a stream
		void BuildAndSendMessage (const std::string &msg, size_t transferred) const;
	};
}